//! \file mgrect.h
//! \brief 定义矩形图形基类 MgBaseRect
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_BASERECT_SHAPE_H_
#define TOUCHVG_BASERECT_SHAPE_H_

#include "mgbasesp.h"

//! 矩形图形基类
/*! \ingroup CORE_SHAPE
 */
class MgBaseRect : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgBaseRect, MgBaseShape)
public:
    //! 返回本对象的类型
    static int Type() { return 4; }
    
    //! 返回中心点
    Point2d getCenter() const;
    
    //! 返回矩形框，是本对象未旋转时的形状
    Box2d getRect() const;
    
    //! 返回宽度
    float getWidth() const;
    
    //! 返回高度
    float getHeight() const;
    
    //! 返回对角线长度
    float getDiagonalLength() const;
    
    //! 返回倾斜角度
    float getAngle() const;
    
    //! 返回是否为空矩形
    bool isEmpty(float minDist) const;
    
    //! 返回是否为水平矩形
    bool isOrtho() const;
    
    //! 设置水平矩形，对于正方形pt1固定
    void setRect2P(const Point2d& pt1, const Point2d& pt2);
    
    //! 设置倾斜矩形，(pt1-pt2)为旋转前的对角点，basept为旋转中心
    void setRectWithAngle(const Point2d& pt1, const Point2d& pt2,
                          float angle, const Point2d& basept);
    
    //! 设置四个角点
    void setRect4P(const Point2d points[4]);
    
    //! 给定两个相邻角点的位置进行几何变形
    bool transformWith2P(const Point2d& pt1, int i1, const Point2d& pt2, int i2);
    
    //! 设置中心点
    void setCenter(const Point2d& pt);
    
    //! 设置是否为方形
    void setSquare(bool square) { setFlag(kMgSquare, square); }
    
#ifndef SWIG
    virtual int getSubType() const { return getFlag(kMgSquare) ? 1 : 0; }
    virtual const Point2d* getPoints() const { return _points; }
#endif

protected:
    MgBaseRect();
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    void _copy(const MgBaseRect& src);
    bool _equals(const MgBaseRect& src) const;
    bool _isKindOf(int type) const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    bool _isClosed() const { return true; }
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    Point2d     _points[4]; // 从左上角起顺时针的四个角点
};

//! 矩形图形类
/*! \ingroup CORE_SHAPE
 */
class MgRect : public MgBaseRect
{
    MG_INHERIT_CREATE(MgRect, MgBaseRect, 11)
};

#endif // TOUCHVG_BASERECT_SHAPE_H_
