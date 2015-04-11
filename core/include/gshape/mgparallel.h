//! \file mgparallel.h
//! \brief 定义平行四边形图形类 MgParallel
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_PARALLEL_SHAPE_H_
#define TOUCHVG_PARALLEL_SHAPE_H_

#include "mgbasesp.h"

//! 平行四边形图形类
/*! \ingroup CORE_SHAPE
 */
class MgParallel : public MgBaseShape
{
    MG_DECLARE_CREATE(MgParallel, MgBaseShape, 17)
public:
    //! 返回中心点
    Point2d getCenter() const { return (_points[0] + _points[2]) / 2; }
    
    //! 返回矩形框，是本对象未旋转时的形状
    Box2d getRect() const { return Box2d(getCenter(), getWidth(), getHeight()); }
    
    //! 返回宽度
    float getWidth() const { return _points[0].distanceTo(_points[1]); }
    
    //! 返回高度
    float getHeight() const { return _points[2].distanceTo(_points[1]); }
    
    //! 返回角度, [-PI, PI)
    float angle() const { return (_points[2] - _points[3]).angleTo2(_points[0] - _points[3]); }
    
    //! 返回是否为空矩形
    bool isEmpty(float minDist) const {
        return getWidth() <= minDist || getHeight() <= minDist; }
    
#ifndef SWIG
    virtual const Point2d* getPoints() const { return _points; }
#endif

protected:
    bool _isClosed() const { return true; }
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    int _getHandleType(int index) const;
    bool _isHandleFixed(int index) const;
    bool _offset(const Vector2d& vec, int segment);
    bool _rotateHandlePoint(int index, const Point2d& pt);
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    Point2d     _points[4]; // 从左上角起顺时针的四个角点
};

#endif // TOUCHVG_PARALLEL_SHAPE_H_
