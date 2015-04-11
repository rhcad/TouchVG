//! \file gicorerecord.cpp
//! \brief 实现内核视图类 GiCoreView 的录制功能
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "gicoreview.h"
#include "gicoreviewimpl.h"
#include <algorithm>

static const bool VG_PRETTY = false;

long GiCoreView::getRecordTick(bool forUndo, long curTick)
{
    MgRecordShapes* recorder = impl->recorder(forUndo);
    if (!recorder)
        return 0;
    long tick = recorder->getCurrentTick(curTick);
    long pauseTick = curTick - impl->startPauseTick;
    return isPaused() && tick > pauseTick ? tick - pauseTick : tick;
}

bool GiCoreView::startRecord(const char* path, long doc, bool forUndo,
                             long curTick, MgStringCallback* c)
{
    MgRecordShapes* p = new MgRecordShapes(path, MgShapeDoc::fromHandle(doc), forUndo, curTick);
    impl->setRecorder(forUndo, p);
    
    if (isPlaying() || forUndo) {
        return true;
    }
    
    if (!saveToFile(doc, p->getFileName(false, 0).c_str(), VG_PRETTY)) {
        return false;
    }
    
    if (c) {
        c->onGetString(p->getFileName(false, 0).c_str());
    }
    return true;
}

void GiCoreView::stopRecord(bool forUndo)
{
    impl->setRecorder(forUndo, NULL);
    if (!forUndo && impl->play.playing) {
        impl->play.playing->clear();
    }
}

bool GiCoreView::recordShapes(bool forUndo, long tick, long changeCount, long doc, long shapes)
{
    return recordShapes(forUndo, tick, changeCount, doc, shapes, NULL, NULL);
}

bool GiCoreView::recordShapes(bool forUndo, long tick, long changeCount, long doc,
                              long shapes, const mgvector<long>* exts, MgStringCallback* c)
{
    MgRecordShapes* recorder = impl->recorder(forUndo);
    int ret = 0;
    std::vector<MgShapes*> arr;
    int i;
    
    for (i = 0; i < (exts ? exts->count() : 0); i++) {
        MgShapes* p = MgShapes::fromHandle(exts->get(i));
        if (p) {
            arr.push_back(p);
        }
    }
    
    if (recorder && !recorder->isLoading() && !recorder->isPlaying()) {
        ret = recorder->recordStep(tick, changeCount, impl->changeCount,
                                   MgShapeDoc::fromHandle(doc),
                                   MgShapes::fromHandle(shapes), arr) ? 2 : 1;
        if (ret > 1 && c) {
            c->onGetString(recorder->getFileName(false, recorder->getFileCount() - 1).c_str());
        }
    } else {
        GiPlaying::releaseDoc(doc);
        GiPlaying::releaseShapes(shapes);
    }
    for (i = 0; i < (int)arr.size(); i++) {
        MgObject::release_pointer(arr[i]);
    }
    
    return ret > 0;
}

bool GiCoreView::restoreRecord(int type, const char* path, long doc, long changeCount,
                               int index, int count, int tick, long curTick)
{
    MgRecordShapes* recorder = impl->recorder(type == 0);
    if (recorder || !path)
        return false;
    
    recorder = new MgRecordShapes(path, MgShapeDoc::fromHandle(doc), type == 0, curTick);
    recorder->restore(index, count, tick, curTick);
    impl->setRecorder(type == 0, recorder);
    
    if (type == 0 && changeCount != 0) {
        if (!giAtomicCompareAndSwap(&impl->changeCount, changeCount, impl->changeCount)) {
            LOGE("Fail to set changeCount via giAtomicCompareAndSwap");
        }
    }
    
    return true;
}

bool GiCoreView::isUndoLoading() const
{
    return impl->recorder(true) && impl->recorder(true)->isLoading();
}

bool GiCoreView::canUndo() const
{
    return impl->recorder(true) && impl->recorder(true)->canUndo();
}

bool GiCoreView::canRedo() const
{
    return impl->recorder(true) && impl->recorder(true)->canRedo();
}

int GiCoreView::getRedoIndex() const
{
    return impl->recorder(true) ? impl->recorder(true)->getFileCount() : 0;
}

int GiCoreView::getRedoCount() const
{
    return impl->recorder(true) ? impl->recorder(true)->getMaxFileCount() : 0;
}

bool GiCoreView::undo(GiView* view)
{
    MgRecordShapes* recorder = impl->recorder(true);
    bool ret = false;
    long changeCount = impl->changeCount;
    
    if (recorder) {
        recorder->setLoading(true);
        ret = recorder->undo(impl->getShapeFactory(), impl->doc(), &changeCount);
        if (ret) {
            submitBackDoc(view, true);
            submitDynamicShapes(view);
            if (!giAtomicCompareAndSwap(&impl->changeCount, changeCount, impl->changeCount)) {
                LOGE("Fail to set changeCount via giAtomicCompareAndSwap");
            }
            recorder->resetDoc(MgShapeDoc::fromHandle(acquireFrontDoc()));
            impl->regenAll(true);
            impl->hideContextActions();
        }
        recorder->setLoading(false);
    }
    if (ret && impl->cmds()) {
        impl->getCmdSubject()->onDocLoaded(impl->motion(), true);
    }
    
    return ret;
}

bool GiCoreView::redo(GiView* view)
{
    MgRecordShapes* recorder = impl->recorder(true);
    bool ret = false;
    long changeCount = impl->changeCount;
    
    if (recorder) {
        recorder->setLoading(true);
        ret = recorder->redo(impl->getShapeFactory(), impl->doc(), &changeCount);
        if (ret) {
            submitBackDoc(view, true);
            submitDynamicShapes(view);
            if (!giAtomicCompareAndSwap(&impl->changeCount, changeCount, impl->changeCount)) {
                LOGE("Fail to set changeCount via giAtomicCompareAndSwap");
            }
            recorder->resetDoc(MgShapeDoc::fromHandle(acquireFrontDoc()));
            impl->regenAll(true);
            impl->hideContextActions();
        }
        recorder->setLoading(false);
    }
    if (ret && impl->cmds()) {
        impl->getCmdSubject()->onDocLoaded(impl->motion(), true);
    }
    
    return ret;
}

bool GiCoreView::isUndoRecording() const
{
    return !!impl->recorder(true);
}

bool GiCoreView::isRecording() const
{
    return impl->recorder(false) && !impl->recorder(false)->isPlaying();
}

bool GiCoreView::isPlaying() const
{
    return impl->recorder(false) && impl->recorder(false)->isPlaying();
}

int GiCoreView::getFrameIndex() const
{
    return impl->recorder(false) ? impl->recorder(false)->getFileCount() : -1;
}

long GiCoreView::getFrameTick()
{
    return impl->recorder(false) ? (long)impl->recorder(false)->getFileTick() : 0;
}

int GiCoreView::getFrameFlags()
{
    return impl->recorder(false) ? impl->recorder(false)->getFileFlags() : 0;
}

bool GiCoreView::isPaused() const
{
    return impl->startPauseTick != 0;
}

bool GiCoreView::onPause(long curTick)
{
    bool ret = giAtomicCompareAndSwap(&impl->startPauseTick, curTick, 0);
    if (!ret) {
        LOGE("Fail to set startPauseTick via giAtomicCompareAndSwap");
    }
    return ret;
}

bool GiCoreView::onResume(long curTick)
{
    long startPauseTick = impl->startPauseTick;
    
    if (startPauseTick != 0
        && giAtomicCompareAndSwap(&impl->startPauseTick, 0, startPauseTick)) {
        long ticks = curTick - startPauseTick;
        if (impl->recorder(true) && !impl->recorder(true)->onResume(ticks)) {
            LOGE("recorder(true)->onResume(%ld) fail", ticks);
            return false;
        }
        if (impl->recorder(false) && !impl->recorder(false)->onResume(ticks)) {
            LOGE("recorder(false)->onResume(%ld) fail", ticks);
            return false;
        }
        return true;
    } else if (startPauseTick) {
        LOGE("Fail to set startPauseTick via giAtomicCompareAndSwap");
    }
    
    return false;
}

// GiPlaying
//

struct GiPlaying::Impl {
    MgShapeDoc* frontDoc_;
    MgShapeDoc* backDoc;
    MgShapes*   front_;
    MgShapes*   back;
    int         tag;
    bool        doubleSided;
    volatile long stopping;
    
    Impl(int tag, bool doubleSided) : frontDoc_(NULL), backDoc(NULL)
        , front_(NULL), back(NULL), tag(tag), doubleSided(doubleSided), stopping(0) {}
    
    MgShapeDoc*& frontDoc() { return doubleSided ? frontDoc_ : backDoc; }
    MgShapes*& frontShapes() { return doubleSided ? front_ : back; }
};

GiPlaying* GiPlaying::create(MgCoreView* v, int tag, bool doubleSided)
{
    GiPlaying* p = new GiPlaying(tag, doubleSided);
    if (v && tag >= 0) {
        GiCoreViewData::fromHandle(v->viewDataHandle())->addPlaying(p);
    }
    return p;
}

void GiPlaying::release(MgCoreView* v)
{
    if (v) {
        GiCoreViewData::fromHandle(v->viewDataHandle())->removePlaying(this);
    }
    delete this;
}

GiPlaying::GiPlaying(int tag, bool doubleSided) : impl(new Impl(tag, doubleSided))
{
}

GiPlaying::~GiPlaying()
{
    clear();
    delete impl;
}

void GiPlaying::clear()
{
    MgObject::release_pointer(impl->frontDoc_);
    MgObject::release_pointer(impl->backDoc);
    MgObject::release_pointer(impl->front_);
    MgObject::release_pointer(impl->back);
}

int GiPlaying::getTag() const
{
    return impl->tag;
}

void GiPlaying::stop()
{
    giAtomicIncrement(&impl->stopping);
}

bool GiPlaying::isStopping() const
{
    return impl->stopping > 0;
}

long GiPlaying::acquireFrontDoc()
{
    if (!impl->frontDoc())
        return 0;
    impl->frontDoc()->addRef();
    return impl->frontDoc()->toHandle();
}

void GiPlaying::releaseDoc(long doc)
{
    MgShapeDoc* p = MgShapeDoc::fromHandle(doc);
    MgObject::release_pointer(p);
}

MgShapeDoc* GiPlaying::getBackDoc()
{
    if (!impl->backDoc) {
        impl->backDoc = MgShapeDoc::createDoc();
    }
    return impl->backDoc;
}

void GiPlaying::submitBackDoc()
{
    if (impl->doubleSided) {
        MgObject::release_pointer(impl->frontDoc_);
        if (impl->backDoc) {
            impl->frontDoc_ = impl->backDoc->shallowCopy();
        }
    }
}

long GiPlaying::acquireFrontShapes()
{
    if (!impl->frontShapes())
        return 0;
    impl->frontShapes()->addRef();
    return impl->frontShapes()->toHandle();
}

void GiPlaying::releaseShapes(long shapes)
{
    MgShapes* p = MgShapes::fromHandle(shapes);
    MgObject::release_pointer(p);
}

long GiPlaying::getBackShapesHandle(bool needClear)
{
    return getBackShapes(needClear)->toHandle();
}

MgShapes* GiPlaying::getBackShapes(bool needClear)
{
    if (needClear || !impl->back) {
        MgObject::release_pointer(impl->back);
        impl->back = MgShapes::create();
    } else {
        MgShapes* old = impl->back;
        impl->back = old->shallowCopy();
        MgObject::release_pointer(old);
    }
    return impl->back;
}

void GiPlaying::submitBackShapes()
{
    if (impl->doubleSided) {
        MgObject::release_pointer(impl->front_);
        impl->front_ = impl->back;
        impl->front_->addRef();
    }
}
