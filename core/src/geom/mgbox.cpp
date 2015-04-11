// mgbox.cpp: 实现矩形框类 Box2d
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgbox.h"
#include "mgmat.h"

Box2d::Box2d(const Box2d& src, bool bNormalize)
{
    xmin = src.xmin;
    ymin = src.ymin;
    xmax = src.xmax;
    ymax = src.ymax;
    if (bNormalize)
        normalize();
}

Box2d& Box2d::set(const Box2d& src, bool bNormalize)
{
    xmin = src.xmin;
    ymin = src.ymin;
    xmax = src.xmax;
    ymax = src.ymax;
    return bNormalize ? normalize() : *this;
}

Box2d& Box2d::set(const Point2d& p1, const Point2d& p2)
{
    xmin = mgMin(p1.x, p2.x);
    ymin = mgMin(p1.y, p2.y);
    xmax = mgMax(p1.x, p2.x);
    ymax = mgMax(p1.y, p2.y);
    return *this;
}

Box2d& Box2d::set(float x1, float y1, float x2, float y2)
{
    xmin = mgMin(x1, x2);
    ymin = mgMin(y1, y2);
    xmax = mgMax(x1, x2);
    ymax = mgMax(y1, y2);
    return *this;
}

Box2d& Box2d::set(const Point2d& p1, const Point2d& p2, 
                  const Point2d& p3, const Point2d& p4)
{
    xmin = mgMin(mgMin(p1.x, p2.x), mgMin(p3.x, p4.x));
    ymin = mgMin(mgMin(p1.y, p2.y), mgMin(p3.y, p4.y));
    xmax = mgMax(mgMax(p1.x, p2.x), mgMax(p3.x, p4.x));
    ymax = mgMax(mgMax(p1.y, p2.y), mgMax(p3.y, p4.y));
    return *this;
}

Box2d& Box2d::set(int count, const Point2d* points)
{
    if (count < 1 || !points)
        return empty();

    set(points[0], points[0]);
    for (int i = 0; i < count; i++)
    {
        if (xmin > points[i].x)
            xmin = points[i].x;
        if (ymin > points[i].y)
            ymin = points[i].y;
        if (xmax < points[i].x)
            xmax = points[i].x;
        if (ymax < points[i].y)
            ymax = points[i].y;
    }

    return *this;
}

bool Box2d::isIntersect(const Box2d& box) const
{
    if (xmax - xmin < -_MGZERO || ymax - ymin < -_MGZERO || isNull())
        return false;
    if (box.xmax - box.xmin < -_MGZERO
        || box.ymax - box.ymin < -_MGZERO || box.isNull())
        return false;

    if (mgMin(xmax, box.xmax) < mgMax(xmin, box.xmin))
        return false;
    if (mgMin(ymax, box.ymax) < mgMax(ymin, box.ymin))
        return false;

    return true;
}

Box2d& Box2d::intersectWith(const Box2d& r1, const Box2d& r2)
{
    if (r1.isEmptyMinus())
        return empty();
    if(r2.isEmptyMinus())
        return empty();

    float l = mgMax(r1.xmin, r2.xmin);
    float b = mgMax(r1.ymin, r2.ymin);
    float r = mgMin(r1.xmax, r2.xmax);
    float t = mgMin(r1.ymax, r2.ymax);
    if (r < l || t < b)
        return empty();
    else
        return set(l, b, r, t);
}

Box2d& Box2d::unionWith(const Box2d& r1, const Box2d& r2)
{
    if (r2.isEmpty(Tol::gTol(), false))
        return set(r1, true);
    if (r1.isEmpty(Tol::gTol(), false))
        return set(r2, true);
    return set(
        mgMin(r1.xmin, r2.xmin),
        mgMin(r1.ymin, r2.ymin),
        mgMax(r1.xmax, r2.xmax),
        mgMax(r1.ymax, r2.ymax));
}

Box2d Box2d::operator*(const Matrix2d& m) const
{
    if (m.isOrtho())
        return Box2d(leftBottom() * m, rightTop() * m);
    return Box2d(leftBottom() * m, rightTop() * m,
        leftTop() * m, rightBottom() * m);
}

Box2d& Box2d::operator*=(const Matrix2d& m)
{
    if (m.isOrtho())
        return set(leftBottom() * m, rightTop() * m);
    return set(leftBottom() * m, rightTop() * m,
        leftTop() * m, rightBottom() * m);
}
