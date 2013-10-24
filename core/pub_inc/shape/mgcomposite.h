//! \file mgcomposite.h
//! \brief 定义复合图形基类 MgComposite
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_COMPOSITE_SHAPE_H_
#define TOUCHVG_COMPOSITE_SHAPE_H_

#include "mgshapes.h"

//! 复合图形基类
/*! \ingroup CORE_SHAPE
*/
class MgComposite : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgComposite, MgBaseShape)
public:
    //! 返回本对象的类型
    static int Type() { return 6; }

    //! 返回图形总数
    int getShapeCount() const;

    //! 返回子图形列表
    MgShapes* shapes() const { return _shapes; }

    //! 返回是否可以单独移动一个子图形，在 offset() 中调用
    virtual bool canOffsetShapeAlone(MgShape* shape) { return !!shape; }
    
    virtual bool isCurve() const { return true; }

protected:
    MgComposite();
    virtual ~MgComposite();

    void _copy(const MgComposite& src);
    bool _equals(const MgComposite& src) const;
    bool _isKindOf(int type) const;
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _isHandleFixed(int index) const;
    void _update();
    void _clearCachedData();
    void _transform(const Matrix2d& mat);
    void _clear();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _offset(const Vector2d& vec, int segment);
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;

protected:
    MgShapes*   _shapes;
};

//! 成组图形类
/*! \ingroup CORE_SHAPE
*/
class MgGroup : public MgComposite
{
    MG_INHERIT_CREATE(MgGroup, MgComposite, 9)
public:
    //! 添加一个新图形或其他图形列表中的图形
    bool addShape(MgShape* shape);

protected:
    bool _offset(const Vector2d& vec, int segment);
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
};

#endif // TOUCHVG_COMPOSITE_SHAPE_H_
