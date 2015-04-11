//! \file mgellipse.h
//! \brief 定义椭圆图形类 MgEllipse
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_ELLIPSE_SHAPE_H_
#define TOUCHVG_ELLIPSE_SHAPE_H_

#include "mgrect.h"

//! 椭圆图形类
/*! \ingroup CORE_SHAPE
 */
class MgEllipse : public MgBaseRect
{
    MG_INHERIT_CREATE(MgEllipse, MgBaseRect, 12)
public:
    //! 返回X半轴长度
    float getRadiusX() const;
    
    //! 返回Y半轴长度
    float getRadiusY() const;
    
    //! 设置半轴长度
    void setRadius(float rx, float ry = 0.0);
    
    //! 设置圆心和半径
    bool setCircle(const Point2d& center, float radius);
    
    //! 给定直径画圆
    bool setCircle2P(const Point2d& start, const Point2d& end);
    
    //! 过三点画圆
    bool setCircle3P(const Point2d& start, const Point2d& point, const Point2d& end);
    
    //! 是否是圆
    bool isCircle() const { return getFlag(kMgSquare); }
    
    //! 是否是圆
    static bool isCircle(const MgBaseShape* sp) {
        return sp->isKindOf(Type()) && sp->getFlag(kMgSquare);
    }
    
    //! 求圆与圆、直线的交点(sp1和sp2至少一个是圆)
    static int crossCircle(Point2d& pt1, Point2d& pt2, const MgBaseShape* sp1,
                           const MgBaseShape* sp2, const Point2d& hitpt);
    
    //! 求圆(sp)与直线(pt1-pt2)的交点
    static int crossCircle(Point2d& pt1, Point2d& pt2, const MgBaseShape* sp);
    
#ifndef SWIG
    //! 返回Bezier顶点，13个点
    const Point2d* getBeziers() const { return _bzpts; }
    
    virtual bool isCurve() const { return true; }
#endif

protected:
    void _update();
    void _transform(const Matrix2d& mat);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    
protected:
    Point2d     _bzpts[13];
};

#endif // TOUCHVG_ELLIPSE_SHAPE_H_
