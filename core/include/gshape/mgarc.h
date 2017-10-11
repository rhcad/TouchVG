//! \file mgarc.h
//! \brief 定义圆弧图形类 MgArc
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_ARC_SHAPE_H_
#define TOUCHVG_ARC_SHAPE_H_

#include "mgbasesp.h"

//! 圆弧图形类
/*! \ingroup CORE_SHAPE
 */
class MgArc : public MgBaseShape
{
    MG_DECLARE_CREATE(MgArc, MgBaseShape, 19)
public:
    Point2d getCenter() const;
    Point2d getStartPoint() const;
    Point2d getEndPoint() const;
    Point2d getMidPoint() const;
    float getRadius() const;
    float getStartAngle() const;
    float getEndAngle() const;
    float getSweepAngle() const;
    Vector2d getStartTangent() const;
    Vector2d getEndTangent() const;
    
    bool setStartMidEnd(const Point2d& start, const Point2d& point, const Point2d& end);
    bool setCenterStartEnd(const Point2d& center, const Point2d& start);
    bool setCenterStartEnd(const Point2d& center, const Point2d& start, const Point2d& end);
    bool setTanStartEnd(const Vector2d& startTan, const Point2d& start, const Point2d& end);
    bool setCenterRadius(const Point2d& center, float radius, float startAngle, float sweepAngle);
    
    void setSubType(int type) { _subtype = type; };
    
#ifndef SWIG
    virtual bool isCurve() const { return true; }
    virtual int getSubType() const { return _subtype; }
#endif

protected:
    bool _isClosed() const;
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _isHandleFixed(int) const;
    bool _setHandlePoint2(int index, const Point2d& pt, float tol, int& data);
    bool _reverse();
    bool setCSE(const Point2d& center, const Point2d& start, const Point2d& end, float lastSweepAngle);
    
private:
    Point2d _points[4]; // center,start,end, mid
    float   _sweepAngle;
    int     _subtype;   // 0:arc, 1:pie
};

#endif // TOUCHVG_ARC_SHAPE_H_
