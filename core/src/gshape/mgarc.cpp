// mgarc.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgarc.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgArc)

MgArc::MgArc() : _sweepAngle(0), _subtype(0)
{
}

MgArc::~MgArc()
{
}

Point2d MgArc::getCenter() const
{
    return _points[0];
}

Point2d MgArc::getStartPoint() const
{
    return _points[1];
}

Point2d MgArc::getEndPoint() const
{
    return _points[2];
}

Point2d MgArc::getMidPoint() const
{
    return _points[3];
}

float MgArc::getRadius() const
{
    return getStartPoint().distanceTo(getCenter());
}

float MgArc::getStartAngle() const
{
   return (getStartPoint() - getCenter()).angle2();
}

float MgArc::getEndAngle() const
{
    return (getEndPoint() - getCenter()).angle2();
}

bool MgArc::_isClosed() const
{
    return _subtype > 0 || fabsf(getSweepAngle()) > _M_2PI - 1e-3f;
}

float MgArc::getSweepAngle() const
{
    if (!mgIsZero(_sweepAngle)) {
        return _sweepAngle;
    }
    
    const float midAngle = (getMidPoint() - getCenter()).angle2();
    const float startAngle = getStartAngle();
    const float endAngle = getEndAngle();

    if (mgEquals(midAngle, startAngle) && mgEquals(startAngle, endAngle)) {
        return endAngle - startAngle;
    }
    
    Tol tol(getRadius() * 1e-3f, 1e-4f);
    if (getStartPoint().isEqualTo(getEndPoint(), tol)
        && (getMidPoint() + (getStartPoint() + getEndPoint()) / 2).isEqualTo(2 * getCenter(), tol)) {
        return _M_2PI;
    }

    float startAngle2 = startAngle;
    float midAngle2 = midAngle;
    float endAngle2 = endAngle;

    // 先尝试看是否为逆时针方向：endAngle2 > midAngle2 > startAngle2 >= 0
    if (startAngle2 < 0)
        startAngle2 += _M_2PI;
    while (midAngle2 < startAngle2)
        midAngle2 += _M_2PI;
    while (endAngle2 < midAngle2)
        endAngle2 += _M_2PI;

    if (fabsf(startAngle2 + endAngle2 - 2 * midAngle2) < _M_PI_6
        && endAngle2 - startAngle2 < _M_2PI) {
        return endAngle2 - startAngle2;
    }

    // 再尝试看是否为顺时针方向：endAngle2 < midAngle2 < startAngle2 <= 0
    startAngle2 = startAngle;
    midAngle2 = midAngle;
    endAngle2 = endAngle;
    if (startAngle2 > 0)
        startAngle2 -= _M_2PI;
    while (midAngle2 > startAngle2)
        midAngle2 -= _M_2PI;
    while (endAngle2 > midAngle2)
        endAngle2 -= _M_2PI;

    if (fabsf(startAngle2 + endAngle2 - 2 * midAngle2) < _M_PI_6) {
        if (endAngle2 - startAngle2 > -_M_2PI)
            return endAngle2 - startAngle2;
        return mgbase::toRange(endAngle2 - startAngle2, -_M_2PI, 0);
    }

    return endAngle - startAngle;   // error
}

Vector2d MgArc::getStartTangent() const
{
    Vector2d rec(getStartPoint() - getCenter());
    return getSweepAngle() > 0 ? rec.perpVector() : -rec.perpVector();
}

Vector2d MgArc::getEndTangent() const
{
    Vector2d rec(getEndPoint() - getCenter());
    return getSweepAngle() > 0 ? rec.perpVector() : -rec.perpVector();
}

bool MgArc::setCenterRadius(const Point2d& center, float radius, float startAngle, float sweepAngle)
{
    if (sweepAngle > _M_2PI - 1e-3)
        sweepAngle = _M_2PI;
    else if (sweepAngle < 1e-3 - _M_2PI)
        sweepAngle = -_M_2PI;
    
    _sweepAngle = sweepAngle;
    _points[0] = center;
    _points[1] = center.polarPoint(startAngle, radius);
    _points[2] = center.polarPoint(startAngle + sweepAngle, radius);
    _points[3] = center.polarPoint(startAngle + sweepAngle / 2, radius);
    _update();
    
    return true;
}

bool MgArc::setStartMidEnd(const Point2d& start, const Point2d& point, const Point2d& end)
{
    Point2d center;
    float r, startAngle, sweepAngle = 0;

    return (mgcurv::arc3P(start, point, end, center, r, &startAngle, &sweepAngle)
            && setCenterRadius(center, r, startAngle, sweepAngle));
}

bool MgArc::setCenterStartEnd(const Point2d& center, const Point2d& start)
{
    float startAngle = (start - center).angle2();
    return setCenterRadius(center, start.distanceTo(center), startAngle, 0);
}

bool MgArc::setCenterStartEnd(const Point2d& center, const Point2d& start, const Point2d& end)
{
    return setCSE(center, start, end, getSweepAngle());
}

bool MgArc::setCSE(const Point2d& center, const Point2d& start, 
                   const Point2d& end, float lastSweepAngle)
{
    float startAngle = (start - center).angle2();
    float endAngle = (end - center).angle2();
    float sweepAngle = mgbase::toRange(endAngle - startAngle, -_M_2PI, _M_2PI);
    
    if (fabsf(sweepAngle - lastSweepAngle) > _M_PI) {
        if (fabsf(sweepAngle) < _M_D2R * 5 && fabsf(lastSweepAngle) > _M_PI + _M_PI_2) {
            sweepAngle = lastSweepAngle > 0 ? _M_2PI : -_M_2PI;
        } else {
            sweepAngle = sweepAngle + (sweepAngle > 0 ?  -_M_2PI :  _M_2PI);
        }
    }

    return setCenterRadius(center, start.distanceTo(center), startAngle, sweepAngle);
}

bool MgArc::setTanStartEnd(const Vector2d& startTan, const Point2d& start, const Point2d& end)
{
    Point2d center;
    float radius, startAngle, sweepAngle = 0;

    return (mgcurv::arcTan(start, end, startTan, center, radius, &startAngle, &sweepAngle)
            && setCenterRadius(center, radius, startAngle, sweepAngle));
}

void MgArc::_output(MgPath& path) const
{
    float r = getRadius();
    float sweepAngle = getSweepAngle();
    Point2d points[16];
    
    if (r < _MGZERO || fabsf(sweepAngle) < _MGZERO)
        return;
    
    int count = mgcurv::arcToBezier(points, getCenter(), r, r,
                                    getStartAngle(), sweepAngle);
    
    if (_subtype > 0) {
        path.moveTo(getCenter());
        path.lineTo(points[0]);
        path.beziersTo(count - 1, points + 1);
        path.closeFigure();
    } else {
        path.moveTo(points[0]);
        path.beziersTo(count - 1, points + 1);
    }
}

void MgArc::_update()
{
    Point2d points[16];
    int n = mgcurv::arcToBezier(points, getCenter(), getRadius(), 0, getStartAngle(), getSweepAngle());

    mgnear::beziersBox(_extent, n, points);
    if (_subtype > 0) {
        _extent.unionWith(getCenter());
    }
    __super::_update();
}

bool MgArc::_reverse()
{
    mgSwap(_points[1], _points[2]);
    return true;
}

int MgArc::_getPointCount() const
{
    return 4;
}

Point2d MgArc::_getPoint(int index) const
{
    return _points[index < 0 ? 0 : index % 4];
}

void MgArc::_setPoint(int index, const Point2d& pt)
{
    _points[index % 4] = pt;
    _sweepAngle = 0;
}

void MgArc::_copy(const MgArc& src)
{
    for (int i = 0; i < _getPointCount(); i++)
        _points[i] = src._points[i];
    _sweepAngle = src._sweepAngle;
    _subtype = src._subtype;
    __super::_copy(src);
}

bool MgArc::_equals(const MgArc& src) const
{
    for (int i = 0; i < _getPointCount(); i++) {
        if (_points[i] != src._points[i])
            return false;
    }
    return _subtype == src._subtype && __super::_equals(src);
}

void MgArc::_transform(const Matrix2d& mat)
{
    mat.transformPoints(_getPointCount(), _points);
    __super::_transform(mat);
}

void MgArc::_clear()
{
    for (int i = 0; i < _getPointCount(); i++)
        _points[i] = Point2d();
    _sweepAngle = 0;
    __super::_clear();
}

float MgArc::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    Point2d points[16];
    int n = mgcurv::arcToBezier(points, getCenter(), getRadius(), 0, getStartAngle(), getSweepAngle());

    float dist, distMin = _FLT_MAX;
    Point2d ptTemp;

    if (_subtype > 0) {
        dist = mglnrel::ptToLine(getCenter(), getStartPoint(), pt, ptTemp);
        if (dist <= tol && dist < distMin) {
            distMin = dist;
            res.nearpt = ptTemp;
        }
        dist = mglnrel::ptToLine(getCenter(), getEndPoint(), pt, ptTemp);
        if (dist <= tol && dist < distMin) {
            distMin = dist;
            res.nearpt = ptTemp;
        }
    }
    for (int i = 0; i + 3 < n; i += 3) {
        mgnear::nearestOnBezier(pt, points + i, ptTemp);
        dist = pt.distanceTo(ptTemp);
        if (dist <= tol && dist < distMin) {
            distMin = dist;
            res.nearpt = ptTemp;
        }
    }
    
    return distMin;
}

bool MgArc::_hitTestBox(const Box2d& rect) const
{
    if (!getExtent().isIntersect(rect))
        return false;

    Point2d points[16];
    int n = mgcurv::arcToBezier(points, getCenter(), getRadius(), 0, getStartAngle(), getSweepAngle());
    
    return rect.contains(getCenter()) || mgnear::beziersIntersectBox(rect, n, points);
}

bool MgArc::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 8);
    s->writeInt("subtype", _subtype);
    return ret;
}

bool MgArc::_load(MgShapeFactory* factory, MgStorage* s)
{
    _subtype = s->readInt("subtype", _subtype);
    return __super::_load(factory, s) && s->readFloatArray("points", &(_points[0].x), 8) == 8;
}

int MgArc::_getHandleCount() const
{
    return 4;
}

int MgArc::_getHandleType(int index) const
{
    if (index == 3) {
        return kMgHandleMidPoint;
    }
    if (index >= 4 && index <= 7) {
        return kMgHandleOutside;
    }
    if (index == 0) {
        return kMgHandleCenter;
    }
    return __super::_getHandleType(index);
}

bool MgArc::_isHandleFixed(int index) const
{
    return index > 2;
}

Point2d MgArc::_getHandlePoint(int index) const
{
    if (index == 1) {
        return getStartPoint();
    }
    if (index == 2) {
        return getEndPoint();
    }
    if (index == 3) {
        return getMidPoint();
    }
    if (index == 4) {
        return (getStartPoint() * 2 + getCenter()) / 3;
    }
    if (index == 5) {
        return (getEndPoint() * 2 + getCenter()) / 3;
    }
    if (index == 6) {
        return getStartPoint() + getStartTangent();
    }
    if (index == 7) {
        return getEndPoint() + getEndTangent();
    }
    return getCenter();
}

bool MgArc::_setHandlePoint2(int index, const Point2d& pt, float, int& data)
{
    static float lastSweepAngle;
    
    if (index == 1 || index == 2) {     // 起点、终点
        return setCenterRadius(getCenter(), pt.distanceTo(getCenter()), getStartAngle(), getSweepAngle());
    }
    if (index == 3) {                   // 弧线中点
        return setStartMidEnd(getStartPoint(), pt, getEndPoint());
    }
    if (index == 4) {                   // 改变起始角度
        if (data == 0) {
            lastSweepAngle = getSweepAngle();
            data++;
        }
        Point2d startPt(getCenter().polarPoint((pt - getCenter()).angle2(), getRadius()));
        bool ret = setCSE(getCenter(), startPt, getEndPoint(), lastSweepAngle);
        lastSweepAngle = getSweepAngle();
        return ret;
    }
    if (index == 5) {                   // 改变终止角度
        if (data == 0) {
            lastSweepAngle = getSweepAngle();
            data++;
        }
        Point2d endPt(getCenter().polarPoint((pt - getCenter()).angle2(), getRadius()));
        bool ret = setCSE(getCenter(), getStartPoint(), endPt, lastSweepAngle);
        lastSweepAngle = getSweepAngle();
        return ret;
    }
    if (index == 6) {
        return setTanStartEnd(pt - getStartPoint(), getStartPoint(), getEndPoint());
    }
    if (index == 7) {
        return (setTanStartEnd(getEndPoint() - pt, getEndPoint(), getStartPoint())
            && _reverse());
    }
    return offset(pt - getCenter(), -1);
}
