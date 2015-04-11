// mgdot.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdot.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgDot)

MgDot::MgDot() : _type(0)
{
}

MgDot::~MgDot()
{
}

int MgDot::_getPointCount() const
{
    return 1;
}

Point2d MgDot::_getPoint(int) const
{
    return _point;
}

void MgDot::_setPoint(int, const Point2d& pt)
{
    _point = pt;
}

void MgDot::_copy(const MgDot& src)
{
    _point = src._point;
    _type = src._type;
    __super::_copy(src);
}

bool MgDot::_equals(const MgDot& src) const
{
    return (_point == src._point && _type == src._type && __super::_equals(src));
}

void MgDot::_update()
{
    _extent.set(_point, 0.1f, 0.f);
    __super::_update();
}

void MgDot::_transform(const Matrix2d& mat)
{
    _point *= mat;
    __super::_transform(mat);
}

void MgDot::_clear()
{
    _point = Point2d::kOrigin();
    __super::_clear();
}

float MgDot::_hitTest(const Point2d& pt, float, MgHitResult& res) const
{
    res.nearpt = _point;
    return pt.distanceTo(_point);
}

void MgDot::_output(MgPath&) const
{
}

bool MgDot::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeInt("ptype", _type);
    s->writeFloat("x", _point.x);
    s->writeFloat("y", _point.y);
    return ret;
}

bool MgDot::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    _type = s->readInt("ptype", _type);
    _point.set(s->readFloat("x", _point.x), s->readFloat("y", _point.y));
    return ret;
}
