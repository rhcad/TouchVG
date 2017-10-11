//! \file mgshapedoc.cpp
//! \brief 实现图形文档 MgShapeDoc
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgshapedoc.h"
#include "mgstorage.h"
#include <vector>
#include "mglayer.h"
#include "mgcomposite.h"
#include "mglog.h"

struct MgShapeDoc::Impl {
    std::vector<MgLayer*> layers;
    MgLayer*    curLayer;
    MgShapes*   curShapes;
    GiContext   context;
    Matrix2d    xf;
    Box2d       rectW;
    Box2d       rectWInitial;
    float       viewScale;
    volatile long   refcount;
    bool        readOnly;
};

//static volatile long _n = 0;

MgShapeDoc::MgShapeDoc()
{
    //LOGD("+MgShapeDoc %ld", giAtomicIncrement(&_n));
    im = new Impl();
    im->curLayer = MgLayer::create(this, 0);
    im->layers.push_back(im->curLayer);
    im->curShapes = im->curLayer;
    im->viewScale = 0;
    im->readOnly = false;
    im->refcount = 1;
}

MgShapeDoc::~MgShapeDoc()
{
    for (unsigned i = 0; i < im->layers.size(); i++) {
        im->layers[i]->release();
    }
    delete im;
    //LOGD("-MgShapeDoc %ld", giAtomicDecrement(&_n));
}

MgShapeDoc* MgShapeDoc::createDoc()
{
    return new MgShapeDoc();
}

void MgShapeDoc::addRef()
{
    giAtomicIncrement(&im->refcount);
}

void MgShapeDoc::release()
{
    if (giAtomicDecrement(&im->refcount) == 0) {
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
        im->context = doc.im->context;
    }
}

MgShapeDoc* MgShapeDoc::shallowCopy() const
{
    MgShapeDoc* p = new MgShapeDoc();
    p->copyShapes(this, false);
    return p;
}

int MgShapeDoc::copyShapes(const MgShapeDoc* src, bool deeply)
{
    unsigned i;
    int ret = 0;
    
    copy(*src);
    
    for (i = 0; i < im->layers.size() && i < src->im->layers.size(); i++) {
        ret += im->layers[i]->copyShapes(src->im->layers[i], deeply);
    }
    for (; i < src->im->layers.size(); i++) {
        MgLayer* newLayer = MgLayer::create(this, i);
        ret += newLayer->copyShapes(src->im->layers[i], deeply);
        im->layers.push_back(newLayer);
    }
    while (i < im->layers.size()) {
        im->layers.back()->release();
        im->layers.pop_back();
    }
    
    im->curLayer = im->layers[src->im->curLayer->getIndex()];
    im->curShapes = im->curLayer;
    
    const MgObject* owner = src->getCurrentShapes()->getOwner();
    if (owner && owner->isKindOf(MgComposite::Type())) {
        int sid = ((const MgComposite*)owner)->getOwnerShape()->getID();
        const MgShape* sp = im->curShapes->findShape(sid);
        
        if (sp && sp->shapec()->isKindOf(MgComposite::Type())) {
            im->curShapes = ((const MgComposite*)sp->shapec())->shapes();
        }
    }
    
    return ret;
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
const Box2d& MgShapeDoc::getPageRectW() const { return im->rectW; }
float MgShapeDoc::getViewScale() const { return im->viewScale; }
bool MgShapeDoc::isReadOnly() const { return im->readOnly; }
void MgShapeDoc::setReadOnly(bool readOnly) { im->readOnly = readOnly; }

void MgShapeDoc::setPageRectW(const Box2d& rectW, float viewScale, bool resetInitial)
{
    im->rectW = rectW;
    im->viewScale = viewScale;
    if (resetInitial)
        im->rectWInitial = rectW;
}

void MgShapeDoc::clear()
{
    while (im->layers.size() > 1) {
        im->layers.back()->clear();
        im->layers.back()->release();
        im->layers.pop_back();
    }
    im->layers[0]->clear();
    im->curLayer = im->layers[0];
    im->curShapes = im->curLayer;
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
        if (!im->layers[i]->isHided()) {
            rect.unionWith(im->layers[i]->getExtent());
        }
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

const MgShape* MgShapeDoc::findShape(int sid) const
{
    for (unsigned i = 0; i < im->layers.size(); i++) {
        const MgShape* sp = im->layers[i]->findShape(sid);
        if (sp) {
            return sp;
        }
    }
    return MgShape::Null();
}

const MgShape* MgShapeDoc::getLastShape() const
{
    return im->curLayer->getLastShape();
}

MgShapes* MgShapeDoc::getCurrentShapes() const
{
    return im->curShapes;
}

bool MgShapeDoc::setCurrentShapes(MgShapes* shapes)
{
    im->curShapes = shapes ? shapes : im->curLayer;
    return true;
}

MgLayer* MgShapeDoc::getCurrentLayer() const
{
    return im->curLayer;
}

int MgShapeDoc::getLayerCount() const
{
    return (int)im->layers.size();
}

bool MgShapeDoc::switchLayer(int index)
{
    bool ret = false;

    if (index == getLayerCount()) {
        im->curLayer = MgLayer::create(this, index);
        im->layers.push_back(im->curLayer);
    }
    if (index >= 0 && index < getLayerCount()) {
        im->curLayer = im->layers[index];
        im->curShapes = im->curLayer;
        ret = true;
    }

    return ret;
}

int MgShapeDoc::draw(GiGraphics& gs) const
{
    int n = 0;

    for (unsigned i = 0; i < im->layers.size(); i++) {
        if (!im->layers[i]->isHided()) {
            n += im->layers[i]->draw(gs);
        }
    }

    return n;
}

int MgShapeDoc::dyndraw(int mode, GiGraphics& gs) const
{
    return dyndraw(mode, gs, (const int*)0);
}

int MgShapeDoc::dyndraw(int mode, GiGraphics& gs, const int* ignoreIds) const
{
    int n = 0;
    
    for (unsigned i = 0; i < im->layers.size(); i++) {
        if (!im->layers[i]->isHided()) {
            n += im->layers[i]->dyndraw(mode, gs, (const GiContext*)0, -1, ignoreIds);
        }
    }
    
    return n;
}

bool MgShapeDoc::save(MgStorage* s, int startIndex) const
{
    bool ret = true;
    Box2d rect;

    if (!s || !s->writeNode("shapedoc", -1, false)) {
        return false;
    }

    if (startIndex == 0) {
        s->writeFloatArray("transform", &im->xf.m11, 6);
        s->writeFloatArray("pageExtent", im->rectWInitial.isEmpty() ?
                           &im->rectW.xmin : &im->rectWInitial.xmin, 4);
        s->writeFloat("viewScale", im->viewScale);
        rect = getExtent();
        s->writeFloatArray("extent", &rect.xmin, 4);
        s->writeInt("count", (int)im->layers.size());
    }

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
        return s && s->setError("No shapedoc node.");
    }

    if (!addOnly) {
        s->readFloatArray("transform", &im->xf.m11, 6, false);
        im->rectWInitial.empty();
        if (s->readFloatArray("pageExtent", &im->rectW.xmin, 4, false) == 4 ||
            s->readFloatArray("zoomExtent", &im->rectW.xmin, 4, false) == 4) {
            im->rectWInitial = im->rectW;
        }
        im->viewScale = s->readFloat("viewScale", im->viewScale);
        s->readFloatArray("extent", &rect.xmin, 4, false);
        s->readInt("count", 0);
    }

    for (int i = 0; i < 99; i++) {
        if (i < getLayerCount()) {
            ret = im->layers[i]->load(factory, s, addOnly) >= 0 || ret;
        }
        else {
            MgLayer* layer = MgLayer::create(this, i);
            if (layer->load(factory, s, addOnly) >= 0) {
                im->layers.push_back(layer);
                ret = true;
            }
            else {
                layer->release();
                break;
            }
        }
        addOnly = false;
    }

    s->readNode("shapedoc", -1, true);

    return ret;
}

bool MgShapeDoc::saveAll(MgStorage* s, const GiTransform* xform)
{
    if (xform && im->rectWInitial.isEmpty()) {
        im->rectW = xform->getWndRectW();
        im->viewScale = xform->getViewScale();
    }
    return save(s, 0);
}

bool MgShapeDoc::loadAll(MgShapeFactory* factory, MgStorage* s, GiTransform* xform)
{
    im->rectW.set(0.f, 0.f, 1024.f, 768.f);
    im->viewScale = 1.f;
    
    bool ret = load(factory, s, false);
    if (ret && xform) {
        xform->setModelTransform(im->xf);
        xform->zoomTo(im->rectWInitial.isEmpty() ? im->rectW : im->rectWInitial);
    }
    
    return ret;
}

bool MgShapeDoc::zoomToInitial(GiTransform* xform)
{
    bool ret = xform && !im->rectWInitial.isEmpty();
    if (ret) {
        xform->setModelTransform(im->xf);
        xform->zoomTo(im->rectWInitial);
    }
    return ret;
}
