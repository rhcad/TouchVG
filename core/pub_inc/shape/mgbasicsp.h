//! \file mgbasicsp.h
//! \brief 定义基本图形类
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_BASICSHAPE_H_
#define TOUCHVG_BASICSHAPE_H_

#include "mgshape.h"

#ifndef SWIG
#include "mgbase.h"
#include "mgnear.h"
#include "mgcurv.h"
#include "mglnrel.h"
#endif

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
    
    virtual bool isCurve() const { return false; }

protected:
    bool _hitTestBox(const Box2d& rect) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _isHandleFixed(int index) const;
    int _getHandleType(int index) const;

private:
    Point2d     _points[2];
};

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

    //! 设置中心点
    void setCenter(const Point2d& pt);
    
    //! 设置是否为方形
    void setSquare(bool square) { setFlag(kMgSquare, square); }

    virtual bool isCurve() const { return false; }

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

    virtual bool isCurve() const { return true; }

protected:
    void _update();
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;

protected:
    Point2d     _bzpts[13];
};

//! 圆角矩形类
/*! \ingroup CORE_SHAPE
*/
class MgRoundRect : public MgBaseRect
{
    MG_INHERIT_CREATE(MgRoundRect, MgBaseRect, 13)
public:
    //! 返回X圆角半径
    float getRadiusX() const { return _rx; }

    //! 返回Y圆角半径
    float getRadiusY() const { return _ry; }

    //! 设置圆角半径
    void setRadius(float rx, float ry = 0.0);

    virtual bool isCurve() const;

protected:
    void _copy(const MgRoundRect& src);
    bool _equals(const MgRoundRect& src) const;
    void _clear();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);

protected:
    float      _rx;
    float      _ry;
};

//! 菱形图形类
/*! \ingroup CORE_SHAPE
*/
class MgDiamond : public MgBaseRect
{
    MG_INHERIT_CREATE(MgDiamond, MgBaseRect, 14)
protected:
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    int _getHandleType(int index) const;
    bool _isHandleFixed(int index) const;
    void _update();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _rotateHandlePoint(int index, const Point2d& pt);
};

//! 折线基类
/*! \ingroup CORE_SHAPE
*/
class MgBaseLines : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgBaseLines, MgBaseShape)
public:
    //! 返回本对象的类型
    static int Type() { return 5; }

    //! 设置是否闭合
    void setClosed(bool closed) { setFlag(kMgClosed, closed); }

    //! 返回终点
    Point2d endPoint() const;

    //! 改变顶点数
    bool resize(int count);

    //! 添加一个顶点
    bool addPoint(const Point2d& pt);
    
    //! 在指定段插入一个顶点
    bool insertPoint(int segment, const Point2d& pt);

    //! 删除一个顶点
    bool removePoint(int index);

    //! 返回边的最大序号
    int maxEdgeIndex() const;
    
#ifndef SWIG
    //! 返回坐标数组
    Point2d* getPoints() { return _points; }
#endif

protected:
    MgBaseLines();
    virtual ~MgBaseLines();
    bool _isClosed() const { return _count > 2 && __super::_isClosed(); }
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    void _copy(const MgBaseLines& src);
    bool _equals(const MgBaseLines& src) const;
    bool _isKindOf(int type) const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);

protected:
    Point2d*    _points;
    int      _maxCount;
    int      _count;
};

//! 折线图形类
/*! \ingroup CORE_SHAPE
*/
class MgLines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgLines, MgBaseLines, 15)
public:
    virtual bool isCurve() const { return false; }
protected:
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _isHandleFixed(int index) const;
};

//! 二次样条曲线类
/*! \ingroup CORE_SHAPE
*/
class MgSplines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgSplines, MgBaseLines, 16)
public:
    virtual bool isCurve() const { return true; }

protected:
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
};

//! 平行四边形图形基类
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

    virtual bool isCurve() const { return false; }

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
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);

protected:
    Point2d     _points[4]; // 从左上角起顺时针的四个角点
};

//! 图像矩形类
/*! \ingroup CORE_SHAPE
 */
class MgImageShape : public MgBaseRect
{
    MG_INHERIT_CREATE(MgImageShape, MgBaseRect, 18)
public:
    const char* getName() const { return _name; }
    void setName(const char* name);
    
    static MgShape* findShapeByImageID(MgShapes* shapes, const char* name);
    
protected:
    void _copy(const MgImageShape& src);
    bool _equals(const MgImageShape& src) const;
    void _clear();
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    char    _name[64];
};

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
    
    virtual bool isCurve() const { return true; }

protected:
    bool _isClosed() const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint2(int index, const Point2d& pt, float tol, int& data);
    bool _reverse();
    bool setCSE(const Point2d& center, const Point2d& start, const Point2d& end, float lastSweepAngle);

private:
    Point2d _points[4]; // center,start,end, mid
};

#endif // TOUCHVG_BASICSHAPE_H_
