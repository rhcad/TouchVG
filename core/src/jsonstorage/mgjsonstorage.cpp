#include "mgjsonstorage.h"
#include "mgstorage.h"
#include <vector>
#include "mglog.h"
#include "utf8_unchecked.h"
#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "rapidjson/filestream.h"   // wrapper of C stream for prettywriter as output
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

//! JSON序列化适配器类，内部实现类
class MgJsonStorage::Impl : public MgStorage
{
public:
    Impl() : _fs((FileStream *)0), _err((const char*)0), _arrmode(false), _numAsStr(false) {}
    virtual ~Impl() { if (_fs) delete(_fs); }
    
    void clear();
    const char* stringify(bool pretty);
    Document& document() { return _doc; }
    const char* getError() { return _err ? _err : _doc.GetParseError(); }
    FileStream& createStream(FILE* fp);
    bool save(FILE* fp, bool pretty);
    void setArrayMode(bool arr) { _arrmode = arr; }
    void saveNumberAsString(bool str) { _numAsStr = str; }
    
private:
    bool readNode(const char* name, int index, bool ended);
    bool writeNode(const char* name, int index, bool ended);
    bool setError(const char* err);
    
    int readInt(const char* name, int defvalue);
    bool readBool(const char* name, bool defvalue);
    float readFloat(const char* name, float defvalue);
    double readDouble(const char* name, double defvalue);
    int readFloatArray(const char* name, float* values, int count, bool report = true);
    int readDoubleArray(const char* name, double* values, int count, bool report = true);
    int readString(const char* name, char* value, int count);
    
    void writeInt(const char* name, int value);
    void writeUInt(const char* name, int value);
    void writeBool(const char* name, bool value);
    void writeFloat(const char* name, float value);
    void writeDouble(const char* name, double value);
    void writeFloatArray(const char* name, const float* values, int count);
    void writeDoubleArray(const char* name, const double* values, int count);
    void writeString(const char* name, const char* value);
    int readIntArray(const char* name, int* values, int count, bool report = true);
    void writeIntArray(const char* name, const int* values, int count);
    
    bool hasNum(const char* name) { return strspn(name, "01234567890") > 0; }
    
private:
    Document _doc;
    std::vector<Value*> _stack;
    std::vector<Value*> _created;
    StringBuffer _strbuf;
    FileStream  *_fs;
    const char* _err;
    int _nodeCount;
    bool _arrmode;
    bool _numAsStr;
};

MgJsonStorage::MgJsonStorage() : _impl(new Impl())
{
}

MgJsonStorage::~MgJsonStorage()
{
    delete _impl;
}

const char* MgJsonStorage::stringify(bool pretty)
{
    return _impl->stringify(pretty);
}

bool MgJsonStorage::save(FILE* fp, bool pretty)
{
    return fp && !_impl->document().IsNull() && _impl->save(fp, pretty);
}

void MgJsonStorage::setArrayMode(bool arr)
{
    _impl->setArrayMode(arr);
}

void MgJsonStorage::saveNumberAsString(bool str)
{
    _impl->saveNumberAsString(str);
}

MgStorage* MgJsonStorage::storageForRead(const char* content)
{
    _impl->clear();
    if (content && *content) {
        _impl->document().Parse<0>(content);
        if (_impl->getError()) {
            LOGE("parse error: %s", _impl->getError());
        }
    }
    
    return _impl;
}

MgStorage* MgJsonStorage::storageForRead(FILE* fp)
{
    _impl->clear();
    if (fp) {
        utf8::uint8_t head[3];
        fread(head, 1, sizeof(head), fp);
        if (!utf8::starts_with_bom(head, head + sizeof(head)))
            fseek(fp, 0, SEEK_SET);
        _impl->document().ParseStream<0>(_impl->createStream(fp));
        if (_impl->getError()) {
            LOGE("parse error: %s", _impl->getError());
        }
    }
    
    return _impl;
}

void MgJsonStorage::clear()
{
    _impl->clear();
}

const char* MgJsonStorage::getParseError()
{
    return _impl->getError();
}

MgStorage* MgJsonStorage::storageForWrite()
{
    _impl->clear();
    return _impl;
}

void MgJsonStorage::Impl::clear()
{
    _doc.SetNull();
    _stack.clear();
    _strbuf.Clear();
    _nodeCount = 0;
    if (_fs) {
        delete _fs;
        _fs = (FileStream *)0;
    }
    for (size_t i = 0; i < _created.size(); i++) {
        delete _created[i];
    }
    _created.clear();
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
    if (_doc.IsNull()) {
        return false;
    }
    if (!ended) {                       // 开始一个新节点
        char tmpname[32];
        
        if (name && index >= 0) {       // 形成实际节点名称
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
            sprintf_s(tmpname, sizeof(tmpname), "%s%d", name, index + 1);
#else
            sprintf(tmpname, "%s%d", name, index + 1);
#endif
            name = tmpname;
        }
        
        if (_stack.empty()) {
            if (name && *name) {
                if (!_doc.IsObject() || !_doc.HasMember(name)) {
                    return false;           // 没有此节点
                }
                _stack.push_back(&_doc[name]);  // 当前JSON对象压栈
            } else {
                _stack.push_back(&_doc);
            }
            _err = (const char*)0;
        }
        else {
            Value &parent = *_stack.back();
            if (parent.IsArray() && index >= 0 && index < (int)parent.Size()) {
                _stack.push_back(&parent[index]);
            }
            else if (parent.IsObject() && parent.HasMember(name)) {
                _stack.push_back(&parent[name]);
            }
            else {
                return false;
            }
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
        
        if (_stack.empty() && (!name || !*name)) {
            _doc.SetObject();
            _stack.push_back(&_doc);
            _err = (const char*)0;
            return true;
        }
        
        Value tmpnode(kObjectType);
        
        if (index >= 0 && _arrmode) {
            Value &parent = *_stack.back();
            if (!parent.IsArray())
                parent.SetArray();
            parent.PushBack(tmpnode, _doc.GetAllocator());
            _stack.push_back(parent.End() - 1);
            return true;
        }
        
        Value namenode(name, _doc.GetAllocator());  // 节点名是临时串，要复制
        
        if (_stack.empty()) {
            _doc.SetObject();
            _doc.AddMember(namenode, tmpnode, _doc.GetAllocator());
            _stack.push_back(&(_doc.MemberEnd() - 1)->value); // 新节点压栈
            _err = (const char*)0;
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
    _strbuf.Clear();
    
    return true;
}

bool MgJsonStorage::parseInt(const char* str, int& value)
{
    char *endptr;
    value = (int)strtoul(str, &endptr, 0);
    return !endptr || !*endptr;
}

int MgJsonStorage::Impl::readInt(const char* name, int defvalue)
{
    int ret = defvalue;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = (*node)[name];
        
        if (item.IsInt()) {
            ret = item.GetInt();
        }
        else if (item.IsUint()) {
            ret = item.GetUint();
        }
        else if (item.IsBool()) {
            ret = item.GetBool() ? 1 : 0;
        }
        else if (item.IsString() && MgJsonStorage::parseInt(item.GetString(), defvalue)) {
            ret = defvalue;
        }
        else {
            LOGD("Invalid value for readInt(%s)", name);
        }
    }
    
    return ret;
}

bool MgJsonStorage::Impl::readBool(const char* name, bool defvalue)
{
    return !!readInt(name, defvalue ? 1 : 0);
}

bool MgJsonStorage::parseFloat(const char* str, float& value)
{
    char *endptr;
    value = (float)strtod(str, &endptr);
    return !endptr || !*endptr;
}

bool MgJsonStorage::parseFloat(const char* str, double& value)
{
    char *endptr;
    value = strtod(str, &endptr);
    return !endptr || !*endptr;
}

float MgJsonStorage::Impl::readFloat(const char* name, float defvalue)
{
    float ret = defvalue;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsDouble()) {
            ret = (float)item.GetDouble();
        }
        else if (item.IsInt()) {    // 浮点数串可能没有小数点，需要判断整数
            ret = (float)item.GetInt();
        }
        else if (item.IsString() && MgJsonStorage::parseFloat(item.GetString(), defvalue)) {
            ret = defvalue;
        }
        else {
            LOGD("Invalid value for readFloat(%s)", name);
        }
    }
    
    return ret;
}

double MgJsonStorage::Impl::readDouble(const char* name, double defvalue)
{
    double ret = defvalue;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsDouble()) {
            ret = item.GetDouble();
        }
        else if (item.IsInt()) {    // 浮点数串可能没有小数点，需要判断整数
            ret = item.GetInt();
        }
        else if (item.IsString() && MgJsonStorage::parseFloat(item.GetString(), defvalue)) {
            ret = defvalue;
        }
        else {
            LOGD("Invalid value for readFloat(%s)", name);
        }
    }
    
    return ret;
}

int MgJsonStorage::Impl::readFloatArray(const char* name, float* values,
                                        int count, bool report)
{
    int ret = 0;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    report = report && count > 0 && values;
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
                    else if (v.IsString() && MgJsonStorage::parseFloat(v.GetString(), values[ret])) {
                        ret++;
                    }
                    else if (report) {
                        LOGD("Invalid value for readFloatArray(%s)", name);
                    }
                }
            }
        }
        else if (report) {
            LOGD("Invalid value for readFloatArray(%s)", name);
        }
    }
    if (values && ret < count && report) {
        LOGD("readFloatArray(%s, %d): %d", name, count, ret);
        setError("readFloatArray: lose numbers");
    }
    
    return ret;
}

int MgJsonStorage::Impl::readDoubleArray(const char* name, double* values,
                                         int count, bool report)
{
    int ret = 0;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    report = report && count > 0 && values;
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
                    else if (v.IsString() && MgJsonStorage::parseFloat(v.GetString(), values[ret])) {
                        ret++;
                    }
                    else if (report) {
                        LOGD("Invalid value for readFloatArray(%s)", name);
                    }
                }
            }
        }
        else if (report) {
            LOGD("Invalid value for readFloatArray(%s)", name);
        }
    }
    if (values && ret < count && report) {
        LOGD("readFloatArray(%s, %d): %d", name, count, ret);
        setError("readFloatArray: lose numbers");
    }
    
    return ret;
}

int MgJsonStorage::Impl::readString(const char* name, char* value, int count)
{
    int ret = 0;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsString()) {
            ret = item.GetStringLength();
            if (value) {
                ret = ret < count ? ret : count;
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
                strncpy_s(value, count + 1, item.GetString(), ret);
#else
                strncpy(value, item.GetString(), ret);
#endif
            }
        }
        else {
            LOGD("Invalid value for readString(%s)", name);
        }
    }
    if (value) {
        value[ret] = 0;
    }
    
    return ret;
}

void MgJsonStorage::Impl::writeInt(const char* name, int value)
{
    if (_numAsStr) {
        char buf[20];
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
        sprintf_s(buf, sizeof(buf), "%d", value);
#else
        snprintf(buf, sizeof(buf), "%d", value);
#endif
        Value* v = new Value(buf, (unsigned)strlen(buf), _doc.GetAllocator());
        _created.push_back(v);
        
        if (hasNum(name)) {
            Value namenode(name, _doc.GetAllocator());
            _stack.back()->AddMember(namenode, *v, _doc.GetAllocator());
        } else {
            _stack.back()->AddMember(name, *v, _doc.GetAllocator());
        }
    } else {
        if (hasNum(name)) {
            Value namenode(name, _doc.GetAllocator());
            Value* v = new Value(value);
            _created.push_back(v);
            _stack.back()->AddMember(namenode, *v, _doc.GetAllocator());
        } else {
            _stack.back()->AddMember(name, value, _doc.GetAllocator());
        }
    }
}

void MgJsonStorage::Impl::writeUInt(const char* name, int value)
{
    if (value >= 0 && value <= 0xFF && !_numAsStr) {
        _stack.back()->AddMember(name, (unsigned)value, _doc.GetAllocator());
    } else {
        char buf[20];
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
        sprintf_s(buf, sizeof(buf), "0x%x", value);
#else
        snprintf(buf, sizeof(buf), "0x%x", value);
#endif
        Value* v = new Value(buf, (unsigned)strlen(buf), _doc.GetAllocator());
        _created.push_back(v);
        
        if (hasNum(name)) {
            Value namenode(name, _doc.GetAllocator());
            _stack.back()->AddMember(namenode, *v, _doc.GetAllocator());
        } else {
            _stack.back()->AddMember(name, *v, _doc.GetAllocator());
        }
    }
}

void MgJsonStorage::Impl::writeBool(const char* name, bool value)
{
    _stack.back()->AddMember(name, value, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeFloat(const char* name, float value)
{
    if (hasNum(name)) {
        Value namenode(name, _doc.GetAllocator());
        Value* v = new Value((double)value);
        _created.push_back(v);
        _stack.back()->AddMember(namenode, *v, _doc.GetAllocator());
    } else {
        _stack.back()->AddMember(name, (double)value, _doc.GetAllocator());
    }
}

void MgJsonStorage::Impl::writeDouble(const char* name, double value)
{
    if (hasNum(name)) {
        Value namenode(name, _doc.GetAllocator());
        Value* v = new Value((double)value);
        _created.push_back(v);
        _stack.back()->AddMember(namenode, *v, _doc.GetAllocator());
    } else {
        _stack.back()->AddMember(name, (double)value, _doc.GetAllocator());
    }
}

void MgJsonStorage::Impl::writeFloatArray(const char* name, const float* values, int count)
{
    Value node(kArrayType);
    
    for (int i = 0; i < count; i++) {
        node.PushBack((double)values[i], _doc.GetAllocator());
    }
    _stack.back()->AddMember(name, node, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeDoubleArray(const char* name, const double* values, int count)
{
    Value node(kArrayType);
    
    for (int i = 0; i < count; i++) {
        node.PushBack(values[i], _doc.GetAllocator());
    }
    _stack.back()->AddMember(name, node, _doc.GetAllocator());
}

void MgJsonStorage::Impl::writeString(const char* name, const char* value)
{
    if (value) {
        Value* v = new Value(value, (unsigned)strlen(value), _doc.GetAllocator());
        _created.push_back(v);
        _stack.back()->AddMember(name, *v, _doc.GetAllocator());
    } else {
        _stack.back()->AddMember(name, "", _doc.GetAllocator());
    }
}

int MgJsonStorage::Impl::readIntArray(const char* name, int* values, int count, bool report)
{
    int ret = 0;
    Value *node = _stack.empty() ? (Value *)0 : _stack.back();
    
    report = report && count > 0 && values;
    if (node && node->HasMember(name)) {
        const Value &item = node->GetMember(name);
        
        if (item.IsArray()) {
            ret = item.Size();
            if (values) {
                int n = ret < count ? ret : count;
                ret = 0;
                for (int i = 0; i < n; i++) {
                    const Value &v = item[i];
                    
                    if (v.IsInt()) {
                        values[ret++] = v.GetInt();
                    }
                    else if (v.IsString() && parseInt(v.GetString(), values[ret])) {
                        ret++;
                    }
                    else if (report) {
                        LOGD("Invalid value for readIntArray(%s)", name);
                    }
                }
            }
        }
        else if (report) {
            LOGD("Invalid value for readIntArray(%s)", name);
        }
    }
    if (values && ret < count && report) {
        LOGD("readIntArray(%s, %d): %d", name, count, ret);
        setError("readIntArray: lose numbers");
    }
    
    return ret;
}

void MgJsonStorage::Impl::writeIntArray(const char* name, const int* values, int count)
{
    Value node(kArrayType);
    
    for (int i = 0; i < count; i++) {
        node.PushBack(values[i], _doc.GetAllocator());
    }
    _stack.back()->AddMember(name, node, _doc.GetAllocator());
}

using namespace std;
using namespace utf8;
using namespace utf8::unchecked;

template <typename chartype>
static size_t toutf8_(FILE* fpi, FILE* fpo)
{
    size_t ret = 0;
    vector<chartype> buf(1024);
    
    fwrite(bom, 1, sizeof(bom), fpo);
    for (;;) {
        size_t n = fread(&buf.front(), sizeof(chartype), buf.size(), fpi);
        if (n == 0 || n > buf.size())
            break;
        vector<uint8_t> utf8result;
        unsigned tpsize = sizeof(chartype);
        if (tpsize == 2)
            utf16to8(buf.begin(), buf.begin() + n, back_inserter(utf8result));
        else
            utf32to8(buf.begin(), buf.begin() + n, back_inserter(utf8result));
        ret += fwrite(&utf8result.front(), 1, utf8result.size(), fpo);
    }
    
    return ret;
}

bool MgJsonStorage::toUTF8(const char* infile, const char* outfile)
{
    uint8_t head[4] = { 1, 1, 1, 1 };
    FILE* fp = mgopenfile(infile, "rt");
    size_t ret = 0;
    
    if (fp) {
        fread(head, 1, sizeof(head), fp);
        
        if (starts_with_bom(head, head + sizeof(head), bom32be, sizeof(bom32be))
            || starts_with_bom(head, head + sizeof(head), bom32le, sizeof(bom32le))) {
            FILE* fpo = mgopenfile(outfile, "wt");
            if (fpo) {
                fseek(fp, sizeof(bom32be), SEEK_SET);
                ret = toutf8_<uint32_t>(fp, fpo);
                fclose(fpo);
            } else {
                LOGE("Fail to create file: %s", outfile);
            }
        }
        else if (starts_with_bom(head, head + sizeof(head), bom16be, sizeof(bom16be))
                 || starts_with_bom(head, head + sizeof(head), bom16le, sizeof(bom16le))) {
            FILE* fpo = mgopenfile(outfile, "wt");
            if (fpo) {
                fseek(fp, sizeof(bom16be), SEEK_SET);
                ret = toutf8_<uint16_t>(fp, fpo);
                fclose(fpo);
            } else {
                LOGE("Fail to create file: %s", outfile);
            }
        }
        
        fclose(fp);
    }
    
    return ret > 0;
}

bool MgJsonStorage::toUTF16(const char* infile, const char* outfile)
{
    uint8_t head[4] = { 1, 1, 1, 1 };
    FILE* fp = mgopenfile(infile, "rt");
    size_t ret = 0;
    
    if (fp) {
        fread(head, 1, sizeof(head), fp);
        
        bool isutf8 = starts_with_bom(head, head + sizeof(head));
        
        if (!isutf8 && !starts_with_bom(head, head + sizeof(head), bom16be, sizeof(bom16be))
            && !starts_with_bom(head, head + sizeof(head), bom16le, sizeof(bom16le))) {
            isutf8 = true;
            fseek(fp, 0, SEEK_SET);
        }
        if (isutf8) {
            FILE* fpo = mgopenfile(outfile, "wt");
            if (fpo) {
                vector<uint8_t> buf(1024);
                
                fwrite(bom16le, 1, sizeof(bom16le), fpo);
                for (;;) {
                    size_t n = fread(&buf.front(), 1, buf.size(), fp);
                    if (n == 0 || n > buf.size())
                        break;
                    vector<uint16_t> utf16result;
                    utf8to16(buf.begin(), buf.begin() + n, back_inserter(utf16result));
                    ret += fwrite(&utf16result.front(), 2, utf16result.size(), fpo);
                }
                fclose(fpo);
            } else {
                LOGE("Fail to create file: %s", outfile);
            }
        }
        
        fclose(fp);
    }
    
    return ret > 0;
}
