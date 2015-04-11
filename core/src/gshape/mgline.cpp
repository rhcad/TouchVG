// mgline.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgline.h"
#include "mgshape_.h"

static const float RAYMUL = 1e5f;

MG_IMPLEMENT_CREATE(MgLine)

MgLine::MgLine() : _subtype(0)
{
}

MgLine::~MgLine()
{
}

int MgLine::_getPointCount() const
{
    return 2;
}

Point2d MgLine::_getPoint(int index) const
{
    return _points[index ? 1 : 0];
}

void MgLine::_setPoint(int index, const Point2d& pt)
{
    _points[index ? 1 : 0] = pt;
}

int MgLine::_getHandleCount() const
{
    return _subtype ? 2 : 3;
}

Point2d MgLine::_getHandlePoint(int index) const
{
    return index < 2 ? _getPoint(index) : center();
}

bool MgLine::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    return index < 2 && __super::_setHandlePoint(index, pt, tol);
}

bool MgLine::_isHandleFixed(int index) const
{
    return index >= 2;
}

int MgLine::_getHandleType(int index) const
{
    return index >= 2 ? kMgHandleMidPoint : __super::_getHandleType(index);
}

void MgLine::_copy(const MgLine& src)
{
    _points[0] = src._points[0];
    _points[1] = src._points[1];
    _subtype = src._subtype;
    __super::_copy(src);
}

bool MgLine::_equals(const MgLine& src) const
{
    return (_points[0] == src._points[0]
            && _points[1] == src._points[1]
            && _subtype == src._subtype
            && __super::_equals(src));
}

void MgLine::_update()
{
    Vector2d vec((_points[1] - _points[0]) * RAYMUL);
    _extent.set(isBeeline() ? _points[0] - vec : _points[0],
                _subtype ? _points[1] + vec : _points[1]);
    __super::_update();
}

void MgLine::_transform(const Matrix2d& mat)
{
    _points[0] *= mat;
    _points[1] *= mat;
    __super::_transform(mat);
}

void MgLine::_clear()
{
    _points[1] = _points[0];
    __super::_clear();
}

float MgLine::_hitTest(const Point2d& pt, float, MgHitResult& res) const
{
    if (isRayline()) {
        float d = mglnrel::ptToBeeline2(_points[0], _points[1], pt, res.nearpt);
        float v = (res.nearpt - _points[0]).projectScaleToVector(_points[1] - _points[0]);
        if (v < 0) {
            res.nearpt = _points[0];
            d = pt.distanceTo(_points[0]);
        }
        return d;
    }
    if (isBeeline()) {
        return mglnrel::ptToBeeline2(_points[0], _points[1], pt, res.nearpt);
    }
    return (res.ignoreHandle == 0 ? _FLT_MAX : mglnrel::ptToLine(_points[0], _points[1], pt, res.nearpt));
}

bool MgLine::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    Vector2d vec((_points[1] - _points[0]) * RAYMUL);
    Point2d pts[2] = { isBeeline() ? _points[0] - vec : _points[0],
        _subtype ? _points[1] + vec : _points[1] };
    return mglnrel::clipLine(pts[0], pts[1], rect);
}

void MgLine::_output(MgPath& path) const
{
    Vector2d vec((_points[1] - _points[0]) * RAYMUL);
    Point2d pts[2] = { isBeeline() ? _points[0] - vec : _points[0],
        _subtype ? _points[1] + vec : _points[1] };
    path.moveTo(pts[0]);
    path.lineTo(pts[1]);
}

bool MgLine::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 4);
    s->writeInt("subtype", _subtype);
    return ret;
}

bool MgLine::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    _subtype = s->readInt("subtype", _subtype);
    return s->readFloatArray("points", &(_points[0].x), 4) == 4 && ret;
}
