//! \file mgdiamond.h
//! \brief 定义菱形图形类 MgDiamond
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_DIAMOND_SHAPE_H_
#define TOUCHVG_DIAMOND_SHAPE_H_

#include "mgrect.h"

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
    void _output(MgPath& path) const;
};

#endif // TOUCHVG_DIAMOND_SHAPE_H_
