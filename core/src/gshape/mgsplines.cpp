// mgsplines.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgsplines.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgSplines)

MgSplines::MgSplines() : _knotvs((Vector2d*)0)
{
}

MgSplines::~MgSplines()
{
    delete[] _knotvs;
}

float MgSplines::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    if (_count == 2) {
        return mglnrel::ptToLine(_points[0], _points[1], pt, res.nearpt);
    }
    if (_knotvs) {
        return mgnear::cubicSplinesHit(_count, _points, _knotvs, isClosed(),
                                       pt, tol, res.nearpt, res.segment, false);
    }
    return mgnear::quadSplinesHit(_count, _points, isClosed(),
                                  pt, tol, res.nearpt, res.segment);
}

bool MgSplines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    if (_knotvs) {
        return mgnear::cubicSplinesIntersectBox(rect, _count, _points, _knotvs, isClosed(), false);
    }
    return true;
}

void MgSplines::_output(MgPath& path) const
{
    if (_count < 2) {
    }
    else if (_count == 2) {
        path.moveTo(_points[0]);
        path.lineTo(_points[1]);
    }
    else if (_knotvs) {
        path.moveTo(_points[0]);
        for (int i = 1; i + 1 < _count; i++) {
            path.bezierTo(_points[i] + _knotvs[i],
                          _points[i+1] - _knotvs[i+1], _points[i+1]);
        }
        if (isClosed()) {
            path.closeFigure();
        }
    }
    else {
        Point2d mid;
        
        for (int i = 0; i < (isClosed() ? _count : _count - 2); i++) {
            if (i == 0) {
                path.moveTo(isClosed() ? (_points[0] + _points[1]) / 2 : _points[0]);
            }
            if (isClosed() || i + 3 < _count)
                mid = (_points[(i+1) % _count] + _points[(i+2) % _count]) / 2;
            else
                mid = _points[i+2];
            path.quadTo(_points[(i+1) % _count], mid);
        }
        if (isClosed()) {
            path.closeFigure();
        }
    }
}

void MgSplines::_copy(const MgSplines& src)
{
    __super::_copy(src);    // will clear _knotvs via MgSplines::resize
    if (src._knotvs) {
        _knotvs = new Vector2d[_count];
        for (int i = 0; i < _count; i++)
            _knotvs[i] = src._knotvs[i];
    }
}

bool MgSplines::_equals(const MgSplines& src) const
{
    if (!__super::_equals(src) || !_knotvs != !src._knotvs)
        return false;
    
    if (_knotvs) {
        for (int i = 0; i < _count; i++) {
            if (_knotvs[i] != src._knotvs[i])
                return false;
        }
    }
    
    return true;
}

void MgSplines::_transform(const Matrix2d& mat)
{
    if (_knotvs) {
        for (int i = 0; i < _count; i++)
            _knotvs[i] *= mat;
    }
    __super::_transform(mat);
}

void MgSplines::_clear()
{
    clearVectors();
    __super::_clear();
}

void MgSplines::_setPoint(int index, const Point2d& pt)
{
    clearVectors();
    __super::_setPoint(index, pt);
}

void MgSplines::clearVectors()
{
    if (_knotvs) {
        delete[] _knotvs;
        _knotvs = (Vector2d*)0;
    }
}

bool MgSplines::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    if (_knotvs) {
        s->writeFloatArray("vec", (const float*)_knotvs, _count * 2);
    }
    return ret;
}

bool MgSplines::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    if (ret && _count > 0 && s->readFloatArray("vec") > 0) {
        _knotvs = new Vector2d[_count];
        if (s->readFloatArray("vec", (float*)_knotvs, _count * 2) != _count * 2) {
            ret = false;
        }
    }
    return ret;
}

bool MgSplines::resize(int count)
{
    if (count != _count)
        clearVectors();
    return __super::resize(count);
}

bool MgSplines::addPoint(const Point2d& pt)
{
    clearVectors();
    return __super::addPoint(pt);
}

bool MgSplines::insertPoint(int segment, const Point2d& pt)
{
    clearVectors();
    return __super::insertPoint(segment, pt);
}

bool MgSplines::removePoint(int index)
{
    clearVectors();
    return __super::removePoint(index);
}

bool MgSplines::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    int preindex = (isClosed() && 0 == index) ? _count - 1 : index - 1;
    int postindex = (isClosed() && index + 1 == _count) ? 0 : index + 1;
    
    float predist = preindex < 0 ? _FLT_MAX : getPoint(preindex).distanceTo(pt);
    float postdist = postindex >= _count ? _FLT_MAX : getPoint(postindex).distanceTo(pt);
    
    if (predist < tol || postdist < tol) {
        if (_count <= 3)
            return false;
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

bool MgSplines::smooth(const Matrix2d& m2d, float tol)
{
    return smoothForPoints(_count, _points, m2d, tol) > 0;
}

int MgSplines::smoothForPoints(int count, const Point2d* points, const Matrix2d& m2d, float tol)
{
    if (count < 3 || !points || tol < _MGZERO)
        return 0;
    
    int i, knotCount = count + 1;
    Point2d* ptx = new Point2d[count];
    Point2d* knots = new Point2d[knotCount];
    Vector2d* knotvs = new Vector2d[knotCount];
    Matrix2d d2m(m2d.inverse());
    
    for (i = 0; i < count; i++)
        ptx[i] = points[i] * m2d;
    
    _count = mgcurv::fitCurve(knotCount, knots, knotvs, count, ptx, tol);
    _maxCount = knotCount;
    
    for (i = 0; i < _count; i++) {
        knots[i] *= d2m;
        knotvs[i] *= d2m;
    }
    delete[] ptx;
    delete[] _points;
    _points = knots;
    delete[] _knotvs;
    _knotvs = knotvs;
    update();
    
    return _count;
}
