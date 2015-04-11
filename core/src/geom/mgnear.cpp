// mgnear.cpp: 实现曲线最近点计算函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgnear.h"
#include "mgcurv.h"
#include "mglnrel.h"

Box2d mgnear::bezierBox1(const Point2d points[4])
{
    return bezierBox4(points[0], points[1], points[2], points[3]);
}

void mgnear::beziersBox(
    Box2d& box, int count, const Point2d* points, bool closed)
{
    box.empty();

    for (int i = 0; i + 3 < count; i += 3) {
        box.unionWith(bezierBox1(points + i));
    }
    if (closed && count > 3) {
        box.unionWith(bezierBox4(points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(), points[0]));
    }
}

bool mgnear::beziersIntersectBox(
    const Box2d& box, int count, const Point2d* points, bool closed)
{
    for (int i = 0; i + 3 < count; i += 3) {
        if (box.isIntersect(bezierBox1(points + i))) {
            return true;
        }
    }
    if (closed && count > 3) {
        if (box.isIntersect(bezierBox4(points[count - 1],
            points[count - 1] * 2 - points[count - 2].asVector(),
            points[0] * 2 - points[1].asVector(), points[0])))
        {
            return true;
        }
    }
    
    return false;
}

void mgnear::cubicSplinesBox(
    Box2d& box, int n, const Point2d* knots, 
    const Vector2d* knotvs, bool closed, bool hermite)
{
    int n2 = (closed && n > 1) ? n + 1 : n;
    float d = hermite ? 1.f/3.f : 1.f;

    box.empty();
    for (int i = 0; i + 1 < n2; i++)
    {
        box.unionWith(bezierBox4(knots[i], 
            knots[i] + knotvs[i] * d,
            knots[(i + 1) % n] - knotvs[(i + 1) % n] * d,
            knots[(i + 1) % n]));
    }
}

bool mgnear::cubicSplinesIntersectBox(
    const Box2d& box, int n, const Point2d* knots, 
    const Vector2d* knotvs, bool closed, bool hermite)
{
    int n2 = (closed && n > 1) ? n + 1 : n;
    float d = hermite ? 1.f/3.f : 1.f;
    
    for (int i = 0; i + 1 < n2; i++)
    {
        Point2d pts[4] = { knots[i], 
            knots[i] + knotvs[i] * d,
            knots[(i + 1) % n] - knotvs[(i + 1) % n] * d,
            knots[(i + 1) % n] };
        if (mgnear::beziersIntersectBox(box, 4, pts, false))
            return true;
    }
    
    return false;
}

float mgnear::cubicSplinesHit(
    int n, const Point2d* knots, const Vector2d* knotvs, bool closed, 
    const Point2d& pt, float tol, Point2d& nearpt, int& segment, bool hermite)
{
    Point2d ptTemp;
    float dist, distMin = _FLT_MAX;
    const Box2d rect (pt, 2 * tol, 2 * tol);

    segment = -1;
    if (knotvs) {
        Point2d pts[4];
        int n2 = (closed && n > 1) ? n + 1 : n;
        
        for (int i = 0; i + 1 < n2; i++) {
            mgcurv::cubicSplineToBezier(n, knots, knotvs, i, pts, hermite);
            if (rect.isIntersect(bezierBox1(pts))) {
                dist = mgnear::nearestOnBezier(pt, pts, ptTemp);
                if (dist < distMin) {
                    distMin = dist;
                    nearpt = ptTemp;
                    segment = i;
                }
            }
        }
    } else {
        for (int i = 0; i + 3 < n; i += 3) {
            if (rect.isIntersect(bezierBox1(knots + i))) {
                dist = mgnear::nearestOnBezier(pt, knots + i, ptTemp);
                if (dist < distMin) {
                    distMin = dist;
                    nearpt = ptTemp;
                    segment = i;
                }
            }
        }
    }

    return distMin;
}

float mgnear::quadSplinesHit(int n, const Point2d* knots, bool closed,
                             const Point2d& pt, float tol, Point2d& nearpt, int& segment)
{
    Point2d ptTemp;
    float dist, distMin = _FLT_MAX;
    Point2d pts[3 + 4];
    const Box2d rect (pt, 2 * tol, 2 * tol);
    
    segment = -1;
    for (int i = 0; i < (closed ? n : n - 2); i++, pts[0] = pts[2]) {
        if (i == 0) {
            pts[0] = closed ? (knots[0] + knots[1]) / 2 : knots[0];
        }
        pts[1] = knots[(i+1) % n];
        if (closed || i + 3 < n)
            pts[2] = (knots[(i+1) % n] + knots[(i+2) % n]) / 2;
        else
            pts[2] = knots[i+2];
        
        mgcurv::quadBezierToCubic(pts, pts + 3);
        if (rect.isIntersect(bezierBox1(pts + 3))) {
            dist = mgnear::nearestOnBezier(pt, pts + 3, ptTemp);
            if (dist < distMin) {
                distMin = dist;
                nearpt = ptTemp;
                segment = i;
            }
        }
    }
    
    return distMin;
}

int mgcurv::bsplinesToBeziers(
    Point2d points[/*1+n*3*/], int n, const Point2d* ctlpts, bool closed)
{
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    int i = 0;
        
    pt1 = ctlpts[0];
    pt2 = ctlpts[1];
    pt3 = ctlpts[2];
    pt4 = ctlpts[3 % n];
    points[i++].set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    points[i++].set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
    points[i++].set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
    points[i++].set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    
    for (int ci = 4; ci < (closed ? n + 3 : n); ci++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[ci % n];
        points[i++].set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
        points[i++].set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
        points[i++].set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    }
    
    return i;
}

float mgnear::linesHit(
    int n, const Point2d* points, bool closed, 
    const Point2d& pt, float tol, Point2d& nearpt, int& segment,
    bool* inside, int* hitType, int flags, int ignoreVertex)
{
    Point2d ptTemp;
    float dist, distMin = _FLT_MAX;
    const Box2d rect (pt, 2 * tol, 2 * tol);
    int n2 = (closed && n > 1) ? n + 1 : n;
    
    int type = mglnrel::ptInArea(pt, n, points, segment, Tol(tol), closed, flags, ignoreVertex);
    
    if (inside) {
        *inside = (closed && type == mglnrel::kPtInArea);
    }
    if (hitType) {
        *hitType = type;
    }
    if (type == mglnrel::kPtAtVertex) {
        nearpt = points[segment];
        distMin = nearpt.distanceTo(pt);
        return distMin;
    }
    if (type == mglnrel::kPtOnEdge) {
        distMin = mglnrel::ptToLine(points[segment], points[(segment+1)%n], pt, nearpt);
        return distMin;
    }
    if (!closed || type != mglnrel::kPtInArea) {
        return distMin;
    }
    
    for (int i = 0; i + 1 < n2; i++) {
        const Point2d& pt2 = points[(i + 1) % n];
        if (closed || rect.isIntersect(Box2d(points[i], pt2))) {
            dist = mglnrel::ptToLine(points[i], pt2, pt, ptTemp);
            if (distMin > 1e10f || (dist <= tol && dist < distMin)) {
                distMin = dist;
                nearpt = ptTemp;
                if (dist <= tol)
                    segment = i;
            }
        }
    }

    return distMin;
}

static inline
Point2d RoundRectTan(int nFrom, int nTo, const Box2d& rect, float r)
{
    Point2d pt1, pt2;
    mgnear::getRectHandle(rect, nFrom, pt1);
    mgnear::getRectHandle(rect, nTo, pt2);
    return pt1.rulerPoint(pt2, r, 0.f);
}

static void _RoundRectHit(
    const Box2d& rect, float rx, float ry, 
    const Point2d& pt, float tol, const Box2d &rectTol, 
    Point2d* pts, float& distMin, 
    Point2d& nearpt, int& segment)
{
    Point2d ptsBezier[13], ptTemp;
    Vector2d vec;
    float dx = rect.width() * 0.5f - rx;
    float dy = rect.height() * 0.5f - ry;
    
    // 按逆时针方向从第一象限到第四象限连接的四段
    mgcurv::ellipseToBezier(ptsBezier, rect.center(), rx, ry);
    
    pts[3] = ptsBezier[0];
    for (int i = 0; i < 4; i++) {
        pts[0] = pts[3];
        pts[1] = ptsBezier[3 * i];
        pts[2] = ptsBezier[3 * i + 1];
        pts[3] = ptsBezier[3 * i + 2];
        
        switch (i)
        {
        case 0: vec.set(dx, dy); break;
        case 1: vec.set(-dx, dy); break;
        case 2: vec.set(-dx, -dy); break;
        case 3: vec.set(dx, -dy); break;
        }
        
        for (int j = 0; j < 4; j++)
            pts[j] += vec;
        
        if (rectTol.isIntersect(Box2d(4, pts))) {
            mgnear::nearestOnBezier(pt, pts, ptTemp);
            float dist = pt.distanceTo(ptTemp);
            if (dist <= tol && dist < distMin) {
                distMin = dist;
                nearpt = ptTemp;
                segment = (5 - i) % 4;
            }
        }
        
        pts[3] -= vec;
    }
}

float mgnear::roundRectHit(
    const Box2d& rect, float rx, float ry, 
    const Point2d& pt, float tol, Point2d& nearpt, int& segment)
{
    rx = fabsf(rx);
    if (ry < _MGZERO)
        ry = rx;
    rx = mgMin(rx, rect.width() * 0.5f);
    ry = mgMin(ry, rect.height() * 0.5f);
    segment = -1;
    
    Point2d ptTemp, ptTemp2;
    float dist, distMin = _FLT_MAX;
    Point2d pts[8];
    const Box2d rectTol (pt, 2 * tol, 2 * tol);
    
    // 顶边上的两个圆弧切点，左，右
    pts[0] = RoundRectTan(0, 1, rect, rx);
    pts[1] = RoundRectTan(1, 0, rect, rx);
    
    // 右边上的两个圆弧切点，上，下
    pts[2] = RoundRectTan(1, 2, rect, ry);
    pts[3] = RoundRectTan(2, 1, rect, ry);
    
    // 底边上的两个圆弧切点，右，左
    pts[4] = RoundRectTan(2, 3, rect, rx);
    pts[5] = RoundRectTan(3, 2, rect, rx);
    
    // 左边上的两个圆弧切点，下，上
    pts[6] = RoundRectTan(3, 0, rect, ry);
    pts[7] = RoundRectTan(0, 3, rect, ry);
    
    for (int i = 0; i < 4; i++) {
        Box2d rcLine (pts[2 * i], pts[2 * i + 1]);
        if (rcLine.isEmpty() || rectTol.isIntersect(rcLine)) {
            dist = mglnrel::ptToLine(pts[2 * i], pts[2 * i + 1], pt, ptTemp);
            if (dist <= tol && dist < distMin) {
                distMin = dist;
                nearpt = ptTemp;
                segment = 4 + i;
            }
        }
    }
    
    if (rx > _MGZERO && ry > _MGZERO) {
        _RoundRectHit(rect, rx, ry, pt, tol, 
            rectTol, pts, distMin, nearpt, segment);
    }

    return distMin;
}

void mgnear::getRectHandle(const Box2d& rect, int index, Point2d& pt)
{
    switch (index)
    {
    case 0: pt = rect.leftTop(); break;
    case 1: pt = rect.rightTop(); break;
    case 2: pt = rect.rightBottom(); break;
    case 3: pt = rect.leftBottom(); break;
    case 4: pt = Point2d(rect.center().x, rect.ymax); break;
    case 5: pt = Point2d(rect.xmax, rect.center().y); break;
    case 6: pt = Point2d(rect.center().x, rect.ymin); break;
    case 7: pt = Point2d(rect.xmin, rect.center().y); break;
    default: pt = rect.center(); break;
    }
}

void mgnear::moveRectHandle(Box2d& rect, int index, 
                              const Point2d& pt, bool lockCornerScale)
{
    Point2d pts[4];

    for (int i = 0; i < 4; i++)
        mgnear::getRectHandle(rect, index / 4 * 4 + i, pts[i]);
    pts[index % 4] = pt;

    if (index >= 0 && index < 4) {
        Point2d pt1(pt);
        
        if (lockCornerScale && !rect.isEmpty()) {
            Point2d& pt2 = pts[(index + 2) % 4];
            float w = fabsf(pt2.x - pt.x);
            float h = fabsf(pt2.y - pt.y);
            
            if (w * rect.height() > h * rect.width())
                h = w * rect.height() / rect.width();
            else
                w = h * rect.width() / rect.height();
            
            pt1.x = pt2.x + w * (pt2.x > pt.x ? -1.f : 1.f);
            pt1.y = pt2.y + h * (pt2.y > pt.y ? -1.f : 1.f);
        }
        if (index % 2 == 0) {
            pts[(index + 1) % 4].y = pt1.y;
            pts[(index + 3) % 4].x = pt1.x;
        }
        else {
            pts[(index + 1) % 4].x = pt1.x;
            pts[(index + 3) % 4].y = pt1.y;
        }
        rect.set(4, pts);
    }
    else if (index >= 4 && index < 8) {
        rect.set(pts[3].x, pts[2].y, pts[1].x, pts[0].y);
    }
}

// computeCubicBaseValue, computeCubicFirstDerivativeRoots
// © 2011 Mike "Pomax" Kamermans of nihongoresources.com
// http://processingjs.nihongoresources.com/bezierinfo/#bounds

// compute the value for the cubic bezier function at time=t
static float computeCubicBaseValue(float t, float a, float b, float c, float d) {
    float v = 1-t, v2 = v*v, t2 = t*t;
    return v2*v*a + 3*v2*t*b + 3*v*t2*c + t2*t*d;
}

// compute the value for the first derivative of the cubic bezier function at time=t
static Point2d computeCubicFirstDerivativeRoots(float a, float b, float c, float d) {
    Point2d ret (-1, -1);
    float tl = -a+2*b-c;
    float tr = -sqrtf(-a*(c-d) + b*b - b*(c+d) +c*c);
    float dn = -a+3*b-3*c+d;
    if (dn != 0) {
        ret.x = (tl+tr)/dn;
        ret.y = (tl-tr)/dn;
    }
    return ret;
}

// Compute the bounding box based on the straightened curve, for best fit
Box2d mgnear::bezierBox4(const Point2d& pt1, const Point2d& pt2,
                         const Point2d& pt3, const Point2d& pt4)
{
    Box2d bbox(pt1, pt4);
    int flag = 0;

    // find the zero point for x and y in the derivatives
    Point2d ts = computeCubicFirstDerivativeRoots(pt1.x, pt2.x, pt3.x, pt4.x);
    if (ts.x>=0 && ts.x<=1) {
        float x = computeCubicBaseValue(ts.x, pt1.x, pt2.x, pt3.x, pt4.x);
        if (x < bbox.xmin) bbox.xmin = x;
        if (x > bbox.xmax) bbox.xmax = x;
        flag |= 1;
    }
    if (ts.y>=0 && ts.y<=1) {
        float x = computeCubicBaseValue(ts.y, pt1.x, pt2.x, pt3.x, pt4.x);
        if (x < bbox.xmin) bbox.xmin = x;
        if (x > bbox.xmax) bbox.xmax = x;
        flag |= 2;
    }
    ts = computeCubicFirstDerivativeRoots(pt1.y, pt2.y, pt3.y, pt4.y);
    if (ts.x>=0 && ts.x<=1) {
        float y = computeCubicBaseValue(ts.x, pt1.y, pt2.y, pt3.y, pt4.y);
        if (y < bbox.ymin) bbox.ymin = y;
        if (y > bbox.ymax) bbox.ymax = y;
        flag |= 4;
    }
    if (ts.y>=0 && ts.y<=1) {
        float y = computeCubicBaseValue(ts.y, pt1.y, pt2.y, pt3.y, pt4.y);
        if (y < bbox.ymin) bbox.ymin = y;
        if (y > bbox.ymax) bbox.ymax = y;
        flag |= 8;
    }
    if (!flag) {
        bbox.unionWith(pt2);
        bbox.unionWith(pt3);
    }

    return bbox;
}
