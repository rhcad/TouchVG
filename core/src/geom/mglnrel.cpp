// mglnrel.cpp: 实现直线位置关系函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mglnrel.h"

bool mglnrel::isLeft(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return (b-a).crossProduct(pt-a) > 0.f;
}

bool mglnrel::isLeft2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    float dist = (b-a).distanceToVector(pt-a);
    return dist > tol.equalPoint();
}

bool mglnrel::isLeftOn(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return (b-a).crossProduct(pt-a) >= 0.f;
}

bool mglnrel::isLeftOn2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    float dist = (b-a).distanceToVector(pt-a);
    return dist > -tol.equalPoint();
}

bool mglnrel::isColinear(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return mgIsZero((b-a).crossProduct(pt-a));
}

bool mglnrel::isColinear2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    float dist = (b-a).crossProduct(pt-a);
    return fabsf(dist) < tol.equalPoint();
}

bool mglnrel::isIntersectProp(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d)
{
    if (mglnrel::isColinear(a,b,c) || mglnrel::isColinear(a,b,d)
        || mglnrel::isColinear(c,d,a) || mglnrel::isColinear(c,d,b))
        return false;
    
    return (mglnrel::isLeft(a,b,c) ^ mglnrel::isLeft(a,b,d))
        && (mglnrel::isLeft(c,d,a) ^ mglnrel::isLeft(c,d,b));
}

bool mglnrel::isBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    if (!mglnrel::isColinear(a, b, pt))
        return false;
    
    if (a.x != b.x)
        return (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
    else
        return (a.y <= pt.y && pt.y <= b.y) || (a.y >= pt.y && pt.y >= b.y);
}

bool mglnrel::isProjectBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    float proj = (pt - a).projectScaleToVector(b - a);
    return proj >= 0 && proj <= 1;
}

bool mglnrel::isProjectBetweenRayline(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    float proj = (pt - a).projectScaleToVector(b - a);
    return proj >= 0;
}

bool mglnrel::isBetweenLine2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    if (!mglnrel::isColinear2(a, b, pt, tol))
        return false;
    
    if (a.x != b.x) {
        return ((a.x <= pt.x + tol.equalPoint()) && (pt.x <= b.x + tol.equalPoint()))
            || ((a.x >= pt.x - tol.equalPoint()) && (pt.x >= b.x - tol.equalPoint()));
    }
    else {
        return ((a.y <= pt.y + tol.equalPoint()) && (pt.y <= b.y + tol.equalPoint()))
            || ((a.y >= pt.y - tol.equalPoint()) && (pt.y >= b.y - tol.equalPoint()));
    }
}

bool mglnrel::isBetweenLine3(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d* nearpt)
{
    bool ret;
    if (a.x != b.x) {
        ret = (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
        if (nearpt) {
            *nearpt = fabsf(pt.x - a.x) < fabsf(pt.x - b.x) ? a : b;
        }
    }
    else {
        ret = (a.y <= pt.y && pt.y <= b.y) || (a.y >= pt.y && pt.y >= b.y);
        if (nearpt) {
            *nearpt = fabsf(pt.y - a.y) < fabsf(pt.y - b.y) ? a : b;
        }
    }
    return ret;
}

bool mglnrel::isIntersect(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d)
{
    if (mglnrel::isIntersectProp(a, b, c, d))
        return true;
    else if (mglnrel::isBetweenLine(a, b, c) || mglnrel::isBetweenLine(a, b, d) 
        || mglnrel::isBetweenLine(c, d, a) || mglnrel::isBetweenLine(c, d, b))
        return true;
    else
        return false;
}

float mglnrel::ptToBeeline(const Point2d& a, const Point2d& b, const Point2d& pt)
{
    return (b-a).crossProduct(pt-a);
}

float mglnrel::ptToBeeline2(
    const Point2d& a, const Point2d& b, Point2d pt, Point2d& ptPerp)
{
    if (a == b) {
        ptPerp = a;
        return a.distanceTo(pt);
    }
    else if (mgEquals(a.x, b.x)) {
        ptPerp.set(a.x, pt.y);
        return fabsf(a.x - pt.x);
    }
    else if (mgEquals(a.y, b.y)) {
        ptPerp.set(pt.x, a.y);
        return fabsf(a.y - pt.y);
    }
    else {
        float t1 = ( b.y - a.y ) / ( b.x - a.x );
        float t2 = -1.f / t1;
        ptPerp.x = ( pt.y - a.y + a.x * t1 - pt.x * t2 ) / ( t1 - t2 );
        ptPerp.y = a.y + (ptPerp.x - a.x) * t1;
        return pt.distanceTo(ptPerp);
    }
}

float mglnrel::ptToLine(
    const Point2d& a, const Point2d& b, Point2d pt, Point2d& nearpt)
{
    Point2d ptTemp;
    float dist = mglnrel::ptToBeeline2(a, b, pt, nearpt);
    if (!mglnrel::isBetweenLine3(a, b, nearpt, &ptTemp)) {
        dist = pt.distanceTo(ptTemp);
        nearpt = ptTemp;
    }
    return dist;
}

bool mglnrel::crossLineAbc(
    float a1, float b1, float c1, float a2, float b2, float c2,
    Point2d& ptCross, const Tol& tolVec)
{
    float sinnum, cosnum;
    
    sinnum = a1*b2 - a2*b1;
    if (mgIsZero(sinnum))
        return false;
    cosnum = a1*a2 + b1*b2;
    if (!mgIsZero(cosnum) && fabsf(sinnum / cosnum) < tolVec.equalVector())
        return false;
    
    ptCross.x = (b1*c2 - b2*c1) / sinnum;
    ptCross.y = (a2*c1 - a1*c2) / sinnum;
    
    return true;
}

bool mglnrel::cross2Beeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, 
    Point2d& ptCross, float* pu, float* pv, const Tol& tolVec)
{
    float u, v, denom, cosnum;
    
    denom = (c.x - d.x) * (b.y - a.y) - (c.y - d.y) * (b.x - a.x);
    if (mgIsZero(denom))            // 平行或重合
        return false;
    
    cosnum = (b.x - a.x) * (d.x - c.x) + (b.y - a.y) * (d.y - c.y);
    if (!mgIsZero(cosnum) && fabsf(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x) * (d.y-c.y) - (c.y-a.y) * (d.x-c.x)) / denom;
    v = ((c.x-a.x) * (b.y-a.y) - (c.y-a.y) * (b.x-a.x)) / denom;
    
    ptCross = (1 - u) * a + u * b;
    if (pu) *pu = u;
    if (pv) *pv = v;
    
    return true;
}

static const float LINE_LIMIT = 1e5f - 1.f;

bool mglnrel::cross2Line(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, const Tol& tolVec)
{
    Box2d box(-LINE_LIMIT, -LINE_LIMIT, LINE_LIMIT, LINE_LIMIT);
    Box2d clipbox(box * 0.5f);
    
    if (!box.contains(a) || !box.contains(b)) {
        Point2d a2(a), b2(b);
        if (!mglnrel::clipLine(a2, b2, clipbox))
            return false;
        return cross2Line(a2, b2, c, d, ptCross, tolVec);
    }
    if (!box.contains(c) || !box.contains(d)) {
        Point2d c2(c), d2(d);
        if (!mglnrel::clipLine(c2, d2, clipbox))
            return false;
        return cross2Line(a, b, c2, d2, ptCross, tolVec);
    }
    
    float u, v, denom, cosnum;
    
    if (mgMin(a.x,b.x) - mgMax(c.x,d.x) > _MGZERO 
        || mgMin(c.x,d.x) - mgMax(a.x,b.x) > _MGZERO
        || mgMin(a.y,b.y) - mgMax(c.y,d.y) > _MGZERO 
        || mgMin(c.y,d.y) - mgMax(a.y,b.y) > _MGZERO) {
        return false;
    }
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom)) {
        return false;
    }
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabsf(denom / cosnum) < tolVec.equalVector()) {
        return false;
    }
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    if (u < _MGZERO || u > 1.f - _MGZERO) {
        return false;
    }
    
    v = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    if (v < _MGZERO || v > 1.f - _MGZERO) {
        return false;
    }
    
    ptCross = (1 - u) * a + u * b;
    
    return true;
}

bool mglnrel::cross2LineV(const Point2d& a, const Point2d& b, const Point2d& c,
                          const Point2d& d, float* v1, float* v2)
{
    float denom = (b.x - a.x) * (d.y - c.y) - (b.y - a.y) * (d.x - c.x);
    
    if (mgIsZero(denom)) {
        return false;
    }
    
    float u = (a.y - c.y) * (d.x - c.x) - (a.x - c.x) * (d.y - c.y);
    u /= denom;
    
    float v = (a.y - c.y) * (b.x - a.x) - (a.x - c.x) * (b.y - a.y);
    v /= denom;
    
    if (v1) {
        *v1 = u;
    }
    if (v2) {
        *v2 = v;
    }
    
    return !(u < 0 || u > 1 || v < 0 || v > 1);
}

bool mglnrel::crossLineBeeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, float* pv, const Tol& tolVec)
{
    float u, denom, cosnum;
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabsf(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    if (u < _MGZERO || u > 1.f - _MGZERO)
        return false;
    
    if (pv) {
        *pv = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    }
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
}

enum { kOutUp = 1 << 1, kOutBottom = 1 << 2, kOutLeft = 1 << 3, kOutRight = 1 << 4 };
static inline unsigned clipValue(Point2d& pt, const Box2d& box)
{
    unsigned ret = 0;
    if (pt.y > box.ymax)
        ret |= kOutUp;
    else if (pt.y < box.ymin)
        ret |= kOutBottom;
    if (pt.x < box.xmin)
        ret |= kOutLeft;
    else if (pt.x > box.xmax)
        ret |= kOutRight;
    return ret;
}

bool mglnrel::clipLine(Point2d& pt1, Point2d& pt2, const Box2d& _box)
{
    Box2d box (_box);
    box.normalize();
    
    unsigned v1, v2;
    v1 = clipValue(pt1, box);
    v2 = clipValue(pt2, box);
    
    for ( ; ; ) {
        if (!v1 && !v2)
            return true;
        if (v1 & v2)
            return false;
        
        double x = 0, y = 0, t;
        unsigned v;
        
        if (v1)
            v = v1;
        else
            v = v2;
        if (v & kOutUp) {
            t = pt1.y;
            x = pt1.x + ((double)pt2.x - pt1.x) * (box.ymax - t) / (pt2.y - t);
            y = box.ymax;
        }
        else if (v & kOutBottom) {
            t = pt1.y;
            x = pt1.x + ((double)pt2.x - pt1.x) * (box.ymin - t) / (pt2.y - t);
            y = box.ymin;
        }
        else if (v & kOutLeft) {
            t = pt1.x;
            y = pt1.y + ((double)pt2.y - pt1.y) * (box.xmin - t) / (pt2.x - t);
            x = box.xmin;
        }
        else if (v & kOutRight) {
            t = pt1.x;
            y = pt1.y + ((double)pt2.y - pt1.y) * (box.xmax - t) / (pt2.x - t);
            x = box.xmax;
        }
        
        if (v == v1) {
            pt1.set((float)x, (float)y);
            v1 = clipValue(pt1, box);
        } else {
            pt2.set((float)x, (float)y);
            v2 = clipValue(pt2, box);
        }
    }
}

static bool checkEdge(int &isodd, const Point2d& pt, const Point2d& p1,
                      const Point2d& p2, const Point2d& p0)
{
    if (!((p2.x > p1.x) && (pt.x >= p1.x) && (pt.x < p2.x)) &&
        !((p1.x > p2.x) && (pt.x <= p1.x) && (pt.x > p2.x)) ) {
        return false;
    }
    
    if (pt.y > p1.y + (pt.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x)) {
        if (mgEquals(pt.x, p1.x)) {
            if (((p0.x > pt.x) && (p2.x > pt.x)) ||
                ((p0.x < pt.x) && (p2.x < pt.x)) ) {
                return false;
            }
        }
        isodd = 1 - isodd;
    }
    
    return true;
}

int mglnrel::ptInArea(
    const Point2d& pt, int count, const Point2d* pts, 
    int& order, const Tol& tol, bool closed, int flags, int ignoreVertex)
{
    int i;
    int odd = 1;
    float minDist = tol.equalPoint();
    Point2d nearpt;
    
    if (flags & (1 << kPtAtVertex)) {
        order = -1;
        for (i = 0; i < count && tol.equalPoint() < 1.e5f; i++) {
            if (i == ignoreVertex) {
                continue;
            }
            float d = pt.distanceTo(pts[i]);
            if (minDist > d) {
                minDist = d;
                order = i;
            }
        }
        if (order >= 0) {
            return kPtAtVertex;
        }
    }
    
    if (flags & (1 << kPtOnEdge)) {
        order = -1;
        minDist = tol.equalPoint();
        
        for (i = 0; i < (closed ? count : count - 1); i++) {
            int ei = i+1 < count ? i+1 : 0;
            if (i == ignoreVertex || ei == ignoreVertex) {
                continue;
            }
            
            const Point2d& p1 = pts[i];
            const Point2d& p2 = pts[ei];
            
            float d = mglnrel::ptToBeeline2(p1, p2, pt, nearpt);
            if (minDist > d) {
                minDist = d;
                order = i;
            }
            else if (!checkEdge(odd, pt, p1, p2, i > 0 ? pts[i-1] : pts[count-1])) {
                continue;
            }
        }
        if (order >= 0) {
            return kPtOnEdge;
        }
    }

    return 0 == odd && (flags & (1 << kPtInArea)) ? kPtInArea : kPtOutArea;
}

bool mglnrel::isConvex(int count, const Point2d* vs, bool* acw)
{
    if (count < 3 || !vs) {
        return true;
    }
    bool z = (  (vs[count-1].x - vs[count-2].x) * (vs[1].y - vs[0].y)
              > (vs[count-1].y - vs[count-2].y) * (vs[1].x - vs[0].x) );
    for (int i = 0; i < count; i++) {
        if (z != (  (vs[i].x - vs[i-1].x) * (vs[i+1].y - vs[i].y)
                  > (vs[i].y - vs[i-1].y) * (vs[i+1].x - vs[i].x) )) {
            return false;
        }
    }
    if (acw) {
        *acw = z;
    }
    return true;
}
