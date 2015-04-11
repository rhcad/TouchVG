// mgparallel.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgparallel.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgParallel)

MgParallel::MgParallel()
{
}

MgParallel::~MgParallel()
{
}

int MgParallel::_getPointCount() const
{
    return 4;
}

Point2d MgParallel::_getPoint(int index) const
{
    return (index < 0 ? Point2d() : _points[index < 4 ? index : index % 4]);
}

void MgParallel::_setPoint(int index, const Point2d& pt)
{
    _points[index] = pt;
}

void MgParallel::_copy(const MgParallel& src)
{
    for (int i = 0; i < 4; i++)
        _points[i] = src._points[i];
    __super::_copy(src);
}

bool MgParallel::_equals(const MgParallel& src) const
{
    for (int i = 0; i < 4; i++) {
        if (_points[i] != src._points[i])
            return false;
    }
    return __super::_equals(src);
}

void MgParallel::_update()
{
    _points[3] = _points[0] + _points[2] + (- _points[1]);
    _extent.set(4, _points);
    if (_extent.isEmpty())
        _extent.set(_points[0], 2 * Tol::gTol().equalPoint(), 0);
    __super::_update();
}

void MgParallel::_transform(const Matrix2d& mat)
{
    mat.transformPoints(4, _points);
    __super::_transform(mat);
}

void MgParallel::_clear()
{
    for (int i = 1; i < 4; i++)
        _points[i] = _points[0];
    __super::_clear();
}

int MgParallel::_getHandleCount() const
{
    return 8;
}

Point2d MgParallel::_getHandlePoint(int index) const
{
    return (index < 4 ? __super::_getHandlePoint(index) :
            (_points[index % 4] + _points[(index + 1) % 4]) / 2);
}

int MgParallel::_getHandleType(int index) const
{
    return index < 4 ? __super::_getHandleType(index) : kMgHandleMidPoint;
}

bool MgParallel::_isHandleFixed(int index) const
{
    return index >= 4;
}

bool MgParallel::_setHandlePoint(int index, const Point2d& pt, float)
{
    index = index % 4;
    if (getFlag(kMgFixedLength)) {
        Point2d& basept = _points[(index - 1) % 4];
        _points[index] = basept.rulerPoint(pt, _points[index].distanceTo(basept), 0);
    }
    else {
        _points[index] = pt;
    }
    _points[(index + 1) % 4] = (_points[index] + _points[(index + 2) % 4]
                                + (- _points[(index + 3) % 4]));
    update();
    return true;
}

bool MgParallel::_offset(const Vector2d& vec, int segment)
{
    if (segment < 0 || getFlag(kMgFixedSize))
        return __super::_offset(vec, segment);
    return _setHandlePoint(segment, _points[segment] + vec, 0);
}

bool MgParallel::_rotateHandlePoint(int index, const Point2d& pt)
{
    return getFlag(kMgFixedSize) && __super::_rotateHandlePoint(index, pt);
}

float MgParallel::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    return linesHit(4, _points, true, pt, tol, res);
}

bool MgParallel::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;

    for (int i = 0; i < 4; i++) {
        if (Box2d(_points[i], _points[(i + 1) % 4]).isIntersect(rect))
            return true;
    }

    return false;
}

void MgParallel::_output(MgPath& path) const
{
    path.moveTo(_points[0]);
    path.linesTo(3, _points + 1);
    path.closeFigure();
}

bool MgParallel::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 8);
    return ret;
}

bool MgParallel::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    return s->readFloatArray("points", &(_points[0].x), 8) == 8 && ret;
}
