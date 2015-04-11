//! \file mgline.h
//! \brief 定义线段图形类 MgLine
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_LINE_SHAPE_H_
#define TOUCHVG_LINE_SHAPE_H_

#include "mgbasesp.h"

//! 线段图形类
/*! \ingroup CORE_SHAPE
 */
class MgLine : public MgBaseShape
{
    MG_DECLARE_CREATE(MgLine, MgBaseShape, 10)
public:
    //! 返回起点
    const Point2d& startPoint() const { return _points[0]; }
    
    //! 返回终点
    const Point2d& endPoint() const { return _points[1]; }
    
    //! 返回终点
    Point2d center() const { return (_points[0] + _points[1]) / 2; }
    
    //! 返回线段长度
    float length() const { return _points[0].distanceTo(_points[1]); }
    
    //! 返回线段角度, [-PI, PI)
    float angle() const { return (_points[1] - _points[0]).angle2(); }
    
    //! 设置起点，未调 update()
    void setStartPoint(const Point2d& pt) { _points[0] = pt; }
    
    //! 设置终点，未调 update()
    void setEndPoint(const Point2d& pt)  { _points[1] = pt; }
    
    //! 设置是否为射线
    void setRayline(bool ray) { _subtype = ray ? 1 : 0; }
    
    //! 设置是否为无穷直线
    void setBeeline(bool bee) { _subtype = bee ? 2 : 0; }
    
    //! 返回是否为射线
    bool isRayline() const { return _subtype == 1; }
    
    //! 返回是否为无穷直线
    bool isBeeline() const { return _subtype == 2; }
    
#ifndef SWIG
    virtual int getSubType() const { return _subtype; }
    virtual const Point2d* getPoints() const { return _points; }
#endif
    
protected:
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _isHandleFixed(int index) const;
    int _getHandleType(int index) const;
    
private:
    Point2d     _points[2];
    int         _subtype;       // 0：线段，1：射线，2：无穷直线
};

#endif // TOUCHVG_LINE_SHAPE_H_
