// mgcurv.cpp: 实现曲线拟和函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcurv.h"
#include "mgbase.h"
#include "mglnrel.h"
#include "mgdblpt.h"
#include "mglnrel.h"

void mgcurv::quadBezierToCubic(const Point2d quad[3], Point2d cubic[4])
{
    cubic[0] = quad[0];
    cubic[1] = quad[0] + 2.f / 3.f * (quad[1] - quad[0]);
    cubic[2] = quad[2] + 2.f / 3.f * (quad[1] - quad[2]);
    cubic[3] = quad[2];
}

void mgcurv::fitBezier(const Point2d* pts, float t, Point2d& fitpt)
{
    float v = 1.f - t;
    float v2 = v * v;
    float t2 = t * t;

    fitpt.x = (v2 * v) * pts[0].x + 3 * t * v2 * pts[1].x
        + 3 * t2 * v * pts[2].x + t2 * t * pts[3].x;
    fitpt.y = (v2 * v) * pts[0].y + 3 * t * v2 * pts[1].y
        + 3 * t2 * v * pts[2].y + t2 * t * pts[3].y;
}

/*
 http://www.planetclegg.com/projects/WarpingTextToSplines.html
 A = P3 - 3 * P2 + 3 * P1 - P0
 B = 3 * P2 - 6 * P1 + 3 * P0
 C = 3 * P1 - 3 * P0
 D = P0
 tangent: 3At^2 + 2Bt + C
 */
void mgcurv::bezierTanget(const Point2d* pts, float t, Point2d& outpt)
{
    Point2d A = pts[3] + (-3) * pts[2] + 3 * pts[1] + (-1) * pts[0];
    Point2d B = 3 * pts[2] + (-6) * pts[1] + 3 * pts[0];
    Point2d C = 3 * pts[1] + (-3) * pts[0];
    
    outpt = A * (3 * t * t) + B * (2 * t) + C;
}

void mgcurv::bezier4P(
    const Point2d& pt1, const Point2d& pt2, const Point2d& pt3, 
    const Point2d& pt4, Point2d& ctrpt1, Point2d& ctrpt2)
{
    Point2d ptCtr = (-5 * pt1 + 18 * pt2 -  9 * pt3 + 2 * pt4) / 6;
    ctrpt2      = (-5 * pt4 + 18 * pt3 -  9 * pt2 + 2 * pt1) / 6;
    ctrpt1 = ptCtr;
}

void mgcurv::splitBezier(const Point2d* pts, float t, Point2d* pts1, Point2d* pts2)
{
    Point2d &p5 = pts1[1];
    Point2d  p6;
    Point2d &p7 = pts2[2];
    Point2d &p8 = pts1[2];
    Point2d &p9 = pts2[1];
    Point2d &p10 = pts1[3];
    
    pts1[0] = pts[0];
    pts2[3] = pts[3];
    p5 = (1 - t) * pts[0] + t * pts[1];
    p6 = (1 - t) * pts[1] + t * pts[2];
    p7 = (1 - t) * pts[2] + t * pts[3];
    p8 = (1 - t) * p5 + t * p6;
    p9 = (1 - t) * p6 + t * p7;
    p10 = (1 - t) * p8 + t * p9;
    pts2[0] = p10;
}

bool mgcurv::bezierIsStraight(const Point2d* pts)
{
    return mglnrel::isColinear(pts[0], pts[3], pts[1]) || mglnrel::isColinear(pts[0], pts[3], pts[2]);
}

// lengthOfBezier, bezierPointDistantFromPoint and bezierIntersectionWithLine are modified
// from WDBezierSegment.m of Inkpad with the Mozilla Public License. Copyright (c) 2009-2013 Steve Sprang

static double base3(double t, double p1, double p2, double p3, double p4)
{
    double t1 = -3.0*p1 + 9.0*p2 - 9.0*p3 + 3.0*p4;
    double t2 = t*t1 + 6.0*p1 - 12.0*p2 + 6.0*p3;
    return t*t2 - 3.0*p1 + 3.0*p2;
}

static double cubicF(double t, const Point2d* pts)
{
    double xbase = base3(t, pts[0].x, pts[1].x, pts[2].x, pts[3].x);
    double ybase = base3(t, pts[0].y, pts[1].y, pts[2].y, pts[3].y);
    return mgHypot(xbase, ybase);
}

// Gauss quadrature for cubic Bezier curves http://processingjs.nihongoresources.com/bezierinfo/
float mgcurv::lengthOfBezier(const Point2d* pts)
{
    if (bezierIsStraight(pts)) {
        return pts[0].distanceTo(pts[3]);
    }
    
    double  z2 = 0.5;
    double  sum = 0.0f;
    
    // Legendre-Gauss abscissae (xi values, defined at i=n as the roots of the nth order Legendre polynomial Pn(x))
    static double Tvalues[] = {
        -0.06405689286260562997910028570913709, 0.06405689286260562997910028570913709,
        -0.19111886747361631067043674647720763, 0.19111886747361631067043674647720763,
        -0.31504267969616339684080230654217302, 0.31504267969616339684080230654217302,
        -0.43379350762604512725673089335032273, 0.43379350762604512725673089335032273,
        -0.54542147138883956269950203932239674, 0.54542147138883956269950203932239674,
        -0.64809365193697554552443307329667732, 0.64809365193697554552443307329667732,
        -0.74012419157855435791759646235732361, 0.74012419157855435791759646235732361,
        -0.82000198597390294708020519465208053, 0.82000198597390294708020519465208053,
        -0.88641552700440107148693869021371938, 0.88641552700440107148693869021371938,
        -0.93827455200273279789513480864115990, 0.93827455200273279789513480864115990,
        -0.97472855597130947380435372906504198, 0.97472855597130947380435372906504198,
        -0.99518721999702131064680088456952944, 0.99518721999702131064680088456952944
    };
    
    // Legendre-Gauss weights (wi values, defined by a function linked to in the Bezier primer article)
    static double Cvalues[] = {
        0.12793819534675215932040259758650790, 0.12793819534675215932040259758650790,
        0.12583745634682830250028473528800532, 0.12583745634682830250028473528800532,
        0.12167047292780339140527701147220795, 0.12167047292780339140527701147220795,
        0.11550566805372559919806718653489951, 0.11550566805372559919806718653489951,
        0.10744427011596563437123563744535204, 0.10744427011596563437123563744535204,
        0.09761865210411388438238589060347294, 0.09761865210411388438238589060347294,
        0.08619016153195327434310968328645685, 0.08619016153195327434310968328645685,
        0.07334648141108029983925575834291521, 0.07334648141108029983925575834291521,
        0.05929858491543678333801636881617014, 0.05929858491543678333801636881617014,
        0.04427743881741980774835454326421313, 0.04427743881741980774835454326421313,
        0.02853138862893366337059042336932179, 0.02853138862893366337059042336932179,
        0.01234122979998720018302016399047715, 0.01234122979998720018302016399047715
    };
    
    for (int i = 0; i < 24; i++) {
        double corrected_t = z2 * Tvalues[i] + z2;
        sum += Cvalues[i] * cubicF(corrected_t, pts);
    }
    
    return (float)(z2 * sum);
}

static float bezierPointLength_(const Point2d* pts, float len, float a, float b, Point2d* bsub)
{
    float t = (a + b) * 0.5f;
    
    if (b - a > 0.005f) {
        mgcurv::splitBezier(pts, t, bsub, bsub + 4);
        
        float s = mgcurv::lengthOfBezier(bsub);
        if (s > len + 1e-4f) {
            return bezierPointLength_(pts, len, a, t, bsub);
        }
        if (s < len - 1e-4f) {
            return bezierPointLength_(pts, len, t, b, bsub);
        }
    }
    
    return t;
}

float mgcurv::bezierPointLengthFromStart(const Point2d* pts, float len)
{
    Point2d bsub[8];
    return bezierPointLength_(pts, len, 0.f, 1.f, bsub);
}

bool mgcurv::bezierPointDistantFromPoint(const Point2d* pts, float dist, const Point2d& pt,
                                         Point2d &result, float &tRes)
{
    Point2d     current, last = pts[0];
    float       start = 0.0f, end = 1.0f, step = 0.1f;
    
    for (float t = start + step; t < (end + step); t += step) {
        fitBezier(pts, t, current);
        
        if (current.distanceTo(pt) >= dist) {
            start = (t - step); // back up one iteration
            end = t;
            
            // it's between the last and current point, let's get more precise
            step = 0.0001f;
            
            for (float t = start; t < (end + step); t += step) {
                fitBezier(pts, t, current);
                
                if (current.distanceTo(pt) >= dist) {
                    tRes = t - (step / 2);
                    fitBezier(pts, tRes, result);
                    return true;
                }
            }
        }
        
        last = current;
    }
    
    return false;
}

bool mgcurv::bezierIntersectionWithLine(const Point2d* pts, const Point2d& a, const Point2d& b, float &tIntersect)
{
    if (!Box2d(4, pts).isIntersect(Box2d(a, b))) {
        return false;
    }
    
    float       r, delta = 0.01f;
    Point2d     current, last = pts[0];
    
    for (float t = 0; t < (1.0f + delta); t += delta) {
        fitBezier(pts, t, current);
                
        if (mglnrel::cross2LineV(last, current, a, b, &r)) {
            tIntersect = mgMax(mgMin((t - delta) + delta * r, 1.f), 0.f);
            return true;
        }
        
        last = current;
    }
    
    return false;
}

void mgcurv::ellipse90ToBezier(
    const Point2d& frompt, const Point2d& topt, Point2d& ctrpt1, Point2d& ctrpt2)
{
    float rx = frompt.x - topt.x;
    float ry = topt.y - frompt.y;
    const Point2d center (topt.x, frompt.y);

    const float M = 0.5522847498307933984022516f; // 4(sqrt(2)-1)/3
    float dx = rx * M;
    float dy = ry * M;
    
    ctrpt1.x = center.x + rx;
    ctrpt1.y = center.y + dy;
    ctrpt2.x = center.x + dx;
    ctrpt2.y = center.y + ry;
}

void mgcurv::ellipseToBezier(
    Point2d points[13], const Point2d& center, float rx, float ry)
{
    const float M = 0.5522847498307933984022516f; // 4(sqrt(2)-1)/3
    float dx = rx * M;
    float dy = ry * M;
    
    points[ 0].x = center.x + rx;  //   .   .   .   .   .
    points[ 0].y = center.y;       //       4   3   2
    points[ 1].x = center.x + rx;  //
    points[ 1].y = center.y + dy;  //   5               1
    points[ 2].x = center.x + dx;  //
    points[ 2].y = center.y + ry;  //   6              0,12
    points[ 3].x = center.x;       //
    points[ 3].y = center.y + ry;  //   7               11
    //
    points[ 4].x = center.x - dx;  //       8   9   10
    points[ 4].y = center.y + ry;
    points[ 5].x = center.x - rx;
    points[ 5].y = center.y + dy;
    points[ 6].x = center.x - rx;
    points[ 6].y = center.y;
    
    points[ 7].x = center.x - rx;
    points[ 7].y = center.y - dy;
    points[ 8].x = center.x - dx;
    points[ 8].y = center.y - ry;
    points[ 9].x = center.x;
    points[ 9].y = center.y - ry;
    
    points[10].x = center.x + dx;
    points[10].y = center.y - ry;
    points[11].x = center.x + rx;
    points[11].y = center.y - dy;
    points[12].x = center.x + rx;
    points[12].y = center.y;
}

void mgcurv::roundRectToBeziers(
    Point2d points[16], const Box2d& rect, float rx, float ry)
{
    if (2 * rx > rect.width())
        rx = rect.width() / 2;
    if (2 * ry > rect.height())
        ry = rect.height() / 2;

    int i, j;
    float dx = rect.width() / 2 - rx;
    float dy = rect.height() / 2 - ry;

    mgcurv::ellipseToBezier(points, rect.center(), rx, ry);

    for (i = 3; i >= 1; i--)
    {
        for (j = 3; j >= 0; j--)
            points[4 * i + j] = points[3 * i + j];
    }
    for (i = 0; i < 4; i++)
    {
        float dx1 = (0 == i || 3 == i) ? dx : -dx;
        float dy1 = (0 == i || 1 == i) ? dy : -dy;
        for (j = 0; j < 4; j++)
            points[4 * i + j].offset(dx1, dy1);
    }
}

static void _arcToBezier(
    Point2d points[4], const Point2d& center, float rx, float ry,
    float startAngle, float sweepAngle)
{
    // Compute bezier curve for arc centered along y axis
    // Anticlockwise: (0,-B), (x,-y), (x,y), (0,B)
    float sy = ry / rx;
    ry = rx;
    float B = ry * sinf(sweepAngle / 2);
    float C = rx * cosf(sweepAngle / 2);
    float A = rx - C;
    
    float X = A * 4 / 3;
    float Y = B - X * (rx-A)/B;
    
    points[0].set(C,    -B);
    points[1].set(C+X,  -Y);
    points[2].set(C+X,  Y);
    points[3].set(C,    B);
    
    // rotate to the original angle
    A = startAngle + sweepAngle / 2;
    float s = sinf(A);
    float c = cosf(A);
    
    for (int i = 0; i < 4; i++)
    {
        points[i].set(center.x + points[i].x * c - points[i].y * s,
            center.y + points[i].x * s * sy + points[i].y * c * sy);
    }
}

static int _arcToBezierPlusSweep(
    Point2d points[16], const Point2d& center, float rx, float ry, 
    float startAngle, float sweepAngle)
{
    const float M = 0.5522847498307933984022516f;
    float dx = rx * M;
    float dy = ry * M;

    int k, n;
    float endAngle;

    // 计算第一段椭圆弧的终止角度
    if (startAngle < _M_PI_2) {             // +Y
        endAngle = _M_PI_2;
        k = 1;
    }
    else if (startAngle < _M_PI) {          // -X
        endAngle = _M_PI;
        k = 2;
    }
    else if (startAngle < 3*_M_PI_2) {      // -Y
        endAngle = 3*_M_PI_2;
        k = 3;
    }
    else {                                  // +X
        endAngle = _M_2PI;
        k = 0;
    }
    if (endAngle - startAngle > 1e-5)       // 转换第一段椭圆弧
    {
        _arcToBezier(points, center, rx, ry,
            startAngle, endAngle - startAngle);
        n = 4;
    }
    else
        n = 1;                              // 第一点在下边循环内设置
    sweepAngle -= (endAngle - startAngle);
    startAngle = endAngle;
    while (sweepAngle >= _M_PI_2)           // 增加整90度弧
    {
        if (k == 0)                         // 第一象限
        {
            points[n-1].set(center.x + rx, center.y);
            points[n  ].set(center.x + rx, center.y + dy);
            points[n+1].set(center.x + dx, center.y + ry);
            points[n+2].set(center.x,      center.y + ry);
        }
        else if (k == 1)                    // 第二象限
        {
            points[n-1].set(center.x,      center.y + ry);
            points[n  ].set(center.x - dx, center.y + ry);
            points[n+1].set(center.x - rx, center.y + dy);
            points[n+2].set(center.x - rx, center.y);
        }
        else if (k == 2)                    // 第三象限
        {
            points[n-1].set(center.x - rx, center.y);
            points[n  ].set(center.x - rx, center.y - dy);
            points[n+1].set(center.x - dx, center.y - ry);
            points[n+2].set(center.x,      center.y - ry);
        }
        else                                // 第四象限
        {
            points[n-1].set(center.x,      center.y - ry);
            points[n  ].set(center.x + dx, center.y - ry);
            points[n+1].set(center.x + rx, center.y - dy);
            points[n+2].set(center.x + rx, center.y);
        }
        k = (k + 1) % 4;
        n += 3;
        sweepAngle -= _M_PI_2;
        startAngle += _M_PI_2;
    }
    if (sweepAngle > 1e-5)                  // 增加余下的弧
    {
        _arcToBezier(&points[n-1], center, rx, ry, startAngle, sweepAngle);
        n += 3;
    }

    return n;
}

int mgcurv::arcToBezier(
    Point2d points[16], const Point2d& center, float rx, float ry,
    float startAngle, float sweepAngle)
{
    if (mgIsZero(rx) || fabsf(sweepAngle) < 1e-5)
        return 0;
    if (mgIsZero(ry))
        ry = rx;
    if (sweepAngle > _M_2PI)
        sweepAngle = _M_2PI;
    else if (sweepAngle < -_M_2PI)
        sweepAngle = -_M_2PI;
    
    int n = 0;
    
    if (fabsf(sweepAngle) < _M_PI_2 + 1e-5)
    {
        _arcToBezier(points, center, rx, ry, startAngle, sweepAngle);
        n = 4;
    }
    else if (sweepAngle > 0)
    {
        startAngle = mgbase::to0_2PI(startAngle);
        n = _arcToBezierPlusSweep(
            points, center, rx, ry, startAngle, sweepAngle);
    }
    else // sweepAngle < 0
    {
        float endAngle = startAngle + sweepAngle;
        sweepAngle = -sweepAngle;
        startAngle = mgbase::to0_2PI(endAngle);
        n = _arcToBezierPlusSweep(
            points, center, rx, ry, startAngle, sweepAngle);

        for (int i = 0; i < n / 2; i++)
            mgSwap(points[i], points[n - 1 - i]);
    }
    
    return n;
}

bool mgcurv::arc3P(
    const Point2d& start, const Point2d& point, const Point2d& end,
    Point2d& center, float& radius,
    float* startAngle, float* sweepAngle)
{
    float a1, b1, c1, a2, b2, c2;
    
    a1 = end.x - start.x;
    b1 = end.y - start.y;
    c1 = -0.5f * (a1 * (end.x + start.x) + b1 * (end.y + start.y));
    a2 = end.x - point.x;
    b2 = end.y - point.y;
    c2 = -0.5f * (a2 * (end.x + point.x) + b2 * (end.y + point.y));
    if (!mglnrel::crossLineAbc(a1, b1, c1, a2, b2, c2, center, Tol::gTol()))
        return false;
    radius = mgHypot(center.x - start.x, center.y - start.y);
    
    if (startAngle && sweepAngle)
    {
        // 分别计算圆心到三点的角度
        float a = atan2f(start.y - center.y, start.x - center.x);
        float b = atan2f(point.y - center.y, point.x - center.x);
        float c = atan2f(end.y - center.y, end.x - center.x);
        
        *startAngle = a;
        
        // 判断圆弧的方向，计算转角
        if (a < c)
        {
            if (a < b && b < c)         // 逆时针
                *sweepAngle = c - a;
            else
                *sweepAngle = c - a - _M_2PI;
        }
        else
        {
            if (a > b && b > c)         // 顺时针
                *sweepAngle = c - a;
            else
                *sweepAngle = _M_2PI-(a-c);
        }
    }
    
    return true;
}

bool mgcurv::arcTan(
    const Point2d& start, const Point2d& end, const Vector2d& startTan,
    Point2d& center, float& radius,
    float* startAngle, float* sweepAngle)
{
    float a, b, c;
    
    // 弦的中垂线方程系数
    a = end.x - start.x;
    b = end.y - start.y;
    c = -0.5f * (a*(end.x + start.x) + b*(end.y + start.y));
    
    // 求中垂线和切线的交点center
    if (!mglnrel::crossLineAbc(a, b, c, startTan.x, startTan.y, 
        -startTan.x * start.x - startTan.y * start.y, center, Tol::gTol()))
        return false;
    radius = mgHypot(center.x - start.x, center.y - start.y);
    
    if (startAngle && sweepAngle)
    {
        float sa = atan2f(start.y - center.y, start.x - center.x);
        float ea = atan2f(end.y - center.y, end.x - center.x);
        *startAngle = sa;
        if (startTan.crossProduct(start - center) > 0.f)
            *sweepAngle = -mgbase::to0_2PI(sa - ea);
        else
            *sweepAngle = mgbase::to0_2PI(ea - sa);
    }
    
    return true;
}

bool mgcurv::arcBulge(
    const Point2d& start, const Point2d& end, float bulge,
    Point2d& center, float& radius,
    float* startAngle, float* sweepAngle)
{
    Point2d point ((start.x + end.x)*0.5f, (start.y + end.y)*0.5f);
    point = point.rulerPoint(end, bulge);
    return mgcurv::arc3P(start, point, end, center, radius, startAngle, sweepAngle);
}

static int _crossTwoCircles(point_t& pt1, point_t& pt2,
                            const point_t& c1, double r1,
                            const point_t& c2, double r2)
{
    double d, a, b, c, p, q, r;
    double cos_value[2], sin_value[2];
    
    if (mgEquals(c1.x, c2.x) && mgEquals(c1.y, c2.y) && mgEquals(r1, r2)) {
        return -1;
    }
    
    d = c1.distanceTo(c2);
    if (d > r1 + r2 || d < fabs(r1 - r2)) {
        return 0;
    }
    
    a = 2.0 * r1 * (c1.x - c2.x);
    b = 2.0 * r1 * (c1.y - c2.y);
    c = r2 * r2 - r1 * r1 - c1.distanceSquare(c2);
    p = a * a + b * b;
    q = -2.0 * a * c;
    if (mgEquals(d, r1 + r2) || mgEquals(d, fabs(r1 - r2))) {
        cos_value[0] = -q / p / 2.0;
        sin_value[0] = sqrt(1 - cos_value[0] * cos_value[0]);
        
        pt1.x = r1 * cos_value[0] + c1.x;
        pt1.y = r1 * sin_value[0] + c1.y;
        
        if (!mgEquals(pt1.distanceSquare(c2), r2 * r2)) {
            pt1.y = c1.y - r1 * sin_value[0];
        }
        return 1;
    }
    
    r = c * c - b * b;
    cos_value[0] = (sqrt(q * q - 4.0 * p * r) - q) / p / 2.0;
    cos_value[1] = (-sqrt(q * q - 4.0 * p * r) - q) / p / 2.0;
    sin_value[0] = sqrt(1 - cos_value[0] * cos_value[0]);
    sin_value[1] = sqrt(1 - cos_value[1] * cos_value[1]);
    
    pt1.x = r1 * cos_value[0] + c1.x;
    pt2.x = r1 * cos_value[1] + c1.x;
    pt1.y = r1 * sin_value[0] + c1.y;
    pt2.y = r1 * sin_value[1] + c1.y;
    
    if (!mgEquals(pt1.distanceSquare(c2), r2 * r2)) {
        pt1.y = c1.y - r1 * sin_value[0];
    }
    if (!mgEquals(pt2.distanceSquare(c2), r2 * r2)) {
        pt2.y = c1.y - r1 * sin_value[1];
    }
    if (mgEquals(pt1.y, pt2.y) && mgEquals(pt1.x, pt2.x)) {
        if (pt1.y > 0) {
            pt2.y = -pt2.y;
        } else {
            pt1.y = -pt1.y;
        }
    }
    return 2;
}

// http://blog.csdn.net/cyg0810/article/details/7765894
int mgcurv::crossTwoCircles(Point2d& pt1, Point2d& pt2,
                            const Point2d& c1, float r1, const Point2d& c2, float r2)
{
    point_t p1, p2;
    point_t ca(c1.x, c1.y);
    point_t cb(c2.x, c2.y);
    int n = _crossTwoCircles(p1, p2, ca, r1, cb, r2);
    
    pt1.set((float)p1.x, (float)p1.y);
    pt2.set((float)p2.x, (float)p2.y);
    
    return n;
}

// http://mathworld.wolfram.com/Circle-LineIntersection.html
int _crossLineCircle(point_t& pt1, point_t& pt2, const point_t& a,
                     const point_t& b, double r)
{
    point_t d(b - a);
    double d2 = d.lengthSquare();
    double dz = a.crossProduct(b);
    double z2 = dz * dz;
    double delta = r * r * d2 - z2;
    
    if (delta < 0)
        return 0;
    
    double s = sqrt(delta) / d2;
    double sx = (d.y < 0 ? -d.x : d.x) * s;
    double sy = fabs(d.y) * s;
    double tx = dz * d.y / d2;
    double ty = -dz * d.x / d2;
    
    pt1 = point_t(tx + sx, ty + sy);
    pt2 = point_t(tx - sx, ty - sy);
    
    return delta < 1e-8 ? 1 : 2;
}

int mgcurv::crossLineCircle(Point2d& pt1, Point2d& pt2, const Point2d& a,
                            const Point2d& b, const Point2d& c, float r, bool ray)
{
    if (a == b) {
        return 0;
    }
    
    Point2d perp;
    float dist = mglnrel::ptToBeeline2(a, b, c, perp);
    
    if (fabsf(dist - r) < r * 1e-3f) {
        pt1 = c.rulerPoint(perp, r, 0);
        pt2 = pt1;
        return 1;
    }
    
    point_t p1, p2;
    point_t a1(a.x - c.x, a.y - c.y);
    point_t b1(b.x - c.x, b.y - c.y);
    int n = _crossLineCircle(p1, p2, a1, b1, r);
    
    pt1.set((float)p1.x + c.x, (float)p1.y + c.y);
    pt2.set((float)p2.x + c.x, (float)p2.y + c.y);
    
    if (ray && n > 0) {
        bool b1 = mglnrel::isProjectBetweenRayline(a, b, pt1);
        bool b2 = mglnrel::isProjectBetweenRayline(a, b, pt2);
        
        if (!b1 && !b2) {
            n = 0;
        }
        else if (!b1 || !b2) {
            n = 1;
            if (!b1) {
                pt1 = pt2;
            } else {
                pt2 = pt1;
            }
        }
    }
    
    return n;
}

bool mgcurv::triEquations(
    int n, float *a, float *b, float *c, Vector2d *vs)
{
    if (!a || !b || !c || !vs || n < 2)
        return false;
    
    float w;
    int i;
    
    w = b[0];
    if (mgIsZero(w))
        return false;
    w = 1 / w;
    vs[0].x = vs[0].x * w;
    vs[0].y = vs[0].y * w;
    
    for (i = 0; i <= n-2; i++)
    {
        b[i] = c[i] * w;
        w = b[i+1] - a[i] * b[i];
        if (mgIsZero(w))
            return false;
        w = 1 / w;
        vs[i+1].x = (vs[i+1].x - a[i] * vs[i].x) * w;
        vs[i+1].y = (vs[i+1].y - a[i] * vs[i].y) * w;
    }
    
    for (i = n-2; i >= 0; i--)
    {
        vs[i].x -= b[i] * vs[i+1].x;
        vs[i].y -= b[i] * vs[i+1].y;
    }
    
    return true;
}

bool mgcurv::gaussJordan(int n, float *mat, Vector2d *vs)
{
    int i, j, k, m;
    float c, t;
    Vector2d tt;
    
    if (!mat || !vs || n < 2)
        return false;
    
    for (k = 0; k < n; k++) {
        // 找主元. 即找第k列中第k行以下绝对值最大的元素
        m = k;
        c = mat[k*n+k];
        for (i = k+1; i < n; i++) {
            if (fabsf(mat[i*n+k]) > fabsf(c)) {
                m = i;
                c = mat[i*n+k];
            }
        }
        // 交换第k行和第m行中第k列以后的元素
        if (m != k) {
            for (j = k; j < n; j++) {
                t = mat[m*n+j]; mat[m*n+j] = mat[k*n+j]; mat[k*n+j] = t; 
            }
            tt = vs[m]; vs[m] = vs[k]; vs[k] = tt;
        }
        // 消元. 第k行中第k列以后元素/=mat[k][k]
        c = mat[k*n+k];
        if (mgIsZero(c))
            return false;
        c = 1.f / c;
        for (j = k; j < n; j++)
            mat[k*n+j] *= c;
        vs[k].x = vs[k].x * c;
        vs[k].y = vs[k].y * c;
        // 从第k+1行以下每一行, 对该行第k列以后各元素-=
        for (i = k+1; i < n; i++) {
            c = mat[i*n+k];
            for (j = k; j < n; j++)
                mat[i*n+j] -= mat[k*n+j] * c;
            vs[i].x -= vs[k].x * c;
            vs[i].y -= vs[k].y * c;
        }
    }
    
    // 回代
    for (i = n-2; i >= 0; i--) {
        for (j = i; j < n; j++) {
            vs[i].x -= mat[i*n+j+1] * vs[j+1].x;
            vs[i].y -= mat[i*n+j+1] * vs[j+1].y;
        }
    }
    
    return true;
}

static bool CalcCubicClosed(
    int n, float* a, Vector2d* vecs, const Point2d* knots)
{
    int i, n1 = n - 1;

    for (i = n*n - 1; i >= 0; i--)
        a[i] = 0.f;
    
    a[n1] = 1.0;
    a[0]  = 4.0;
    a[1]  = 1.0;
    a[n1*n+n1 - 1] = 1.0;
    a[n1*n+n1]   = 4.0;
    a[n1*n+0]    = 1.0;
    vecs[0].x  = 3 * (knots[1].x-knots[n1].x);
    vecs[0].y  = 3 * (knots[1].y-knots[n1].y);
    vecs[n1].x = 3 * (knots[0].x-knots[n1 - 1].x);
    vecs[n1].y = 3 * (knots[0].y-knots[n1 - 1].y);
    
    for (i = 1; i < n1; i++) {
        a[i*n+i-1] = 1.0;
        a[i*n+i]   = 4.0;
        a[i*n+i+1] = 1.0;
        vecs[i].x = 3 * (knots[i+1].x-knots[i-1].x);
        vecs[i].y = 3 * (knots[i+1].y-knots[i-1].y);
    }
    
    return mgcurv::gaussJordan(n, a, vecs);
}

static bool CalcCubicUnclosed(
    int flag, int n, const Point2d* knots, 
    float* a, float* b, float* c, Vector2d* vecs)
{
    if (flag & mgcurv::cubicTan1) {         // 起始夹持端
        b[0] = 1.0;
        c[0] = 0.0;
        //vecs[0] = xp0;                    // vecs[0]必须指定切矢量
    }
    else if (flag & mgcurv::cubicArm1) {    // 起始悬臂端
        b[0] = 1.0;
        c[0] = 1.0;
        vecs[0].x = 2 * (knots[1].x-knots[0].x);
        vecs[0].y = 2 * (knots[1].y-knots[0].y);
    }
    else {                                  // 起始自由端
        b[0] = 1.0;
        c[0] = 0.5;
        vecs[0].x = 1.5f * (knots[1].x-knots[0].x);
        vecs[0].y = 1.5f * (knots[1].y-knots[0].y);
    }
    
    if (flag & mgcurv::cubicTan2) {         // 终止夹持端
        a[n - 2] = 0.0;
        b[n - 1] = 1.0;
        //vecs[n - 1] = xpn;                // vecs[n-1]必须指定切矢量
    }
    else if (flag & mgcurv::cubicArm2) {    // 终止悬臂端
        a[n - 2] = 1.0;
        b[n - 1] = 1.0;
        vecs[n - 1].x = 2 * (knots[n - 1].x-knots[n - 2].x);
        vecs[n - 1].y = 2 * (knots[n - 1].y-knots[n - 2].y);
    }
    else {                                  // 终止自由端
        a[n - 2] = 0.5;
        b[n - 1] = 1.0;
        vecs[n - 1].x = 1.5f * (knots[n - 1].x-knots[n - 2].x);
        vecs[n - 1].y = 1.5f * (knots[n - 1].y-knots[n - 2].y);
    }
    
    for (int i = 1; i < n - 1; i++) {
        a[i-1] = 1.0;
        b[i] = 4.0;
        c[i] = 1.0;
        vecs[i].x = 3 * (knots[i+1].x-knots[i-1].x);
        vecs[i].y = 3 * (knots[i+1].y-knots[i-1].y);
    }
    
    return mgcurv::triEquations(n, a, b, c, vecs);
}

bool mgcurv::cubicSplines(
    int n, const Point2d* knots, Vector2d* knotvs,
    int flag, float tension)
{
    bool ret = false;
    
    if (!knots || !knotvs || n < 2)
        return false;
    
    if ((flag & cubicLoop) && n <= 512) {
        float* a = new float[n * n];
        ret = a && CalcCubicClosed(n, a, knotvs, knots);
        delete[] a;
    }
    else {
        float* a = new float[n * 3];
        ret = a && CalcCubicUnclosed(flag, n, knots, 
            a, a+n, a+2*n, knotvs);
        delete[] a;
    }
    
    if (!mgEquals(tension, 1.f)) {
        for (int i = 0; i < n; i++) {
            knotvs[i].x *= tension;
            knotvs[i].y *= tension;
        }
    }

    return ret;
}

void mgcurv::fitCubicSpline(
    int n, const Point2d* knots, const Vector2d* knotvs,
    int i, float t, Point2d& fitpt)
{
    float b2, b3;
    int i1 = i % n;
    int i2 = (i+1) % n;
    
    b2 = 3*(knots[i2].x - knots[i1].x) - 2*knotvs[i1].x - knotvs[i2].x;
    b3 = 2*(knots[i1].x - knots[i2].x) + knotvs[i1].x + knotvs[i2].x;
    fitpt.x = knots[i1].x + (knotvs[i1].x + (b2 + b3*t)*t)*t;
    
    b2 = 3*(knots[i2].y - knots[i1].y) - 2*knotvs[i1].y - knotvs[i2].y;
    b3 = 2*(knots[i1].y - knots[i2].y) + knotvs[i1].y + knotvs[i2].y;
    fitpt.y = knots[i1].y + (knotvs[i1].y + (b2 + b3*t)*t)*t;
}

void mgcurv::cubicSplineToBezier(
    int n, const Point2d* knots, const Vector2d* knotvs,
    int i, Point2d points[4], bool hermite)
{
    int i1 = i % n;
    int i2 = (i+1) % n;
    float d = hermite ? 1.f/3.f : 1.f;
    points[0] = knots[i1];
    points[1] = knots[i1] + knotvs[i1] * d;
    points[2] = knots[i2] - knotvs[i2] * d;
    points[3] = knots[i2];
}

//! The helper class for FitCurve()
struct FitCurveHelper {
    int index;
    int knotCount;
    Point2d* knots;
    Vector2d* knotvs;
    
    static void append(void* data, const Point2d curve[4]) {
        FitCurveHelper* p = (FitCurveHelper*)data;
        
        if (p->knotvs) {
            if (p->index > 0 && p->knots[p->index - 1] == curve[0]) {
                if (p->index < p->knotCount) {
                    p->knots[p->index] = curve[3];
                    p->knotvs[p->index++] = curve[3] - curve[2];
                }
            } else if (p->index + 1 < p->knotCount) {
                p->knots[p->index] = curve[0];
                p->knots[p->index + 1] = curve[3];
                p->knotvs[p->index] = curve[1] - curve[0];
                p->knotvs[p->index + 1] = curve[3] - curve[2];
                p->index += 2;
            }
        } else {
            if (p->index > 0 && p->knots[p->index - 1] == curve[0]) {
                if (p->index + 2 < p->knotCount) {
                    p->knots[p->index++] = curve[2];
                    p->knots[p->index++] = curve[3];
                }
            } else if (p->index + 4 < p->knotCount) {
                p->knots[p->index++] = curve[0];
                p->knots[p->index++] = curve[1];
                p->knots[p->index++] = curve[2];
                p->knots[p->index++] = curve[3];
            }
        }
    }
};

extern void FitCurve(mgcurv::FitCubicCallback, void*, const Point2d *, int, float);
extern void FitCurve2(mgcurv::FitCubicCallback, void*, mgcurv::PtCallback, void*, int, float);

int mgcurv::fitCurve(int knotCount, Point2d* knots, Vector2d* knotvs,
                     int count, const Point2d* pts, float tol)
{
    FitCurveHelper helper;
    
    helper.index = 0;
    helper.knotCount = knotCount;
    helper.knots = knots;
    helper.knotvs = knotvs;
    
    FitCurve(&FitCurveHelper::append, &helper, pts, count, tol);
    return helper.index;
}

int mgcurv::fitCurve2(int knotCount, Point2d* knots, int count, PtCallback pts, void* data, float tol)
{
    FitCurveHelper helper;
    
    helper.index = 0;
    helper.knotCount = knotCount;
    helper.knots = knots;
    helper.knotvs = (Vector2d*)0;
    
    FitCurve2(&FitCurveHelper::append, &helper, pts, data, count, tol);
    return helper.index;
}

void mgcurv::fitCurve3(FitCubicCallback fc, void* data, const Point2d *pts, int n, float tol)
{
    FitCurve(fc, data, pts, n, tol);
}

void mgcurv::fitCurve4(FitCubicCallback fc, void* data, PtCallback pts, void* data2, int n, float tol)
{
    FitCurve2(fc, data, pts, data2, n, tol);
}
