// mgcomposite.cpp: 实现复合图形基类 MgComposite
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcomposite.h"

MgComposite::MgComposite() : _owner(NULL)
{
    _shapes = MgShapes::create(this);
}

MgComposite::~MgComposite()
{ 
    _shapes->release();
}

bool MgComposite::_isKindOf(int type) const
{
    return type == Type() || MgBaseShape::_isKindOf(type);
}

void MgComposite::setOwner(MgObject* owner)
{
    if (!owner || owner->isKindOf(MgShape::Type())) {
        _owner = (MgShape*)owner;
    }
}

int MgComposite::_getPointCount() const
{
    const MgShape* sp = _shapes->getHeadShape();
    return sp ? sp->getPointCount() : 0;
}

Point2d MgComposite::_getPoint(int index) const
{
    const MgShape* sp = _shapes->getHeadShape();
    return sp ? sp->getPoint(index) : Point2d();
}

void MgComposite::_setPoint(int, const Point2d&)
{
}

int MgComposite::_getHandleCount() const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (const MgShape* sp = it.getNext()) {
        n += sp->getHandleCount();
    }
    
    return n;
}

Point2d MgComposite::_getHandlePoint(int index) const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (const MgShape* sp = it.getNext()) {
        int c = sp->getHandleCount();
        if (index < n + c) {
            return sp->getHandlePoint(index - n);
        }
        n += c;
    }
    
    return getExtent().center();
}

bool MgComposite::_setHandlePoint(int index, const Point2d& pt, float)
{
    return _offset(pt - _getHandlePoint(index), -1);
}

int MgComposite::_getHandleType(int index) const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (const MgShape* sp = it.getNext()) {
        int c = sp->getHandleCount();
        if (index < n + c) {
            return sp->getHandleType(index - n);
        }
        n += c;
    }
    
    return kMgHandleNoSnap;
}

bool MgComposite::_isHandleFixed(int index) const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (const MgShape* sp = it.getNext()) {
        int c = sp->getHandleCount();
        if (index < n + c) {
            return sp->shapec()->isHandleFixed(index - n);
        }
        n += c;
    }
    
    return true;
}

int MgComposite::getShapeCount() const
{
    return _shapes->getShapeCount();
}

void MgComposite::_clearCachedData()
{
    __super::_clearCachedData();
    _shapes->clearCachedData();
}

void MgComposite::_update()
{
    _extent = _shapes->getExtent();
    __super::_update();
}

void MgComposite::_transform(const Matrix2d& mat)
{
    MgShapeIterator it(_shapes);
    while (MgShape* sp = const_cast<MgShape*>(it.getNext())) {
        sp->shape()->transform(mat);
    }
    _extent = _shapes->getExtent();
}

void MgComposite::_clear()
{
    _shapes->clear();
    MgBaseShape::_clear();
}

void MgComposite::_copy(const MgComposite& src)
{
    _shapes->copyShapes(src._shapes);
    __super::_copy(src);
}

bool MgComposite::_equals(const MgComposite& src) const
{
    return _shapes->equals(*(src._shapes)) && __super::_equals(src);
}

float MgComposite::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    MgShapeIterator it(_shapes);
    MgHitResult tmpRes;
    Box2d limits(pt, 2 * tol, 0);

    res.segment = 0;
    res.dist = _FLT_MAX;

    while (const MgShape* sp = it.getNext()) {
        if (limits.isIntersect(sp->shapec()->getExtent())) {
            float d = sp->shapec()->hitTest(pt, tol, tmpRes);
            if (res.dist > d - _MGZERO) {
                res = tmpRes;
                res.dist = d;
                res.segment = sp->getID();
            }
        }
    }

    return res.dist;
}

bool MgComposite::_hitTestBox(const Box2d& rect) const
{
    MgHitResult res;
    return !!_shapes->hitTest(rect, res);
}

bool MgComposite::_offset(const Vector2d& vec, int)
{
    MgShapeIterator it(_shapes);
    int n = 0;

    while (MgShape* sp = const_cast<MgShape*>(it.getNext())) {
        n += sp->shape()->offset(vec, -1) ? 1 : 0;
    }

    return n > 0;
}

bool MgComposite::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int) const
{
    MgShapeIterator it(_shapes);
    int n = 0;

    while (const MgShape* sp = it.getNext()) {
        n += sp->draw(mode, gs, ctx.isNullLine() ? NULL : &ctx, -1) ? 1 : 0;
    }

    return n > 0;
}

void MgComposite::_output(MgPath& path) const
{
    MgShapeIterator it(_shapes);
    
    while (const MgShape* sp = it.getNext()) {
        sp->shapec()->output(path);
    }
}

#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgGroup)

MgGroup::MgGroup() : _insert(Point2d::kInvalid())
{
}

MgGroup::~MgGroup()
{
}

int MgGroup::_getPointCount() const
{
    return 2;
}

Point2d MgGroup::_getPoint(int index) const
{
    return index == 0 && !_box.isEmpty() ? _box.center() : _insert;
}

void MgGroup::_setPoint(int index, const Point2d& pt)
{
    if (index == 1) {
        _insert = pt;
    }
}

void MgGroup::_copy(const MgGroup& src)
{
    __super::_copy(src);
    _insert = src._insert;
    _name = src._name;
    _box = _shapes->getExtent();
}

bool MgGroup::_equals(const MgGroup& src) const
{
    return (_insert == src._insert && _name == src._name && __super::_equals(src));
}

void MgGroup::_update()
{
    __super::_update();
    _extent.unionWith(_insert);
    _box = _shapes->getExtent();
}

void MgGroup::_transform(const Matrix2d& mat)
{
    __super::_transform(mat);
    _insert *= mat;
    _box = _shapes->getExtent();
}

void MgGroup::_clear()
{
    _insert = Point2d::kOrigin();
    _box.empty();
    _name.clear();
    __super::_clear();
}

float MgGroup::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    float dist = _insert.distanceTo(pt);
    
    __super::_hitTest(pt, tol, res);
    if (res.dist > dist) {
        res.dist = dist;
        res.nearpt = _insert;
        res.segment = -1;
    }
    return res.dist;
}

bool MgGroup::_hitTestBox(const Box2d& rect) const
{
    return rect.contains(_insert) || __super::_hitTestBox(rect);
}

int MgGroup::_getHandleCount() const
{
    return hasInsertionPoint() ? 2 : 0;
}

Point2d MgGroup::_getHandlePoint(int index) const
{
    return _getPoint(index);
}

bool MgGroup::_setHandlePoint(int index, const Point2d& pt, float)
{
    Vector2d vec(pt - _getHandlePoint(index));
    return (vec.isZeroVector() ? false :
            index == 1 ? _offset(vec, -1) : __super::_offset(vec, -1));
}

bool MgGroup::_isHandleFixed(int index) const
{
    return false;
}

int MgGroup::_getHandleType(int index) const
{
    return index == 0 ? kMgHandleNoSnap : kMgHandleVertex;
}

bool MgGroup::_offset(const Vector2d& vec, int segment)
{
    MgShape* sp = const_cast<MgShape*>(_shapes->findShape(segment));

    if (sp && canOffsetShapeAlone(sp)) {
        return sp->shape()->offset(vec, -1);
    }
    if (!sp) {
        _insert += vec;
    }

    return __super::_offset(vec, segment);
}

bool MgGroup::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    const MgShape* sp = _shapes->findShape(segment);
    if (sp && mode > 0) {
        Point2d cen(_box.center());
        GiContext ctxln(0, GiColor(0, 126, 0, 128), GiContext::kDotLine);
        
        if (_insert != cen) {
            gs.drawLine(&ctxln, _insert, cen);
        }
        gs.drawRect(&ctxln, _box);
    }
    return __super::_draw(mode, gs, ctx, segment);
}

bool MgGroup::_save(MgStorage* s) const
{
    if (!_insert.isDegenerate()) {
        s->writeFloat("x", _insert.x);
        s->writeFloat("y", _insert.y);
        if (!_name.empty()) {
            s->writeString("name", _name.c_str());
        }
    }
    return __super::_save(s) && _shapes->save(s);
}

bool MgGroup::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s) && _shapes->load(factory, s) > 0;
    int len = s->readString("name");
    
    _insert.set(s->readFloat("x", _insert.x), s->readFloat("y", _insert.y));
    _name.resize(len);
    if (len > 0 && ret) {
        ret = s->readString("name", &_name[0], len) == len;
    }
    
    return ret;
}

bool MgGroup::addShapeToGroup(const MgShape* shape)
{
    if (shape && shape->getParent()) {
        return shape->getParent()->moveShapeTo(shape->getID(), _shapes);
    }
    return shape && _shapes->addShape(*shape);
}

void MgGroup::setName(const char* name)
{
    _name = name ? name : "";
}

const MgShape* MgGroup::findGroup(const MgShapes* shapes, const char* name)
{
    MgShapeIterator it(shapes);
    
    while (const MgShape* sp = it.getNext()) {
        if (sp->shapec()->isKindOf(Type())) {
            if (((const MgGroup*)sp->shapec())->_name == name) {
                return sp;
            }
        }
    }
    return NULL;
}
