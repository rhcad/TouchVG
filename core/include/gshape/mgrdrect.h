//! \file mgrdrect.h
//! \brief 定义圆角矩形类 MgRoundRect
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_ROUNDRECT_SHAPE_H_
#define TOUCHVG_ROUNDRECT_SHAPE_H_

#include "mgrect.h"

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
    
#ifndef SWIG
    virtual bool isCurve() const;
#endif

protected:
    void _copy(const MgRoundRect& src);
    bool _equals(const MgRoundRect& src) const;
    void _clear();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    float      _rx;
    float      _ry;
};

#endif // TOUCHVG_ROUNDRECT_SHAPE_H_
