// mgshapes.cpp: 实现图形列表类 MgShapes
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshapes.h"
#include "mgstorage.h"
#include <mgspfactory.h>
#include <list>
#include <map>

struct MgShapes::I
{
    typedef std::list<MgShape*> Container;
    typedef Container::const_iterator citerator;
    typedef Container::iterator iterator;
    typedef std::map<int, MgShape*>  ID2SHAPE;
    
    Container   shapes;
    ID2SHAPE    id2shape;
    MgObject*   owner;
    int         index;
    int         newShapeID;
    
    MgShape* findShape(int sid) const;
    int getNewID(int sid);
};

MgShapes* MgShapes::create(MgObject* owner, int index)
{
    return new MgShapes(owner, owner ? index : -1);
}

MgShapes::MgShapes(MgObject* owner, int index)
{
    im = new I();
    im->owner = owner;
    im->index = index;
    im->newShapeID = 1;
}

MgShapes::~MgShapes()
{
    clear();
    delete im;
}

void MgShapes::release()
{
    delete this;
}

MgObject* MgShapes::clone() const
{
    MgShapes *p = new MgShapes(im->owner, im->index);
    p->copy(*this);
    return p;
}

void MgShapes::copy(const MgObject&)
{
}

void MgShapes::copyShapes(const MgShapes* src)
{
    clear();
    MgShapeIterator it(src);
    while (MgShape* sp = it.getNext()) {
        addShape(*sp);
    }
}

bool MgShapes::equals(const MgObject& src) const
{
    bool ret = false;
    
    if (src.isKindOf(Type())) {
        const MgShapes& _src = (const MgShapes&)src;
        ret = (im->shapes == _src.im->shapes);
    }
    
    return ret;
}

void MgShapes::clear()
{
    for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        (*it)->release();
    }
    im->shapes.clear();
    im->id2shape.clear();
}

void MgShapes::clearCachedData()
{
    for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        (*it)->shape()->clearCachedData();
    }
}

MgObject* MgShapes::getOwner() const
{
    return this ? im->owner : NULL;
}

int MgShapes::getIndex() const
{
    return im->index;
}

MgShape* MgShapes::addShape(const MgShape& src)
{
    MgShape* p = src.cloneShape();
    if (p) {
        p->setParent(this, im->getNewID(src.getID()));
        im->shapes.push_back(p);
        im->id2shape[p->getID()] = p;
    }
    return p;
}

MgShape* MgShapes::addShapeByType(MgShapeFactory* factory, int type)
{
    MgShape* p = factory->createShape(type);
    if (p) {
        p->setParent(this, im->getNewID(0));
        im->shapes.push_back(p);
        im->id2shape[p->getID()] = p;
    }
    return p;
}

MgShape* MgShapes::removeShape(int sid, bool skipLockedShape)
{
    for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        MgShape* shape = *it;
        if (shape->getID() == sid) {
            if (skipLockedShape && shape->shapec()->getFlag(kMgShapeLocked)) {
                return NULL;
            }
            im->shapes.erase(it);
            im->id2shape.erase(shape->getID());
            return shape;
        }
    }
    return NULL;
}

MgShape* MgShapes::moveTo(int sid, MgShapes* dest)
{
    MgShape* shape = NULL;
    
    if (dest && dest != this) {
        shape = removeShape(sid, false);
        
        if (shape && dest->isKindOf(Type())) {
            MgShapes* d = (MgShapes*)dest;
            
            shape->setParent(d, d->im->getNewID(shape->getID()));
            d->im->shapes.push_back(shape);
            d->im->id2shape[sid] = shape;
        }
        else if (shape) {
            MgShape* newsp = addShape(*shape);
            shape->release();
            shape = newsp;
        }
    }
    
    return shape;
}

void MgShapes::moveAllShapesTo(MgShapes* dest)
{
    while (!im->shapes.empty()) {
        moveTo(im->shapes.back()->getID(), dest);
    }
}

bool MgShapes::bringToFront(int sid)
{
    for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        MgShape* shape = *it;
        if (shape->getID() == sid) {
            im->shapes.erase(it);
            im->shapes.push_back(shape);
            return true;
        }
    }
    return false;
}

int MgShapes::getShapeCount() const
{
    return this ? (int)im->shapes.size() : 0;
}

void MgShapes::freeIterator(void*& it) const
{
    if (it) {
        delete (I::citerator*)it;
        it = NULL;
    }
}

MgShape* MgShapes::getFirstShape(void*& it) const
{
    if (!this || im->shapes.empty()) {
        it = NULL;
        return NULL;
    }
    it = (void*)(new I::citerator(im->shapes.begin()));
    return im->shapes.empty() ? NULL : im->shapes.front();
}

MgShape* MgShapes::getNextShape(void*& it) const
{
    I::citerator* pit = (I::citerator*)it;
    if (pit && *pit != im->shapes.end()) {
        ++(*pit);
        if (*pit != im->shapes.end())
            return *(*pit);
    }
    return NULL;
}

MgShape* MgShapes::getHeadShape() const
{
    return (!this || im->shapes.empty()) ? NULL : im->shapes.front();
}

MgShape* MgShapes::getLastShape() const
{
    return (!this || im->shapes.empty()) ? NULL : im->shapes.back();
}

MgShape* MgShapes::findShape(int sid) const
{
    return im->findShape(sid);
}

MgShape* MgShapes::findShapeByTag(int tag) const
{
    if (!this || 0 == tag)
        return NULL;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((*it)->getTag() == tag)
            return *it;
    }
    return NULL;
}

MgShape* MgShapes::findShapeByType(int type) const
{
    if (!this || 0 == type)
        return NULL;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((*it)->shapec()->getType() == type)
            return *it;
    }
    return NULL;
}

Box2d MgShapes::getExtent() const
{
    Box2d extent;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        extent.unionWith((*it)->shapec()->getExtent());
    }
    
    return extent;
}

MgShape* MgShapes::hitTest(const Box2d& limits, MgHitResult& res, Filter filter) const
{
    MgShape* retshape = NULL;
    
    res.dist = _FLT_MAX;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        const MgBaseShape* shape = (*it)->shapec();
        Box2d extent(shape->getExtent());
        
        if (!shape->getFlag(kMgShapeLocked)
            && extent.isIntersect(limits)
            && (!filter || filter(*it))) {
            MgHitResult tmpRes;
            float  tol = (!(*it)->hasFillColor() ? limits.width() / 2
                          : mgMax(extent.width(), extent.height()));
            float  dist = shape->hitTest(limits.center(), tol, tmpRes);
            
            if (res.dist > dist - _MGZERO) {     // 让末尾图形优先选中
                res = tmpRes;
                res.dist = dist;
                retshape = *it;
            }
        }
    }
    
    return retshape;
}

int MgShapes::draw(GiGraphics& gs, const GiContext *ctx) const
{
    return dyndraw(0, gs, ctx, -1);
}

int MgShapes::dyndraw(int mode, GiGraphics& gs, const GiContext *ctx, int segment) const
{
    Box2d clip(gs.getClipModel());
    int count = 0;
    
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end() && !gs.isStopping(); ++it) {
        const MgShape* sp = *it;
        if (sp->getParent() == this && sp->shapec()->getExtent().isIntersect(clip)) {
            if (sp->draw(mode, gs, ctx, segment))
                count++;
        }
    }
    
    return count;
}

bool MgShapes::save(MgStorage* s, int startIndex) const
{
    bool ret = false;
    Box2d rect;
    int index = 0;
    
    if (s && s->writeNode("shapes", im->index, false)) {
        ret = saveExtra(s);
        rect = getExtent();
        s->writeFloatArray("extent", &rect.xmin, 4);
        s->writeUInt("count", (int)im->shapes.size() - startIndex);
        
        for (I::citerator it = im->shapes.begin();
             ret && it != im->shapes.end(); ++it, ++index)
        {
            if (index < startIndex)
                continue;
            ret = saveShape(s, *it, index - startIndex);
        }
        s->writeNode("shapes", im->index, true);
    }
    
    return ret;
}

bool MgShapes::saveShape(MgStorage* s, MgShape* shape, int index) const
{
    bool ret = s->writeNode("shape", index, false);
    
    if (ret) {
        s->writeUInt("type", shape->getType() & 0xFFFF);
        s->writeUInt("id", shape->getID());
        
        Box2d rect(shape->shapec()->getExtent());
        s->writeFloatArray("extent", &rect.xmin, 4);
        
        ret = shape->save(s);
        s->writeNode("shape", index, true);
    }
    
    return ret;
}

bool MgShapes::load(MgShapeFactory* factory, MgStorage* s, bool addOnly)
{
    bool ret = false;
    Box2d rect;
    int index = 0;
    
    if (s && s->readNode("shapes", im->index, false)) {
        if (!addOnly)
            clear();
        
        ret = loadExtra(s);
        //s->readFloatArray("extent", &rect.xmin, 4);
        int n = s->readInt("count", 0);
        
        for (; ret && s->readNode("shape", index, false); n--) {
            const int type = s->readInt("type", 0);
            const int id = s->readInt("id", 0);
            s->readFloatArray("extent", &rect.xmin, 4);
            
            MgShape* shape = addOnly && id ? findShape(id) : NULL;
            
            if (shape && shape->shape()->getType() == type) {
                ret = shape->load(factory, s);
                shape->shape()->setFlag(kMgClosed, shape->shape()->isClosed());
            }
            else {
                shape = factory->createShape(type);
                if (shape) {
                    shape->setParent(this, im->getNewID(id));
                    ret = shape->load(factory, s);
                    if (ret) {
                        shape->shape()->setFlag(kMgClosed, shape->shape()->isClosed());
                        im->shapes.push_back(shape);
                        im->id2shape[shape->getID()] = shape;
                    }
                    else {
                        shape->release();
                    }
                }
            }
            s->readNode("shape", index++, true);
        }
        s->readNode("shapes", im->index, true);
    }
    else if (s && im->index == 0) {
        s->setError("No shapes node.");
    }
    
    return ret;
}

void MgShapes::setNewShapeID(int sid)
{
    im->newShapeID = sid;
}

MgShape* MgShapes::I::findShape(int sid) const
{
    if (!this || 0 == sid)
        return NULL;
    ID2SHAPE::const_iterator it = id2shape.find(sid);
    return it != id2shape.end() ? it->second : NULL;
}

int MgShapes::I::getNewID(int sid)
{
    if (0 == sid || findShape(sid)) {
        while (findShape(newShapeID))
            newShapeID++;
        sid = newShapeID++;
    }
    return sid;
}
