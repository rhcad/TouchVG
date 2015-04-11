// recordshapes.cpp
// Copyright (c) 2013-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "recordshapes.h"
#include "mgshapedoc.h"
#include "mglayer.h"
#include "mglines.h"
#include "mgjsonstorage.h"
#include "mgstorage.h"
#include "mgvector.h"
#include "mglog.h"
#include <sstream>
#include <map>

static const bool VG_PRETTY = false;

struct MgRecordShapes::Impl
{
    std::string     path;
    int             type;
    std::map<int, long> id2ver;
    std::vector<int>    lastids;
    volatile int    fileCount;
    volatile int    maxCount;
    volatile long   loading;
    MgShapeDoc      *lastDoc;
    MgShape         *lastShape;
    volatile long   startTick;
    int             tick, lastTick;
    int             flags[2];
    int             shapeCount;
    MgJsonStorage   *js[3];
    MgStorage       *s[3];
    
    Impl(long curTick) : fileCount(0), maxCount(0), loading(0), lastDoc(NULL)
        , lastShape(NULL), startTick(curTick), tick(0), lastTick(0)
    {
        memset(flags, 0, sizeof(flags));
        memset(js, 0, sizeof(js));
        memset(s, 0, sizeof(s));
    }
    ~Impl() {
        MgObject::release_pointer(lastDoc);
        MgObject::release_pointer(lastShape);
    }
    
    void beginJsonFile();
    bool saveJsonFile();
    std::string getFileName(bool back, int index = -1) const;
    void resetVersion(const MgShapes* shapes);
    void startRecord();
    void stopRecordIndex();
    bool saveIndexFile(bool ended);
    void recordShapes(const MgShapes* shapes);
    bool forUndo() const { return type == 0; }
    bool incrementRecord(MgShapes* dynShapes);
};

MgRecordShapes::MgRecordShapes(const char* path, MgShapeDoc* doc, bool forUndo, long curTick)
{
    _im = new Impl(curTick);
    _im->path = path;
    if (*_im->path.rbegin() != '/' && *_im->path.rbegin() != '\\') {
        _im->path += '/';
    }
    _im->type = forUndo ? 0 : doc ? 1 : 2;
    _im->lastDoc = doc;
    if (doc) {
        _im->resetVersion(doc->getCurrentLayer());
        _im->startRecord();
    }
}

MgRecordShapes::~MgRecordShapes()
{
    _im->stopRecordIndex();
    delete _im;
}

void MgRecordShapes::stopRecordIndex()
{
    _im->stopRecordIndex();
}

bool MgRecordShapes::isPlaying() const
{
    return _im->type > 1;
}

int MgRecordShapes::getFileTick() const
{
    return _im->tick;
}

int MgRecordShapes::getFileFlags() const
{
    return _im->flags[0];
}

int MgRecordShapes::getFileCount() const
{
    return _im->fileCount;
}

int MgRecordShapes::getMaxFileCount() const
{
    return _im->maxCount;
}

long MgRecordShapes::getCurrentTick(long curTick) const
{
    return curTick - _im->startTick;
}

bool MgRecordShapes::onResume(long ticks)
{
    bool ret = giAtomicCompareAndSwap(&_im->startTick, _im->startTick + ticks, _im->startTick);
    if (!ret) {
        LOGE("Fail to set startTick via giAtomicCompareAndSwap");
    }
    return ret;
}

bool MgRecordShapes::recordStep(long tick, long changeCountOld, long changeCountNew, MgShapeDoc* doc,
                                MgShapes* dynShapes, const std::vector<MgShapes*>& extShapes)
{
    _im->beginJsonFile();
    _im->tick = (int)tick;
    
    bool needDyn = _im->lastDoc && !_im->forUndo();
    if (doc) {
        if (_im->lastDoc) {     // undo() set lastDoc as null
            _im->recordShapes(doc->getCurrentLayer());
            MgObject::release_pointer(_im->lastDoc);
            if (_im->flags[0])
                MgObject::release_pointer(_im->lastShape);
        }
        _im->lastDoc = doc;
    }
    
    if (!extShapes.empty()) {
        MgShapes* newsp = MgShapes::create();
        newsp->copyShapes(dynShapes, false, false);
        for (size_t i = 0; i < extShapes.size(); i++) {
            newsp->copyShapes(extShapes[i], false, false);
        }
        MgObject::release_pointer(dynShapes);
        dynShapes = newsp;
    }
    if (needDyn && dynShapes && dynShapes->getShapeCount() > 0) {
        if (!_im->incrementRecord(dynShapes)) {
            _im->flags[0] |= DYN;
            _im->s[0]->writeNode("dynamic", -1, false);
            dynShapes->save(_im->s[0]);
            _im->s[0]->writeNode("dynamic", -1, true);
        }
    }
    MgObject::release_pointer(dynShapes);
    
    _im->s[0]->writeInt("flags", _im->flags[0]);
    if (_im->flags[0] != DYN) {
        _im->s[0]->writeInt("changeCount", (int)changeCountNew);
        _im->s[1]->writeInt("changeCount", (int)changeCountOld);
    }
    
    bool ret = _im->saveJsonFile();
    
    if (ret && _im->s[2]) {
        _im->s[2]->writeNode("r", _im->fileCount - 2, false);
        _im->s[2]->writeInt("tick", _im->tick);
        _im->s[2]->writeInt("flags", _im->flags[0]);
        _im->s[2]->writeNode("r", _im->fileCount - 2, true);
        
        if (_im->fileCount % 10 == 0 || _im->flags[0] != MgRecordShapes::DYN) {
            _im->saveIndexFile(false);
        }
    }
    
    return ret;
}

bool MgRecordShapes::Impl::incrementRecord(MgShapes* dynShapes)
{
    bool ret = false;
    
    if (flags[0] == 0 && lastShape
        && dynShapes->getShapeCount() == 1
        && dynShapes->getLastShape()->shapec()->isKindOf(MgBaseLines::Type())
        && dynShapes->getLastShape()->getType() == lastShape->getType())
    {
        if (lastShape->equals(*dynShapes->getLastShape()))
            return true;
            
        const MgBaseLines* oldlines = (const MgBaseLines*)lastShape->shapec();
        const MgBaseLines* lines = (const MgBaseLines*)dynShapes->getLastShape()->shapec();
        
        if (lines->isIncrementFrom(*oldlines)) {
            const Point2d* pts = lines->getPoints() + oldlines->getPointCount();
            s[0]->writeFloatArray("dyninc", (const float*)pts,
                                  (lines->getPointCount() - oldlines->getPointCount()) * 2);
            flags[0] |= DYN;
            ret = true;
        }
    }
    
    MgObject::release_pointer(lastShape);
    lastShape = const_cast<MgShape*>(dynShapes->getLastShape());
    lastShape->addRef();
    
    return ret;
}

void MgRecordShapes::resetDoc(MgShapeDoc* doc)
{
    if (doc) {
        MgObject::release_pointer(_im->lastDoc);
        _im->lastDoc = doc;
    }
}

void MgRecordShapes::restore(int index, int count, int tick, long curTick)
{
    std::vector<int> arr;
    
    if (_im->s[2] && loadFrameIndex(_im->path, arr)) {
        for (unsigned i = 0; i + 2 < arr.size(); i += 3) {
            _im->s[2]->writeNode("r", i / 3, false);
            _im->s[2]->writeInt("tick", arr[i + 1]);
            _im->s[2]->writeInt("flags", arr[i + 2]);
            _im->s[2]->writeNode("r", i / 3, true);
        }
    }
    _im->fileCount = index;
    _im->maxCount = count ? count : index;
    _im->startTick = curTick - tick;
    LOGD("restore fileCount=%d, maxCount=%d, startTick=%d, frames=%d",
         _im->fileCount, _im->maxCount, tick, (int)arr.size() / 3);
}

bool MgRecordShapes::loadFrameIndex(std::string path, std::vector<int>& arr)
{
    if (*path.rbegin() != '/' && *path.rbegin() != '\\')
        path += '/';
    path += "records.json";
    
    FILE *fp = mgopenfile(path.c_str(), "rt");
    if (!fp) {
        LOGE("Fail to read file: %s", path.c_str());
        return false;
    }
    
    MgJsonStorage js;
    MgStorage* s = js.storageForRead(fp);
    
    fclose(fp);
    s->readNode("records", -1, false);
    
    for (int i = 0; s->readNode("r", i, false); i++) {
        arr.push_back(i + 1);
        arr.push_back(s->readInt("tick", 0));
        arr.push_back(s->readInt("flags", 0));
        s->readNode("r", i, true);
    }
    
    return s->readNode("records", -1, true);
}

std::string MgRecordShapes::getFileName(bool back, int index) const
{
    return _im->getFileName(back, index);
}

std::string MgRecordShapes::getPath() const
{
    return _im->path;
}

bool MgRecordShapes::isLoading() const
{
    return _im->loading > 0;
}

bool MgRecordShapes::canUndo() const
{
    return _im->fileCount > 1 && !_im->loading;
}

bool MgRecordShapes::canRedo() const
{
    return _im->fileCount < _im->maxCount && !_im->loading;
}

void MgRecordShapes::setLoading(bool loading)
{
    if (loading)
        giAtomicIncrement(&_im->loading);
    else
        giAtomicDecrement(&_im->loading);
}

bool MgRecordShapes::undo(MgShapeFactory *factory, MgShapeDoc* doc, long* changeCount)
{
    if (_im->loading > 1 || !_im->lastDoc)
        return false;
    
    giAtomicIncrement(&_im->loading);
    
    std::string fn(_im->getFileName(true, _im->fileCount - 1));
    int ret = applyFile(_im->tick, factory, doc, NULL, fn.c_str(), changeCount);
    
    if (ret) {
        _im->fileCount--;
        _im->resetVersion(doc->getCurrentLayer());
        MgObject::release_pointer(_im->lastDoc);
        LOGD("Undo with %s", fn.c_str());
    }
    giAtomicDecrement(&_im->loading);
    
    return ret != 0;
}

bool MgRecordShapes::redo(MgShapeFactory *factory, MgShapeDoc* doc, long* changeCount)
{
    if (_im->loading > 1)
        return false;
    
    giAtomicIncrement(&_im->loading);
    
    std::string fn(_im->getFileName(false, _im->fileCount));
    int ret = applyFile(_im->tick, factory, doc, NULL, fn.c_str(), changeCount);
    
    if (ret) {
        _im->fileCount++;
        _im->resetVersion(doc->getCurrentLayer());
        MgObject::release_pointer(_im->lastDoc);
        LOGD("Redo with %s", fn.c_str());
    }
    giAtomicDecrement(&_im->loading);
    
    return ret != 0;
}

static void saveIds(const std::vector<int>& ids, MgStorage *s, const char* group)
{
    s->writeNode(group, -1, false);
    for (int i = 0; i < (int)ids.size(); ++i) {
        std::stringstream ss;
        ss << "d" << i;
        s->writeInt(ss.str().c_str(), ids[i]);
    }
    s->writeNode(group, -1, true);
}

void MgRecordShapes::Impl::recordShapes(const MgShapes* shapes)
{
    MgShapeIterator it(shapes);
    std::map<int, long> tmpids(id2ver);
    std::map<int, long>::iterator i;
    int i2 = 0;
    int sid;
    std::vector<int> newids, nowids;
    
    s[0]->writeNode("shapes", shapes->getIndex(), false);
    s[1]->writeNode("shapes", shapes->getIndex(), false);
    
    while (const MgShape* sp = it.getNext()) {
        sid = sp->getID();
        i = id2ver.find(sid);                                   // 查找是否之前已存在
        nowids.push_back(sid);
        
        if (i == id2ver.end()) {                                // 是新增的图形
            newids.push_back(sid);
            id2ver[sid] = sp->shapec()->getChangeCount();       // 增加记录版本
            shapes->saveShape(s[0], sp, shapeCount++);          // 写图形节点
            flags[0] |= flags[0] ? EDIT : ADD;
        } else {
            tmpids.erase(tmpids.find(sid));                     // 标记是已有图形
            if (i->second != sp->shapec()->getChangeCount()) {  // 改变的图形
                i->second = sp->shapec()->getChangeCount();
                id2ver[sid] = sp->shapec()->getChangeCount();   // 更新版本
                shapes->saveShape(s[0], sp, shapeCount++);
                flags[0] |= EDIT;
                i2 += shapes->saveShape(s[1], lastDoc->findShape(sid), i2) ? 1 : 0;
                flags[1] |= EDIT;
            }
        }
    }
    s[0]->writeNode("shapes", shapes->getIndex(), true);
    s[0]->writeInt("count", shapeCount += (int)tmpids.size());
    
    if (!tmpids.empty()) {                                      // 之前存在，现在已删除
        flags[0] |= DEL;
        s[0]->writeNode("delete", -1, false);
        int j = 0;
        for (i = tmpids.begin(); i != tmpids.end(); ++i) {
            sid = i->first;
            id2ver.erase(id2ver.find(sid));
            
            std::stringstream ss;
            ss << "d" << j++;
            s[0]->writeInt(ss.str().c_str(), sid);              // 记下删除的图形的ID
            flags[1] |= ADD;
            i2 += shapes->saveShape(s[1], lastDoc->findShape(sid), i2) ? 1 : 0;
        }
        s[0]->writeNode("delete", -1, true);
    }
    
    s[1]->writeNode("shapes", shapes->getIndex(), true);
    
    if (!newids.empty()) {
        flags[1] |= DEL;
        s[1]->writeNode("delete", -1, false);
        for (unsigned j = 0; j < newids.size(); j++) {
            std::stringstream ss;
            ss << "d" << j;
            s[1]->writeInt(ss.str().c_str(), newids[j]);
        }
        s[1]->writeNode("delete", -1, true);
    }
    if (!flags[0] && nowids.size() == lastids.size() && nowids != lastids) {
        flags[0] |= EDIT;
        flags[1] |= EDIT;
        saveIds(lastids, s[1], "reorder");
        saveIds(nowids, s[0], "reorder");
    }
    lastids = nowids;
    
    s[1]->writeInt("flags", flags[1]);
    s[1]->writeInt("count", i2 + (int)newids.size());
}

void MgRecordShapes::Impl::resetVersion(const MgShapes* shapes)
{
    MgShapeIterator it(shapes);
    
    id2ver.clear();
    lastids.clear();
    while (const MgShape* sp = it.getNext()) {
        id2ver[sp->getID()] = sp->shapec()->getChangeCount();
        lastids.push_back(sp->getID());
    }
}

void MgRecordShapes::Impl::startRecord()
{
    if (!forUndo()) {
        js[2] = new MgJsonStorage();
        s[2] = js[2]->storageForWrite();
        s[2]->writeNode("records", -1, false);
    }
    fileCount = 1;
    maxCount = 1;
}

void MgRecordShapes::Impl::beginJsonFile()
{
    if (maxCount == 0) {
        maxCount = fileCount = 1;
    }
    flags[0] = 0;
    flags[1] = 0;
    shapeCount = 0;
    
    for (int i = 0; i < 2; i++) {
        js[i] = new MgJsonStorage();
        s[i] = js[i]->storageForWrite();
        s[i]->writeNode("record", -1, false);
        s[i]->writeInt("tick", tick);
    }
}

std::string MgRecordShapes::Impl::getFileName(bool back, int index) const
{
    std::stringstream ss;
    if (index < 0)
        index = fileCount;
    ss << path << index << (index > 0 ? (back ? ".vgu" : ".vgr") : ".vg");
    return ss.str();
}

bool MgRecordShapes::Impl::saveJsonFile()
{
    bool ret = false;
    std::string filename;
    
    if (flags[0] == DYN && tick - lastTick < 20) {
        //LOGD("Ignore record at the same time %d", tick);
        flags[0] = flags[1] = 0;
    }
    
    for (int i = 0; i < 2; i++) {
        if (lastDoc && flags[i] && flags[i] != DYN) {
            s[i]->writeFloatArray("transform", &lastDoc->modelTransform().m11, 6);
            s[i]->writeFloatArray("pageExtent", &lastDoc->getPageRectW().xmin, 4);
            s[i]->writeFloat("viewScale", lastDoc->getViewScale());
        }
        if (flags[i] != 0) {
            filename = getFileName(i > 0);
            FILE *fp = mgopenfile(filename.c_str(), "wt");
            
            if (!fp) {
                LOGE("Fail to save file: %s", filename.c_str());
            } else {
                ret = s[i]->writeNode("record", -1, true) && js[i]->save(fp, VG_PRETTY);
                fclose(fp);
                if (!ret) {
                    LOGE("Fail to record shapes: %s", filename.c_str());
                }
            }
        }
        delete js[i];
        js[i] = NULL;
        s[i] = NULL;
    }
    if (ret) {
        /*if (flags[0] && !forUndo()) {
            struct stat stat1;
            filename = getFileName(false);
            stat(filename.c_str(), &stat1);
            
            LOGD("Record %03d: tick=%d, flags=%d, count=%d, filesize=%ld",
                 fileCount, tick, flags[0], shapeCount, (long)stat1.st_size);
        }*/
        maxCount = ++fileCount;
        lastTick = tick;
    }
    
    return ret;
}

bool MgRecordShapes::Impl::saveIndexFile(bool ended)
{
    std::string filename(path + "records.json");
    FILE *fp = mgopenfile(filename.c_str(), "wt");
    bool ret = false;
    
    if (!fp) {
        LOGE("Fail to save file: %s", filename.c_str());
    } else {
        if (ended) {
            s[2]->writeNode("records", -1, true);
        }
        ret = js[2]->save(fp, VG_PRETTY);
        if (!ret) {
            LOGE("Fail to save records: %s", filename.c_str());
        }
        fclose(fp);
    }
    
    return ret;
}

void MgRecordShapes::Impl::stopRecordIndex()
{
    if (js[2]) {
        if (fileCount > 1 && saveIndexFile(true)) {
            LOGD("Save records.json in %s", path.c_str());
        }
        delete js[2];
        js[2] = NULL;
        s[2] = NULL;
    }
    MgObject::release_pointer(lastShape);
}

int MgRecordShapes::applyFile(int& tick, MgShapeFactory *f,
                              MgShapeDoc* doc, MgShapes* dyns, const char* fn,
                              long* changeCount, MgShape* lastShape)
{
    FILE *fp = mgopenfile(fn, "rt");
    if (!fp) {
        //LOGE("Fail to read file: %s", fn);
        return 0;
    }
    
    MgJsonStorage js;
    MgStorage* s = js.storageForRead(fp);
    int ret = 0;
    
    fclose(fp);
    if (s->readNode("record", -1, false)) {
        if (doc) {
            if (s->readFloatArray("transform", &doc->modelTransform().m11, 6, false) == 6) {
                Box2d rect(doc->getPageRectW());
                s->readFloatArray("pageExtent", &rect.xmin, 4);
                float viewScale = s->readFloat("viewScale", doc->getViewScale());
                doc->setPageRectW(rect, viewScale);
            }
            
            MgShapes* stds = doc->getCurrentLayer();
            int flags = s->readInt("flags", 0);
            
            if ((flags & (ADD|EDIT)) && stds->load(f, s, true) > 0) {
                ret |= (flags == ADD) ? SHAPE_APPEND : DOC_CHANGED;
            }
            
            if (s->readNode("delete", -1, false)) {
                for (int i = 0; ; i++) {
                    std::stringstream ss;
                    ss << "d" << i;
                    int sid = s->readInt(ss.str().c_str(), 0);
                    if (sid == 0)
                        break;
                    if (stds->removeShape(sid)) {
                        ret |= DOC_CHANGED;
                        //LOGD("removeShape id=%d", sid);
                    }
                }
                s->readNode("delete", -1, true);
            }
            if (s->readNode("reorder", -1, false)) {
                std::vector<int> ids;
                
                for (int i = 0; ; i++) {
                    std::stringstream ss;
                    ss << "d" << i;
                    int sid = s->readInt(ss.str().c_str(), 0);
                    if (sid == 0)
                        break;
                    ids.push_back(sid);
                }
                s->readNode("reorder", -1, true);
                
                if (!ids.empty() && stds->reorderShapes((int)ids.size(), &ids.front())) {
                    ret |= DOC_CHANGED;
                }
            }
        }
        if (dyns && s->readNode("dynamic", -1, false)) {
            if (dyns->load(f, s) >= 0)
                ret |= DYN_CHANGED;
            s->readNode("dynamic", -1, true);
        } else if (dyns && lastShape
                   && lastShape->shapec()->isKindOf(MgBaseLines::Type())) {
            int n = s->readFloatArray("dyninc", NULL, 0);
            mgvector<float> buf(n);
            
            if (n > 0 && s->readFloatArray("dyninc", buf.address(), n) == n) {
                MgShape* sp = lastShape->cloneShape();
                MgBaseLines* lines = (MgBaseLines*)sp->shape();
                
                for (int i = 0; i + 1 < n; i += 2) {
                    lines->addPoint(Point2d(buf.get(i), buf.get(i + 1)));
                }
                dyns->addShapeDirect(sp, true);
                ret |= DYN_CHANGED;
            }
        }
        if (ret) {
            tick = s->readInt("tick", tick);
        }
        if (ret && changeCount) {
            *changeCount = s->readInt("changeCount", (int)*changeCount);
        }
        
        s->readNode("record", -1, true);
    }
    
    return ret;
}

bool MgRecordShapes::applyFirstFile(MgShapeFactory *factory, MgShapeDoc* doc)
{
    std::string filename(_im->getFileName(false, 0));
    return applyFirstFile(factory, doc, filename.c_str());
}

bool MgRecordShapes::applyFirstFile(MgShapeFactory *factory, MgShapeDoc* doc, const char* filename)
{
    FILE *fp = mgopenfile(filename, "rt");
    if (!fp) {
        LOGE("Fail to read file: %s", filename);
        return 0;
    }
    
    MgJsonStorage js;
    MgStorage* s = js.storageForRead(fp);
    
    fclose(fp);
    _im->fileCount = 1;
    MgObject::release_pointer(_im->lastShape);
    
    return doc->load(factory, s, false);
}

int MgRecordShapes::applyRedoFile(MgShapeFactory *f, MgShapeDoc* doc, MgShapes* dyns, int index)
{
    if (index <= 0)
        index = _im->fileCount;
    
    std::string filename(_im->getFileName(false, index));
    int ret = applyFile(_im->tick, f, doc, dyns, filename.c_str(), NULL, _im->lastShape);
    
    if (ret) {
        _im->fileCount = index + 1;
        MgObject::release_pointer(_im->lastShape);
        if (dyns) {
            _im->lastShape = const_cast<MgShape*>(dyns->getLastShape());
            if (_im->lastShape)
                _im->lastShape->addRef();
        }
    }
    return ret;
}

int MgRecordShapes::applyUndoFile(MgShapeFactory *f, MgShapeDoc* doc,
                                  MgShapes* dyns, int index, long curTick)
{
    if (index <= 0)
        index = _im->fileCount;
    if (index <= 0)
        return 0;
    
    if (index == 1) {
        _im->fileCount = 0;
        _im->startTick = curTick;
        return DYN_CHANGED;
    }
    
    std::string filename(_im->getFileName(true, index - 1));
    int ret = applyFile(_im->tick, f, doc, NULL, filename.c_str());
    
    filename = _im->getFileName(false, index - 1);
    ret |= applyFile(_im->tick, f, NULL, dyns, filename.c_str()) | DYN_CHANGED;
    
    if (ret) {
        _im->fileCount = index - 1;
        MgObject::release_pointer(_im->lastShape);
        if (dyns) {
            _im->lastShape = const_cast<MgShape*>(dyns->getLastShape());
            if (_im->lastShape)
                _im->lastShape->addRef();
        }
    }
    return ret;
}
