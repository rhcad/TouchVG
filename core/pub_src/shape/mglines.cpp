// mglines.cpp: 实现折线图形类 MgLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>

// MgBaseLines
//

MgBaseLines::MgBaseLines()
    : _points(NULL), _maxCount(0), _count(0)
{
}

MgBaseLines::~MgBaseLines()
{
    if (_points)
        delete[] _points;
}

int MgBaseLines::_getPointCount() const
{
    return _count;
}

Point2d MgBaseLines::_getPoint(int index) const
{
    return index >= 0 && index < _count ? _points[index] : Point2d();
}

void MgBaseLines::_setPoint(int index, const Point2d& pt)
{
    if (index >= 0 && index < _count) {
        _points[index] = pt;
    }
}

void MgBaseLines::_copy(const MgBaseLines& src)
{
    resize(src._count);
    for (int i = 0; i < _count; i++)
        _points[i] = src._points[i];

    __super::_copy(src);
}

bool MgBaseLines::_equals(const MgBaseLines& src) const
{
    if (_count != src._count)
        return false;

    for (int i = 0; i < _count; i++)
    {
        if (_points[i] != src._points[i])
            return false;
    }

    return __super::_equals(src);
}

bool MgBaseLines::_isKindOf(int type) const
{
    return type == Type() || __super::_isKindOf(type);
}

void MgBaseLines::_update()
{
    _extent.set(_count, _points);
    if (_extent.isEmpty() && _points)
        _extent.set(_points[0], 2 * Tol::gTol().equalPoint(), 0);
    __super::_update();
}

void MgBaseLines::_transform(const Matrix2d& mat)
{
    for (int i = 0; i < _count; i++)
        _points[i] *= mat;
    __super::_transform(mat);
}

void MgBaseLines::_clear()
{
    _count = 0;
    __super::_clear();
}

Point2d MgBaseLines::endPoint() const
{
    return _count > 0 ? _points[_count - 1] : Point2d();
}

bool MgBaseLines::resize(int count)
{
    if (_maxCount < count)
    {
        _maxCount = (count + 7) / 8 * 8;

        Point2d* pts = new Point2d[_maxCount];

        for (int i = 0; i < _count; i++)
            pts[i] = _points[i];
        if (_points)
            delete[] _points;
        _points = pts;
    }
    _count = count;
    return true;
}

int MgBaseLines::maxEdgeIndex() const
{
    return _count - (isClosed() ? 1 : 2);
}

bool MgBaseLines::addPoint(const Point2d& pt)
{
    resize(_count + 1);
    _points[_count - 1] = pt;
    return true;
}

bool MgBaseLines::insertPoint(int segment, const Point2d& pt)
{
    bool ret = false;
    
    if (segment >= 0 && segment <= maxEdgeIndex()) {
        resize(_count + 1);
        for (int i = (int)_count - 1; i > segment + 1; i--)
            _points[i] = _points[i - 1];
        _points[segment + 1] = pt;
        ret = true;
    }
    
    return ret;
}

bool MgBaseLines::removePoint(int index)
{
    bool ret = false;
    
    if (index < _count && _count > 1)
    {
        for (int i = index + 1; i < _count; i++)
            _points[i - 1] = _points[i];
        _count--;
        ret = true;
    }
    
    return ret;
}

bool MgBaseLines::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    int preindex = (isClosed() && 0 == index) ? _count - 1 : index - 1;
    int postindex = (isClosed() && index + 1 == _count) ? 0 : index + 1;
    
    float predist = preindex < 0 ? _FLT_MAX : getPoint(preindex).distanceTo(pt);
    float postdist = postindex >= _count ? _FLT_MAX : getPoint(postindex).distanceTo(pt);
    
    if (predist < tol || postdist < tol) {
        removePoint(index);
    }
    else if (!isClosed() && ((index == 0 && getPoint(_count - 1).distanceTo(pt) < tol)
             || (index == _count - 1 && getPoint(0).distanceTo(pt) < tol))) {
        removePoint(index);
        setClosed(true);
    }
    else {
        setPoint(index, pt);
    }
    update();
    
    return true;
}

float MgBaseLines::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    return mgnear::linesHit(_count, _points, isClosed(), pt, tol, 
                            res.nearpt, res.segment, &res.inside);
}

bool MgBaseLines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i + 1 < _count; i++) {
        if (Box2d(_points[i], _points[i + 1]).isIntersect(rect))
            return true;
    }
    
    return _count < 2;
}

bool MgBaseLines::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeUInt("count", _count);
    s->writeFloatArray("points", (const float*)_points, _count * 2);
    return ret;
}

bool MgBaseLines::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    
    int n = s->readInt("count", 0);
    if (n < 1 || n > 9999)
        return s->setError(n < 1 ? "No point." : "Too many points.");
    
    resize(n);
    n = s->readFloatArray("points", (float*)_points, _count * 2);
    
    return (n == _count * 2) && ret;
}

// MgLines
//

MG_IMPLEMENT_CREATE(MgLines)

MgLines::MgLines()
{
}

MgLines::~MgLines()
{
}

bool MgLines::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    bool ret = false;
    if (isClosed())
        ret = gs.drawPolygon(&ctx, _count, _points);
    else
        ret = gs.drawLines(&ctx, _count, _points);
    return __super::_draw(mode, gs, ctx, segment) || ret;
}

int MgLines::_getHandleCount() const
{
    return isClosed() ? 2 * _count : 2 * _count - 1;
}

Point2d MgLines::_getHandlePoint(int index) const
{
    return (index < _count ? __super::_getHandlePoint(index)
            : (_points[index % _count] + _points[(index + 1) % _count]) / 2);
}

int MgLines::_getHandleType(int index) const
{
    return index < _count ? __super::_getHandleType(index) : kMgHandleMidPoint;
}

bool MgLines::_isHandleFixed(int index) const
{
    return index >= _count;
}
