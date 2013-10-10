//! \file mgshapedoc.cpp
//! \brief 实现图形文档 MgShapeDoc
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshapedoc.h"
#include <mgstorage.h>
#include <vector>
#include "mglayer.h"

struct MgShapeDoc::Impl {
    std::vector<MgLayer*> layers;
    MgShapes*   curShapes;
    GiContext   context;
    Matrix2d    xf;
    Box2d       rectW;
    float       viewScale;
    long        changeCount;
    volatile long   refcount;

    MgLockRW    lock;
    MgLockRW    dynlock;
    typedef std::pair<ShapesLocked, void*> Observer;
    std::vector<Observer>  observers;
};

MgShapeDoc::MgShapeDoc()
{
    im = new Impl();
    im->layers.push_back(MgLayer::create(this, 0));
    im->curShapes = im->layers[0];
    im->viewScale = 0;
    im->changeCount = 0;
    im->refcount = 1;
}

MgShapeDoc::~MgShapeDoc()
{
    for (unsigned i = 0; i < im->layers.size(); i++) {
        im->layers[i]->release();
    }
    delete im;
}

MgShapeDoc* MgShapeDoc::createDoc()
{
    return new MgShapeDoc();
}

void MgShapeDoc::addRef()
{
    giInterlockedIncrement(&im->refcount);
}

void MgShapeDoc::release()
{
    if (giInterlockedDecrement(&im->refcount) == 0) {
        delete this;
    }
}

MgObject* MgShapeDoc::clone() const
{
    MgObject* p = new MgShapeDoc();
    p->copy(*this);
    return p;
}

void MgShapeDoc::copy(const MgObject& src)
{
    if (src.isKindOf(Type())) {
        const MgShapeDoc& doc = (const MgShapeDoc&)src;
        im->xf = doc.im->xf;
        im->rectW = doc.im->rectW;
        im->viewScale = doc.im->viewScale;
    }
}

bool MgShapeDoc::equals(const MgObject& src) const
{
    if (src.isKindOf(Type())) {
        const MgShapeDoc& doc = (const MgShapeDoc&)src;

        if (im->xf != doc.im->xf
            || im->layers.size() != doc.im->layers.size()) {
            return false;
        }

        for (unsigned i = 0; i < im->layers.size(); i++) {
            if (!im->layers[i]->equals(*(doc.im->layers[i]))) {
                return false;
            }
        }

        return true;
    }

    return false;
}

GiContext* MgShapeDoc::context() { return &im->context; }
Matrix2d& MgShapeDoc::modelTransform() { return im->xf; }
Box2d MgShapeDoc::getPageRectW() const { return im->rectW; }
float MgShapeDoc::getViewScale() const { return im->viewScale; }
int MgShapeDoc::getChangeCount() const { return im->changeCount; }
MgLockRW* MgShapeDoc::getLockData() const { return &im->lock; }
MgLockRW* MgShapeDoc::getDynLockData() const { return &im->dynlock; }

void MgShapeDoc::setPageRectW(const Box2d& rectW, float viewScale)
{
    im->rectW = rectW;
    im->viewScale = viewScale;
}

void MgShapeDoc::clear()
{
    while (im->layers.size() > 1) {
        im->layers.back()->clear();
        im->layers.back()->release();
        im->layers.pop_back();
    }
    im->layers[0]->clear();
    im->curShapes = im->layers[0];
}

void MgShapeDoc::clearCachedData()
{
    for (unsigned i = 0; i < im->layers.size(); i++) {
        im->layers[i]->clearCachedData();
    }
}

Box2d MgShapeDoc::getExtent() const
{
    Box2d rect;

    for (unsigned i = 0; i < im->layers.size(); i++) {
        rect.unionWith(im->layers[i]->getExtent());
    }

    return rect;
}

int MgShapeDoc::getShapeCount() const
{
    int n = 0;

    for (unsigned i = 0; i < im->layers.size(); i++) {
        n += im->layers[i]->getShapeCount();
    }

    return n;
}

MgShapes* MgShapeDoc::getCurrentShapes() const
{
    return im->curShapes;
}

bool MgShapeDoc::setCurrentShapes(MgShapes* shapes)
{
    im->curShapes = shapes ? shapes : im->layers[0];
    return true;
}

int MgShapeDoc::getLayerCount() const
{
    return (int)im->layers.size();
}

bool MgShapeDoc::switchLayer(int index)
{
    bool ret = false;

    if (index == getLayerCount()) {
        im->layers.push_back(MgLayer::create(this, index));
    }
    if (index >= 0 && index < getLayerCount()) {
        im->curShapes = im->layers[index];
        ret = true;
    }

    return ret;
}

int MgShapeDoc::draw(GiGraphics& gs) const
{
    int n = 0;

    for (unsigned i = 0; i < im->layers.size(); i++) {
        n += im->layers[i]->draw(gs);
    }

    return n;
}

bool MgShapeDoc::lockData(int flags, int timeout)
{
    bool ret = im->lock.lockData(flags != 0, timeout);

    if (ret && flags && (flags & 0x100) == 0    // 0x100: NotNotify
        && im->lock.firstLocked())
    {
        im->lock.setEditFlags(flags);
        for (std::vector<Impl::Observer>::iterator it = im->observers.begin();
            it != im->observers.end(); ++it)
        {
            (it->first)(this, it->second, true);
        }
    }

    return ret;
}

long MgShapeDoc::unlockData(bool forWrite)
{
    MgBaseShape* owner = (MgBaseShape *)getCurrentShapes()->getOwner();
    if (owner->isKindOf(MgBaseShape::Type())) {
        owner->update();
    }
    return im->lock.unlockData(forWrite);
}

void MgShapeDoc::afterChanged()
{
    giInterlockedIncrement(&im->changeCount);

    for (std::vector<Impl::Observer>::iterator it = im->observers.begin();
        it != im->observers.end(); ++it)
    {
        (it->first)(this, it->second, false);
    }
}

bool MgShapeDoc::save(MgStorage* s, int startIndex) const
{
    bool ret = true;
    Box2d rect;

    if (!s || !s->writeNode("shapedoc", -1, false))
        return false;

    s->writeFloatArray("transform", &im->xf.m11, 6);
    s->writeFloatArray("zoomExtent", &im->rectW.xmin, 4);
    s->writeFloat("viewScale", im->viewScale);
    rect = getExtent();
    s->writeFloatArray("extent", &rect.xmin, 4);
    s->writeUInt32("count", 1);

    for (unsigned i = 0; i < im->layers.size(); i++) {
        ret = im->layers[i]->save(s, startIndex) || ret;
        startIndex = -1;
    }

    s->writeNode("shapedoc", -1, true);

    return ret;
}

bool MgShapeDoc::load(MgShapeFactory* factory, MgStorage* s, bool addOnly)
{
    bool ret = false;
    Box2d rect;

    if (!s || !s->readNode("shapedoc", -1, false)) {
        return s && s->setError("No root node.");
    }

    s->readFloatArray("transform", &im->xf.m11, 6);
    s->readFloatArray("zoomExtent", &im->rectW.xmin, 4);
    im->viewScale = s->readFloat("viewScale", im->viewScale);
    s->readFloatArray("extent", &rect.xmin, 4);
    s->readUInt32("count", 0);

    for (int i = 0; i < 99; i++) {
        if (i < getLayerCount()) {
            ret = im->layers[i]->load(factory, s, addOnly) || ret;
        }
        else {
            MgLayer* layer = MgLayer::create(this, i);
            if (layer->load(factory, s, addOnly)) {
                im->layers.push_back(layer);
                ret = true;
            }
            else {
                layer->release();
            }
        }
        addOnly = false;
    }

    s->readNode("shapedoc", -1, true);

    return ret;
}

void MgShapeDoc::registerObserver(ShapesLocked func, void* obj)
{
    if (func) {
        unregisterObserver(func, obj);
        im->observers.push_back(Impl::Observer(func, obj));
    }
}

void MgShapeDoc::unregisterObserver(ShapesLocked func, void* obj)
{
    for (std::vector<Impl::Observer>::iterator it = im->observers.begin();
         it != im->observers.end(); ++it) {
        if (it->first == func && it->second == obj) {
            im->observers.erase(it);
            break;
        }
    }
}
