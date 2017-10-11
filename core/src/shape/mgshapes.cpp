// mgshapes.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgstorage.h"
#include "mgspfactory.h"
#include "mglog.h"
#include "mgcomposite.h"
#include <list>
#include <map>
#include <set>

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
    volatile long refcount;
    
    MgShape* findShape(int sid) const;
    int getNewID(int sid);
    
    iterator findPositionOfID(int sid) {
        iterator it = shapes.begin();
        for (; it != shapes.end() && (*it)->getID() != sid; ++it) ;
        return it;
    }
    iterator findPositionOfIndex(int index) {
        iterator it = shapes.begin();
        for (int i = 0; it != shapes.end() && i != index; ++it, ++i) ;
        return it;
    }
    citerator findPositionOfIndex(int index) const {
        citerator it = shapes.begin();
        for (int i = 0; it != shapes.end() && i != index; ++it, ++i) ;
        return it;
    }
};

MgShapes* MgShapes::create(MgObject* owner, int index)
{
    return new MgShapes(owner, owner ? index : -1);
}

//static volatile long _n = 0;

MgShapes::MgShapes(MgObject* owner, int index)
{
    //LOGD("+MgShapes %ld", giAtomicIncrement(&_n));
    im = new I();
    im->owner = owner;
    im->index = index;
    im->newShapeID = 1;
    im->refcount = 1;
}

MgShapes::~MgShapes()
{
    clear();
    delete im;
    //LOGD("-MgShapes %ld", giAtomicDecrement(&_n));
}

void MgShapes::release()
{
    if (giAtomicDecrement(&im->refcount) == 0)
        delete this;
}

void MgShapes::addRef()
{
    giAtomicIncrement(&im->refcount);
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

MgShapes* MgShapes::shallowCopy() const
{
    MgShapes *p = new MgShapes(im->owner, im->index);
    p->copyShapes(this, false);
    return p;
}

int MgShapes::copyShapes(const MgShapes* src, bool deeply, bool needClear)
{
    if (needClear)
        clear();
    
    int ret = 0;
    MgShapeIterator it(src);
    
    while (MgShape* sp = const_cast<MgShape*>(it.getNext())) {
        if (deeply) {
            ret += addShape(*sp) ? 1 : 0;
        } else {
            sp->addRef();
            im->shapes.push_back(sp);
            im->id2shape[sp->getID()] = sp;
            ret++;
        }
    }
    
    return ret;
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
    return im->owner;
}

int MgShapes::getIndex() const
{
    return im->index;
}

bool MgShapes::updateShape(MgShape* shape, bool force)
{
    if (shape && (force || !shape->getParent() || shape->getParent() == this)) {
        I::iterator it = im->findPositionOfID(shape->getID());
        if (it != im->shapes.end()) {
            shape->shape()->update();
            shape->shape()->resetChangeCount((*it)->shapec()->getChangeCount()
                                             + ((*it)->equals(*shape) ? 0 : 1));
            (*it)->release();
            *it = shape;
            shape->setParent(this, shape->getID());
            im->id2shape[shape->getID()] = shape;
            return true;
        }
    }
    return false;
}

void MgShapes::transform(const Matrix2d& mat)
{
    for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        MgShape* newsp = (*it)->cloneShape();
        newsp->shape()->transform(mat);
        if (!updateShape(newsp, true))
            MgObject::release_pointer(newsp);
    }
}

MgShape* MgShapes::cloneShape(int sid) const
{
    const MgShape* p = im->findShape(sid);
    return p ? p->cloneShape() : MgShape::Null();
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

bool MgShapes::addShapeDirect(MgShape* shape, bool force)
{
    if (shape && (force || !shape->getParent() || shape->getParent() == this)) {
        shape->shape()->update();
        shape->setParent(this, im->getNewID(0));
        im->shapes.push_back(shape);
        im->id2shape[shape->getID()] = shape;
        return true;
    }
    return false;
}

bool MgShapes::removeShape(int sid)
{
    I::iterator it = im->findPositionOfID(sid);
    
    if (it != im->shapes.end()) {
        MgShape* shape = *it;
        im->shapes.erase(it);
        im->id2shape.erase(shape->getID());
        shape->release();
        return true;
    }
    
    return false;
}

bool MgShapes::moveShapeTo(int sid, MgShapes* dest)
{
    I::iterator it = im->findPositionOfID(sid);
    
    if (dest && dest != this && it != im->shapes.end()) {
        MgShape* newsp = (*it)->cloneShape();
        newsp->setParent(dest, dest->im->getNewID(newsp->getID()));
        dest->im->shapes.push_back(newsp);
        dest->im->id2shape[newsp->getID()] = newsp;
        
        return removeShape(sid);
    }
    
    return false;
}

void MgShapes::copyShapesTo(MgShapes* dest) const
{
    if (dest && dest != this) {
        for (I::iterator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
            MgShape* newsp = (*it)->cloneShape();
            newsp->setParent(dest, dest->im->getNewID(newsp->getID()));
            dest->im->shapes.push_back(newsp);
            dest->im->id2shape[newsp->getID()] = newsp;
        }
    }
}

bool MgShapes::bringToFront(int sid)
{
    I::iterator it = im->findPositionOfID(sid);
    
    if (it != im->shapes.end()) {
        MgShape* shape = *it;
        im->shapes.erase(it);
        im->shapes.push_back(shape);
        return true;
    }
    
    return false;
}

bool MgShapes::bringToBack(int sid)
{
    I::iterator it = im->findPositionOfID(sid);
    
    if (it != im->shapes.end()) {
        MgShape* shape = *it;
        im->shapes.erase(it);
        im->shapes.push_front(shape);
        return true;
    }
    
    return false;
}

bool MgShapes::bringToIndex(int sid, int index)
{
    I::iterator it = im->findPositionOfID(sid);
    
    if (it != im->shapes.end()) {
        MgShape* shape = *it;
        im->shapes.erase(it);
        it = im->findPositionOfIndex(index);
        im->shapes.insert(it, shape);
        return true;
    }
    
    return false;
}

bool MgShapes::reorderShapes(int n, const int *ids)
{
    I::Container shapes;
    std::set<int> newids;
    
    for (int i = 0; i < n; i++) {
        const MgShape* sp = findShape(ids[i]);
        if (sp) {
            shapes.push_back((MgShape*)sp);
            newids.insert(sp->getID());
        }
    }
    if (!newids.empty() && newids.size() == im->shapes.size()) {
        im->shapes = shapes;
        return true;
    }
    return false;
}

int MgShapes::getShapeCount() const
{
    return (int)im->shapes.size();
}

void MgShapes::freeIterator(void*& it) const
{
    if (it) {
        delete (I::citerator*)it;
        it = (void*)0;
    }
}

const MgShape* MgShapes::getFirstShape(void*& it) const
{
    if (im->shapes.empty()) {
        it = NULL;
        return MgShape::Null();
    }
    it = (void*)(new I::citerator(im->shapes.begin()));
    return im->shapes.empty() ? MgShape::Null() : im->shapes.front();
}

const MgShape* MgShapes::getNextShape(void*& it) const
{
    I::citerator* pit = (I::citerator*)it;
    if (pit && *pit != im->shapes.end()) {
        ++(*pit);
        if (*pit != im->shapes.end())
            return *(*pit);
    }
    return MgShape::Null();
}

const MgShape* MgShapes::getHeadShape() const
{
    return im->shapes.empty() ? MgShape::Null() : im->shapes.front();
}

const MgShape* MgShapes::getLastShape() const
{
    return im->shapes.empty() ? MgShape::Null() : im->shapes.back();
}

const MgShape* MgShapes::findShape(int sid) const
{
    return im->findShape(sid);
}

const MgShape* MgShapes::findShapeByTag(int tag) const
{
    if (0 == tag) {
        return MgShape::Null();
    }
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((*it)->getTag() == tag)
            return *it;
    }
    return MgShape::Null();
}

int MgShapes::getShapeCountByTypeOrTag(int type, int tag) const
{
    int n = 0;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((type != 0 && type == (*it)->shapec()->getType()) ||
            (tag != 0 && tag == (*it)->getTag())) {
            n++;
        }
    }
    return n;
}

int MgShapes::getShapeIndex(int sid) const
{
    int i = 0;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it, ++i) {
        if ((*it)->getID() == sid)
            return i;
    }
    return -1;
}

const MgShape* MgShapes::getShapeAtIndex(int index) const
{
    I::citerator it = im->findPositionOfIndex(index);
    return it != im->shapes.end() ? *it : MgShape::Null();
}

const MgShape* MgShapes::findShapeByType(int type) const
{
    if (0 == type) {
        return MgShape::Null();
    }
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((*it)->shapec()->getType() == type)
            return *it;
    }
    return MgShape::Null();
}

const MgShape* MgShapes::findShapeByTypeAndTag(int type, int tag) const
{
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        if ((*it)->shapec()->getType() == type && (*it)->getTag() == tag)
            return *it;
    }
    return MgShape::Null();
}

int MgShapes::traverseByType(int type, void (*c)(const MgShape*, void*), void* d)
{
    int count = 0;
    
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        const MgBaseShape* shape = (*it)->shapec();
        if (type == 0 || shape->isKindOf(type)) {
            (*c)(*it, d);
            count++;
        } else if (shape->isKindOf(MgComposite::Type())) {
            const MgComposite *composite = (const MgComposite *)shape;
            count += composite->shapes()->traverseByType(type, c, d);
        }
    }
    
    return count;
}

const MgShape* MgShapes::getParentShape(const MgShape* shape)
{
    const MgComposite *composite = (const MgComposite*)0;
    
    if (shape && shape->getParent()
        && shape->getParent()->getOwner()->isKindOf(MgComposite::Type())) {
        composite = (const MgComposite *)(shape->getParent()->getOwner());
    }
    return composite ? composite->getOwnerShape() : MgShape::Null();
}

static const float EXTENT_LIMIT = 1e5f - 1.f;

Box2d MgShapes::getExtent() const
{
    Box2d extent;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        Box2d box((*it)->shapec()->getExtent());
        if (box.xmin > -EXTENT_LIMIT && box.ymin > -EXTENT_LIMIT &&
            box.xmax <  EXTENT_LIMIT && box.ymax <  EXTENT_LIMIT) {
            extent.unionWith(box);
        }
    }
    
    return extent;
}

static bool isVisibleAndLocked(const MgBaseShape* shape)
{
    return (shape->isVisible() && (!shape->isLocked() || shape->getFlag(kMgCanSelLocked)));
}

const MgShape* MgShapes::hitTest(const Box2d& limits, MgHitResult& res,
                                 Filter filter, void* data) const
{
    const MgShape* retshape = MgShape::Null();
    
    res.dist = limits.width() > 1e4f ? limits.width() : limits.width() * 20.f;
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end(); ++it) {
        const MgBaseShape* shape = (*it)->shapec();
        Box2d extent(shape->getExtent());
        
        if ((filter || isVisibleAndLocked(shape))
            && extent.isIntersect(limits)
            && (!filter || filter(*it, data)))
        {
            MgHitResult tmpRes;
            float  tol = (!(*it)->hasFillColor() ? limits.width() / 2
                          : mgMax(extent.width(), extent.height()));
            float  dist = shape->hitTest(limits.center(), tol, tmpRes);
            
            tmpRes.contained = limits.contains(extent);
            if (res.contained == tmpRes.contained
                ? res.dist > dist - _MGZERO         // 让末尾图形优先选中
                : tmpRes.contained)                 // 在捕捉盒子内的小图形优先
            {
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

int MgShapes::dyndraw(int mode, GiGraphics& gs, const GiContext *ctx,
                      int segment, const int* ignoreIds) const
{
    Box2d clip(gs.getClipModel());
    int count = 0;
    
    for (I::citerator it = im->shapes.begin(); it != im->shapes.end() && !gs.isStopping(); ++it) {
        const MgShape* sp = *it;
        if (ignoreIds) {
            for (int i = 0; ignoreIds[i]; i++) {
                if (sp->getID() == ignoreIds[i]) {
                    sp = MgShape::Null();
                    break;
                }
            }
        }
        if (sp && sp->shapec()->isVisible() && sp->shapec()->getExtent().isIntersect(clip)) {
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
        s->writeInt("count", (int)im->shapes.size() - startIndex);
        
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

bool MgShapes::saveShape(MgStorage* s, const MgShape* shape, int index) const
{
    bool ret = shape && s->writeNode("shape", index, false);
    
    if (ret) {
        s->writeInt("type", shape->getType() & 0xFFFF);
        s->writeInt("id", shape->getID());
        
        Box2d rect(shape->shapec()->getExtent());
        s->writeFloatArray("extent", &rect.xmin, 4);
        
        ret = shape->save(s);
        s->writeNode("shape", index, true);
    }
    
    return ret;
}

int MgShapes::load(MgShapeFactory* factory, MgStorage* s, bool addOnly)
{
    Box2d rect;
    int index = 0, count = 0;
    bool ret = s && s->readNode("shapes", im->index, false);
    
    if (ret) {
        if (!addOnly)
            clear();
        
        ret = loadExtra(s);
        s->readFloatArray("extent", &rect.xmin, 4, false);
        int n = s->readInt("count", 0);
        
        for (; ret && s->readNode("shape", index, false); n--) {
            const int type = s->readInt("type", 0);
            const int sid = s->readInt("id", 0);
            s->readFloatArray("extent", &rect.xmin, 4, false);
            
            const MgShape* oldsp = addOnly && sid ? findShape(sid) : MgShape::Null();
            MgShape* newsp = factory->createShape(type);
            
            if (oldsp && oldsp->shapec()->getType() != type) {
                oldsp = MgShape::Null();
            }
            if (newsp) {
                newsp->setParent(this, oldsp ? sid : im->getNewID(sid));
                newsp->shape()->setExtent(rect);
                ret = newsp->load(factory, s);
                if (ret) {
                    count++;
                    newsp->shape()->setFlag(kMgClosed, newsp->shape()->isClosed());
                    im->id2shape[newsp->getID()] = newsp;
                    if (oldsp) {
                        updateShape(newsp);
                    }
                    else {
                        im->shapes.push_back(newsp);
                    }
                }
                else {
                    newsp->release();
                    LOGE("Fail to load shape (id=%d, type=%d)", sid, type);
                }
            } else {
                LOGE("Ignore unknown shape type %d, id=%d", type, sid);
            }
            s->readNode("shape", index++, true);
        }
        s->readNode("shapes", im->index, true);
    }
    else if (s && im->index == 0) {
        s->setError("No shapes node.");
    }
    
    return ret ? count : (count > 0 ? -count : -1);
}

void MgShapes::setNewShapeID(int sid)
{
    im->newShapeID = sid;
}

MgShape* MgShapes::I::findShape(int sid) const
{
    if (0 == sid || -1 == sid)
        return MgShape::Null();
    ID2SHAPE::const_iterator it = id2shape.find(sid);
    return it != id2shape.end() ? it->second : MgShape::Null();
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
