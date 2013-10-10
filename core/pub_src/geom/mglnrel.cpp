// mglnrel.cpp: 实现直线位置关系函数
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

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
    // Eliminate improper cases
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
    
    // If ab not vertical, check betweenness on x; else on y.
    if (a.x != b.x) 
        return  (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
    else
        return  (a.y <= pt.y && pt.y <= b.y) || (a.y >= pt.y && pt.y >= b.y);
}

bool mglnrel::isBetweenLine2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol)
{
    if (!mglnrel::isColinear2(a, b, pt, tol))
        return false;
    
    // If ab not vertical, check betweenness on x; else on y.
    if (a.x != b.x) 
    {
        return ((a.x <= pt.x + tol.equalPoint()) 
            && (pt.x <= b.x + tol.equalPoint())) 
            || ((a.x >= pt.x - tol.equalPoint()) 
            && (pt.x >= b.x - tol.equalPoint()));
    }
    else
    {
        return ((a.y <= pt.y + tol.equalPoint()) 
            && (pt.y <= b.y + tol.equalPoint())) 
            || ((a.y >= pt.y - tol.equalPoint()) 
            && (pt.y >= b.y - tol.equalPoint()));
    }
}

bool mglnrel::isBetweenLine3(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d* nearpt)
{
    bool ret;
    if (a.x != b.x) 
    {
        ret = (a.x <= pt.x && pt.x <= b.x) || (a.x >= pt.x && pt.x >= b.x);
        if (nearpt) {
            *nearpt = fabsf(pt.x - a.x) < fabsf(pt.x - b.x) ? a : b;
        }
    }
    else
    {
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
    float dist = (b-a).crossProduct(pt-a);
    return dist;
}

float mglnrel::ptToBeeline2(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& ptPerp)
{
    // 两点重合
    if (a == b)
    {
        ptPerp = a;
        return a.distanceTo(pt);
    }
    // 竖直线
    else if (mgEquals(a.x, b.x))
    {
        ptPerp.set(a.x, pt.y);
        return fabsf(a.x - pt.x);
    }
    // 水平线
    else if (mgEquals(a.y, b.y))
    {
        ptPerp.set(pt.x, a.y);
        return fabsf(a.y - pt.y);
    }
    else
    {
        float t1 = ( b.y - a.y ) / ( b.x - a.x );
        float t2 = -1.f / t1;
        ptPerp.x = ( pt.y - a.y + a.x * t1 - pt.x * t2 ) / ( t1 - t2 );
        ptPerp.y = a.y + (ptPerp.x - a.x) * t1;
        return pt.distanceTo(ptPerp);
    }
}

float mglnrel::ptToLine(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d& nearpt)
{
    Point2d ptTemp;
    float dist = mglnrel::ptToBeeline2(a, b, pt, nearpt);
    if (!mglnrel::isBetweenLine3(a, b, nearpt, &ptTemp))
    {
        nearpt = ptTemp;
        dist = pt.distanceTo(nearpt);
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
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))            // 平行或重合
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabsf(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    v = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    
    if (pu) *pu = u;
    if (pv) *pv = v;
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
}

bool mglnrel::cross2Line(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, const Tol& tolVec)
{
    float u, v, denom, cosnum;
    
    if (mgMin(a.x,b.x) - mgMax(c.x,d.x) > _MGZERO 
        || mgMin(c.x,d.x) - mgMax(a.x,b.x) > _MGZERO
        || mgMin(a.y,b.y) - mgMax(c.y,d.y) > _MGZERO 
        || mgMin(c.y,d.y) - mgMax(a.y,b.y) > _MGZERO)
        return false;
    
    denom = (c.x-d.x)*(b.y-a.y)-(c.y-d.y)*(b.x-a.x);
    if (mgIsZero(denom))
        return false;
    
    cosnum = (b.x-a.x)*(d.x - c.x) + (b.y-a.y)*(d.y-c.y);
    if (!mgIsZero(cosnum) && fabsf(denom / cosnum) < tolVec.equalVector())
        return false;
    
    u = ((c.x-a.x)*(d.y-c.y)-(c.y-a.y)*(d.x-c.x)) / denom;
    if (u < _MGZERO || u > 1.f - _MGZERO)
        return false;
    
    v = ((c.x-a.x)*(b.y-a.y)-(c.y-a.y)*(b.x-a.x)) / denom;
    if (v < _MGZERO || v > 1.f - _MGZERO)
        return false;
    
    ptCross.x = (1 - u) * a.x + u * b.x;
    ptCross.y = (1 - u) * a.y + u * b.y;
    
    return true;
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

// 线段端点的区域编码:
// 1001  |  1000  |  1010
// 0001  |  0000  |  0010
// 0101  |  0100  |  0110
static inline unsigned ClipCode(Point2d& pt, const Box2d& box)
{
    unsigned code = 0;
    if (pt.y > box.ymax)
        code |= 0x1000;
    else if (pt.y < box.ymin)
        code |= 0x0100;
    if (pt.x < box.xmin)
        code |= 0x0001;
    else if (pt.x > box.xmax)
        code |= 0x0010;
    return code;
}

bool mglnrel::clipLine(Point2d& pt1, Point2d& pt2, const Box2d& _box)
{
    Box2d box (_box);
    box.normalize();
    
    unsigned code1, code2;
    code1 = ClipCode(pt1, box);
    code2 = ClipCode(pt2, box);
    
    for ( ; ; )
    {
        if (!(code1 | code2))       // 完全在矩形内
            return true;
        if (code1 & code2)          // 完全在矩形外
            return false;
        
        float x = 0.f, y = 0.f;
        unsigned code;
        
        if (code1)                  // 起点不在矩形内
            code = code1;
        else                        // 终点不在矩形内
            code = code2;
        if (code & 0x1000)          // 上
        {
            x = pt1.x + (pt2.x - pt1.x) * (box.ymax - pt1.y) / (pt2.y - pt1.y);
            y = box.ymax;
        }
        else if (code & 0x0100)     // 下
        {
            x = pt1.x + (pt2.x - pt1.x) * (box.ymin - pt1.y) / (pt2.y - pt1.y);
            y = box.ymin;
        }
        else if (code & 0x0001)     // 左
        {
            y = pt1.y + (pt2.y - pt1.y) * (box.xmin - pt1.x) / (pt2.x - pt1.x);
            x = box.xmin;
        }
        else if (code & 0x0010)     // 右
        {
            y = pt1.y + (pt2.y - pt1.y) * (box.xmax - pt1.x) / (pt2.x - pt1.x);
            x = box.xmax;
        }
        
        if (code == code1)
        {
            pt1.x = x;
            pt1.y = y;
            code1 = ClipCode(pt1, box);
        }
        else
        {
            pt2.x = x;
            pt2.y = y;
            code2 = ClipCode(pt2, box);
        }
    }
}

static bool PtInArea_Edge(int &odd, const Point2d& pt, const Point2d& p1, 
                          const Point2d& p2, const Point2d& p0)
{
    // 如果从X方向上P不在边[P1,P2)上，则没有交点. 竖直边也没有
    if (!((p2.x > p1.x) && (pt.x >= p1.x) && (pt.x < p2.x)) &&
        !((p1.x > p2.x) && (pt.x <= p1.x) && (pt.x > p2.x)) )
    {
        return false;
    }
    
    // 求从Y负无穷大向上到P的射线和该边的交点(pt.x, yy)
    float yy = p1.y + (pt.x - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
    if (pt.y > yy)      // 相交
    {
        if (mgEquals(pt.x, p1.x))   // 交点是顶点, 则比较P[i+1]和P[i-1]是否在pt.x同侧
        {
            if (((p0.x > pt.x) && (p2.x > pt.x)) ||
                ((p0.x < pt.x) && (p2.x < pt.x)) )      // 同侧
            {
                return false;
            }
        }
        odd = 1 - odd;      // 增加一个交点, 奇偶切换
    }
    
    return true;
}

int mglnrel::ptInArea(
    const Point2d& pt, int count, const Point2d* pts, 
    int& order, const Tol& tol, bool closed)
{
    int i;
    int odd = 1;    // 1: 交点数为偶数, 0: 交点数为奇数
    float minDist = tol.equalPoint();
    Point2d nearpt;
    
    order = -1;
    for (i = 0; i < count && tol.equalPoint() < 1.e5f; i++)
    {
        // P与某顶点重合. 返回 kPtAtVertex, order = 顶点号 [0, count-1]
        float d = pt.distanceTo(pts[i]);
        if (minDist > d) {
            minDist = d;
            order = i;
        }
    }
    if (order >= 0) {
        return kPtAtVertex;
    }
    
    order = -1;
    minDist = tol.equalPoint();
    
    for (i = 0; i < (closed ? count : count - 1); i++)
    {
        const Point2d& p1 = pts[i];
        const Point2d& p2 = (i+1 < count) ? pts[i+1] : pts[0];
        
        // P在某条边上. 返回 kPtOnEdge, order = 边号 [0, count-1]
        float d = mglnrel::ptToBeeline2(p1, p2, pt, nearpt);
        if (minDist > d) {
            minDist = d;
            order = i;
        }
        else if (!PtInArea_Edge(odd, pt, p1, p2, 
                                i > 0 ? pts[i-1] : pts[count-1])) {
            continue;
        }
    }
    if (order >= 0) {
        return kPtOnEdge;
    }

    // 如果射线和多边形的交点数为偶数, 则 p==1, P在区外, 返回 kPtOutArea
    // 为奇数则p==0, P在区内, 返回 kPtInArea
    return 0 == odd ? kPtInArea : kPtOutArea;
}

bool mglnrel::isConvex(int count, const Point2d* vs, bool* pACW)
{
    if (count < 3 || !vs)
        return true;
    bool z0 = (vs[count-1].x - vs[count-2].x) * (vs[1].y - vs[0].y)
            > (vs[count-1].y - vs[count-2].y) * (vs[1].x - vs[0].x);
    for (int i = 0; i < count; i++)
    {
        if (z0 != ((vs[i].x - vs[i-1].x) * (vs[i+1].y - vs[i].y)
                 > (vs[i].y - vs[i-1].y) * (vs[i+1].x - vs[i].x)))
            return false;
    }
    if (pACW)
        *pACW = z0;
    return true;
}
