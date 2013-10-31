// mgcomposite.cpp: 实现复合图形基类 MgComposite
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <mgcomposite.h>

MgComposite::MgComposite()
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

int MgComposite::_getPointCount() const
{
    MgShape* sp = _shapes->getHeadShape();
    return sp ? sp->shapec()->getPointCount() : 0;
}

Point2d MgComposite::_getPoint(int index) const
{
    MgShape* sp = _shapes->getHeadShape();
    return sp ? sp->shapec()->getPoint(index) : Point2d();
}

void MgComposite::_setPoint(int, const Point2d&)
{
}

int MgComposite::_getHandleCount() const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (MgShape* sp = it.getNext()) {
        n += sp->shapec()->getHandleCount();
    }
    
    return n;
}

Point2d MgComposite::_getHandlePoint(int index) const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (MgShape* sp = it.getNext()) {
        int c = sp->shapec()->getHandleCount();
        if (index < n + c) {
            return sp->shapec()->getHandlePoint(index - n);
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
    
    while (MgShape* sp = it.getNext()) {
        int c = sp->shapec()->getHandleCount();
        if (index < n + c) {
            return sp->shapec()->getHandleType(index - n);
        }
        n += c;
    }
    
    return kMgHandleOutside;
}

bool MgComposite::_isHandleFixed(int index) const
{
    int n = 0;
    MgShapeIterator it(_shapes);
    
    while (MgShape* sp = it.getNext()) {
        int c = sp->shapec()->getHandleCount();
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
    MgShapeIterator it(_shapes);
    _extent.empty();

    while (MgShape* sp = it.getNext()) {
        sp->shape()->update();
        _extent.unionWith(sp->shapec()->getExtent());
    }
    __super::_update();
}

void MgComposite::_transform(const Matrix2d& mat)
{
    MgShapeIterator it(_shapes);
    while (MgShape* sp = it.getNext()) {
        sp->shape()->transform(mat);
    }
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

    while (MgShape* sp = it.getNext()) {
        if (limits.isIntersect(sp->shapec()->getExtent())) {
            float d = sp->shapec()->hitTest(pt, tol, tmpRes);
            if (res.dist > d - _MGZERO)
            {
                res = tmpRes;
                res.dist = d;
                res.segment = sp->getID();
            }
        }
    }

    return res.segment != 0;
}

bool MgComposite::_offset(const Vector2d& vec, int)
{
    MgShapeIterator it(_shapes);
    int n = 0;

    while (MgShape* sp = it.getNext()) {
        n += sp->shape()->offset(vec, -1) ? 1 : 0;
    }

    return n > 0;
}

bool MgComposite::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int) const
{
    MgShapeIterator it(_shapes);
    int n = 0;

    while (MgShape* sp = it.getNext()) {
        n += sp->draw(mode, gs, ctx.isNullLine() ? NULL : &ctx, -1) ? 1 : 0;
    }

    return n > 0;
}

#include <mgshape_.h>

MG_IMPLEMENT_CREATE(MgGroup)

MgGroup::MgGroup()
{
}

MgGroup::~MgGroup()
{
}

bool MgGroup::_offset(const Vector2d& vec, int segment)
{
    MgShape* sp = _shapes->findShape(segment);

    if (sp && canOffsetShapeAlone(sp)) {
        return sp->shape()->offset(vec, -1);
    }

    return __super::_offset(vec, segment);
}

bool MgGroup::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    MgShape* sp = _shapes->findShape(segment);
    if (sp) {
        return sp->draw(mode, gs, ctx.isNullLine() ? NULL : &ctx, -1);
    }
    return __super::_draw(mode, gs, ctx, segment);
}

bool MgGroup::_save(MgStorage* s) const
{
    return __super::_save(s) && _shapes->save(s);
}

bool MgGroup::_load(MgShapeFactory* factory, MgStorage* s)
{
    return __super::_load(factory, s) && _shapes->load(factory, s);
}

bool MgGroup::addShape(MgShape* shape)
{
    MgShape* ret = NULL;

    if (shape && shape->getParent()) {
        ret = shape->getParent()->moveTo(shape->getID(), _shapes);
    }
    else if (shape) {
        ret = _shapes->addShape(*shape);
    }

    return !!ret;
}
