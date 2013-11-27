#include "mgjsonstorage.h"
#include "mgstorage.h"
#include <vector>
#include <mglog.h>

#if !defined(_MSC_VER) || _MSC_VER > 1200
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

//! JSON序列化适配器类，内部实现类
class MgJsonStorage::Impl : public MgStorage
{
public:
    Impl() : _fs(NULL), _err(NULL) {}
    virtual ~Impl() { if (_fs) delete(_fs); }
    
    void clear();
    const char* stringify(bool pretty);
    Document& document() { return _doc; }
    const char* getError() { return _err ? _err : _doc.GetParseError(); }
    FileStream& createStream(FILE* fp);
    bool save(FILE* fp, bool pretty);
    
private:
    bool readNode(const char* name, int index, bool ended);
    bool writeNode(const char* name, int index, bool ended);
    bool setError(const char* err);
    
    int readInt(const char* name, int defvalue);
    bool readBool(const char* name, bool defvalue);
    float readFloat(const char* name, float defvalue);
    int readFloatArray(const char* name, float* values, int count);
    int readString(const char* name, char* value, int count);
    
    void writeInt(const char* name, int value);
    void writeUInt(const char* name, int value);
    void writeBool(const char* name, bool value);
    void writeFloat(const char* name, float value);
    void writeFloatArray(const char* name, const float* values, int count);
    void writeString(const char* name, const char* value);
    
private:
    Document _doc;
    std::vector<Value*> _stack;
    StringBuffer _strbuf;
    FileStream  *_fs;
    const char* _err;
    int _nodeCount;
};

#endif

MgJsonStorage::MgJsonStorage() : _impl(NULL)
{
#ifdef RAPIDJSON_DOCUMENT_H_
    _impl = new Impl();
#endif
}

MgJsonStorage::~MgJsonStorage()
{
#ifdef RAPIDJSON_DOCUMENT_H_
    delete _impl;
#endif
}

const char* MgJsonStorage::stringify(bool pretty)
{
#ifdef RAPIDJSON_DOCUMENT_H_
    return _impl->stringify(pretty);
#else
    pretty;
    return "";
#endif
}

bool MgJsonStorage::save(FILE* fp, bool pretty)
{
    return fp && !_impl->document().IsNull() && _impl->save(fp, pretty);
}

MgStorage* MgJsonStorage::storageForRead(const char* content)
{
#ifdef RAPIDJSON_DOCUMENT_H_
    _impl->clear();
    if (content && *content) {
        _impl->document().Parse<0>(content);
        if (_impl->getError()) {
            LOGE("parse error: %s", _impl->getError());
        }
    }
    
    return _impl;
#else
    content;
    return NULL;
#endif
}

MgStorage* MgJsonStorage::storageForRead(FILE* fp)
{
#ifdef RAPIDJSON_DOCUMENT_H_
    _impl->clear();
    if (fp) {
        _impl->document().ParseStream<0>(_impl->createStream(fp));
        if (_impl->getError()) {
            LOGE("parse error: %s", _impl->getError());
        }
    }
#else
    fp;
#endif
    return _impl;
}

void MgJsonStorage::clear()
{
    _impl->clear();
}

const char* MgJsonStorage::getParseError()
{
#ifdef RAPIDJSON_DOCUMENT_H_
    return _impl->getError();
#else
    return "";
#endif
}

MgStorage* MgJsonStorage::storageForWrite()
{
#ifdef RAPIDJSON_DOCUMENT_H_
    _impl->clear();
    return _impl;
#else
    return NULL;
#endif
}

#ifdef RAPIDJSON_DOCUMENT_H_

void MgJsonStorage::Impl::clear()
{
    _doc.SetNull();
    _stack.clear();
    _strbuf.Clear();
    _nodeCount = 0;
    if (_fs) {
        delete _fs;
        _fs = NULL;
    }
}

FileStream& MgJsonStorage::Impl::createStream(FILE* fp)
{
    if (_fs) delete _fs;
    _fs = new FileStream(fp);
    return *_fs;
}

bool MgJsonStorage::Impl::setError(const char* err)
{
    _err = err;
    if (err) {
        LOGE("storage error: %s", err);
    }
    return false;
}

bool MgJsonStorage::Impl::readNode(const char* name, int index, bool ended)
{
    if (!ended) {                       // 开始一个新节点
        char tmpname[32];
        if (index >= 0) {               // 形成实际节点名称
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
            sprintf_s(tmpname, sizeof(tmpname), "%s%d", name, index + 1);
#else
            sprintf(tmpname, "%s%d", name, index + 1);
#endif
            name = tmpname;
        }
        
        if (_stack.empty()) {
            if (!_doc.IsObject() || !_doc.HasMember(name)) {
                return false;           // 没有此节点
            }
            _stack.push_back(&_doc[name]);  // 当前JSON对象压栈
            _err = NULL;
        }
        else {
            Value &parent = *_stack.back();
            if (!parent.IsObject() || !parent.HasMember(name)) {
                return false;
            }
            _stack.push_back(&parent[name]);
        }
    }
    else {                              // 当前节点读取完成
        if (!_stack.empty()) {
            _stack.pop_back();          // 出栈
        }
        if (_stack.empty()) {           // 根节点已出栈
            clear();
        }
        _nodeCount++;
    }
    
    return true;
}

bool MgJsonStorage::Impl::writeNode(const char* name, int index, bool ended)
{
    if (!ended) {                       // 开始一个新节点
        char tmpname[32];
        if (index >= 0) {               // 形成实际节点名称
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
            sprintf_s(tmpname, sizeof(tmpname), "%s%d", name, index + 1);
#else
            sprintf(tmpname, "%s%d", name, index + 1);
#endif
            name = tmpname;
        }
        
        Value tmpnode(kObjectType);
        Value namenode(name, _doc.GetAllocator());  // 节点名是临时串，要复制
        
        if (_stack.empty()) {
            _doc.SetObject();
            _doc.AddMember(namenode, tmpnode, _doc.GetAllocator());
            _stack.push_back(&(_doc.MemberEnd() - 1)->value); // 新节点压栈
            _err = NULL;
        }
        else {
            Value &parent = *_stack.back();
            parent.AddMember(namenode, tmpnode, _doc.GetAllocator());
            _stack.push_back(&(parent.MemberEnd() - 1)->value);
        }
    }
    else {                              // 当前节点写完
        if (!_stack.empty()) {
            _stack.pop_back();          // 出栈
        }
        _nodeCount++;
    }
    
    return true;
}

const char* MgJsonStorage::Impl::stringify(bool pretty)
{
    if (_strbuf.Size() == 0 && !_doc.IsNull()) {
        Document::AllocatorType allocator;
        
        if (pretty) {
            PrettyWriter<StringBuffer> writer(_strbuf, &allocator);
            _doc.Accept(writer);    // DOM树转换到文本流
        }
        else {
            Writer<StringBuffer> writer(_strbuf, &allocator);
            _doc.Accept(writer);
        }
        _doc.SetNull();
        _stack.clear();
    }
    
    return _strbuf.GetString();
}

bool MgJsonStorage::Impl::save(FILE* fp, bool pretty)
{
    Document::AllocatorType allocator;
    
    if (_nodeCount < 100) {
        if (pretty) {
            PrettyWriter<StringBuffer> writer(_strbuf, &allocator);
            _doc.Accept(writer);
        }
        else {
            Writer<StringBuffer> writer(_strbuf, &allocator);
            _doc.Accept(writer);
        }
        fputs(_strbuf.GetString(), fp);
    } else {
        FileStream& fs = createStream(fp);
        
        if (pretty) {
            PrettyWriter<FileStream> writer(fs, &allocator);
            document().Accept(writer);
        }
        else {
            Writer<FileStream> writer(fs, &allocator);
            document().Accept(writer);
        }
    }
    clear();
    
    return true;
}

int MgJsonStorage::Impl::readInt(const char* name, int defvalue)
{
    int ret = defvalue;
    Value *node = _stack.empty() ? NULL : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = (*node)[name];
        
        if (item.IsInt()) {
            ret = item.GetInt();
        }
        else if (item.IsUint()) {
            ret = item.GetUint();
        }
        else {
            LOGD("Invalid value for readInt(%s)", name);
        }
    }
    
    return ret;
}

bool MgJsonStorage::Impl::readBool(const char* name, bool defvalue)
{
    bool ret = defvalue;
    Value *node = _stack.empty() ? NULL : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsBool()) {
            ret = item.GetBool();
        }
        else {
            LOGD("Invalid value for readBool(%s)", name);
        }
    }
    
    return ret;
}

float MgJsonStorage::Impl::readFloat(const char* name, float defvalue)
{
    float ret = defvalue;
    Value *node = _stack.empty() ? NULL : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsDouble()) {
            ret = (float)item.GetDouble();
        }
        else if (item.IsInt()) {    // 浮点数串可能没有小数点，需要判断整数
            ret = (float)item.GetInt();
        }
        else {
            LOGD("Invalid value for readFloat(%s)", name);
        }
    }
    
    return ret;
}

int MgJsonStorage::Impl::readFloatArray(const char* name, float* values, int count)
{
    int ret = 0;
    Value *node = _stack.empty() ? NULL : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsArray()) {
            ret = item.Size();
            if (values) {
                int n = ret < count ? ret : count;
                ret = 0;
                for (int i = 0; i < n; i++) {
                    const Value &v = item[i];
                    
                    if (v.IsDouble()) {
                        values[ret++] = (float)v.GetDouble();
                    }
                    else if (v.IsInt()) {
                        values[ret++] = (float)v.GetInt();
                    }
                    else {
                        LOGD("Invalid value for readFloatArray(%s)", name);
                    }
                }
            }
        }
        else {
            LOGD("Invalid value for readFloatArray(%s)", name);
        }
    }
    if (values && ret < count) {
        setError("readFloatArray: lose numbers.");
    }
    
    return ret;
}

int MgJsonStorage::Impl::readString(const char* name, char* value, int count)
{
    int ret = 0;
    Value *node = _stack.empty() ? NULL : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsString()) {
            ret = item.GetStringLength();
            if (value) {
                ret = ret < count ? ret : count;
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
                strncpy_s(value, count, item.GetString(), ret);
#else
                strncpy(value, item.GetString(), ret);
#endif
            }
        }
        else {
            LOGD("Invalid value for readString(%s)", name);
        }
    }
    
    return ret;
}

void MgJsonStorage::Impl::writeInt(const char* name, int value)
{
    _stack.back()->AddMember(name, value, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeUInt(const char* name, int value)
{
    _stack.back()->AddMember(name, (unsigned)value, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeBool(const char* name, bool value)
{
    _stack.back()->AddMember(name, value, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeFloat(const char* name, float value)
{
    _stack.back()->AddMember(name, (double)value, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeFloatArray(const char* name, const float* values, int count)
{
    Value node(kArrayType);
    
    for (int i = 0; i < count; i++) {
        node.PushBack((double)values[i], _doc.GetAllocator());
    }
    _stack.back()->AddMember(name, node, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeString(const char* name, const char* value)
{
    _stack.back()->AddMember(name, value ? value : "", _doc.GetAllocator());
}

#endif
