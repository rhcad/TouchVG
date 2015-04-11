// mgellipse.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgellipse.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgEllipse)

MgEllipse::MgEllipse()
{
}

MgEllipse::~MgEllipse()
{
}

float MgEllipse::getRadiusX() const
{
    return getWidth() / 2;
}

float MgEllipse::getRadiusY() const
{
    return getHeight() / 2;
}

void MgEllipse::setRadius(float rx, float ry)
{
    rx = fabsf(rx);
    ry = fabsf(ry);
    if (ry < _MGZERO)
        ry = rx;

    Box2d rect(getCenter(), rx * 2, ry * 2);
    setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), getCenter());
}

bool MgEllipse::setCircle(const Point2d& center, float radius)
{
    if (radius < _MGZERO)
        return false;
    Box2d rect(center, radius * 2, 0);
    setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), center);
    return true;
}

bool MgEllipse::setCircle2P(const Point2d& start, const Point2d& end)
{
    return setCircle((start + end) / 2, start.distanceTo(end) / 2);
}

bool MgEllipse::setCircle3P(const Point2d& start, const Point2d& point, const Point2d& end)
{
    Point2d center;
    float radius;
    
    return (mgcurv::arc3P(start, point, end, center, radius)
            && setCircle(center, radius));
}

void MgEllipse::_update()
{
    mgcurv::ellipseToBezier(_bzpts, getCenter(), getWidth() / 2, getHeight() / 2);

    Matrix2d mat(Matrix2d::rotation(getAngle(), getCenter()));
    for (int i = 0; i < 13; i++)
        _bzpts[i] *= mat;

    mgnear::beziersBox(_extent, 13, _bzpts, true);
    __super::_update();
}

void MgEllipse::_transform(const Matrix2d& mat)
{
    __super::_transform(mat);
    _update();
}

int MgEllipse::_getHandleCount() const
{
    return isCircle() ? 5 : 9;
}

Point2d MgEllipse::_getHandlePoint(int index) const
{
    int index2 = isCircle() ? index + 4 : index;
    return (index < _getHandleCount() - 1
            ? MgBaseRect::_getHandlePoint(index2) : getCenter());
}

int MgEllipse::_getHandleType(int index) const
{
    int index2 = isCircle() ? index + 4 : index;
    return (index == _getHandleCount() - 1 ? kMgHandleCenter :
            index2 >= 4 && index2 < 8 ? kMgHandleQuadrant : kMgHandleVertex);
}

bool MgEllipse::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    int index2 = isCircle() ? index + 4 : index;
    return (index < _getHandleCount() - 1
            ? MgBaseRect::_setHandlePoint(index2, pt, tol)
            : offset(pt - getCenter(), -1));
}

float MgEllipse::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    if (isCircle()) {
        Point2d pt1(getCenter()), pt2(pt);
        crossCircle(pt1, pt2, this);
        float d1 = pt.distanceTo(pt1);
        float d2 = pt.distanceTo(pt2);
        res.nearpt = d1 < d2 ? pt1 : pt2;
        return mgMin(d1, d2);
    }
    
    float distMin = _FLT_MAX;
    const Box2d rect (pt, 2 * tol, 2 * tol);
    Point2d ptTemp;

    for (int i = 0; i < 4; i++) {
        if (rect.isIntersect(Box2d(4, _bzpts + 3 * i))) {
            mgnear::nearestOnBezier(pt, _bzpts + 3 * i, ptTemp);
            float dist = pt.distanceTo(ptTemp);
            if (dist <= tol && dist < distMin) {
                distMin = dist;
                res.nearpt = ptTemp;
                res.segment = i;
            }
        }
    }

    return distMin;
}

bool MgEllipse::_hitTestBox(const Box2d& rect) const
{
    if (!getExtent().isIntersect(rect))
        return false;
    
    return mgnear::beziersIntersectBox(rect, 13, _bzpts, true);
}

void MgEllipse::_output(MgPath& path) const
{
    path.moveTo(_bzpts[0]);
    path.beziersTo(12, _bzpts + 1);
    path.closeFigure();
}
