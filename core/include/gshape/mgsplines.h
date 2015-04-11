//! \file mgsplines.h
//! \brief 定义二次样条曲线类 MgSplines
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_SPLINES_SHAPE_H_
#define TOUCHVG_SPLINES_SHAPE_H_

#include "mglines.h"

//! 二次样条曲线类
/*! \ingroup CORE_SHAPE
 */
class MgSplines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgSplines, MgBaseLines, 16)
public:
    
    bool smooth(const Matrix2d& m2d, float tol);
    int smoothForPoints(int count, const Point2d* points, const Matrix2d& m2d, float tol);
    void clearVectors();
#ifndef SWIG
    const Vector2d* getVectors() const { return _knotvs; }
    virtual bool isCurve() const { return true; }
    virtual bool resize(int count);
    virtual bool addPoint(const Point2d& pt);
    virtual bool insertPoint(int segment, const Point2d& pt);
    virtual bool removePoint(int index);
#endif

protected:
    void _copy(const MgSplines& src);
    bool _equals(const MgSplines& src) const;
    void _transform(const Matrix2d& mat);
    void _clear();
    void _setPoint(int index, const Point2d& pt);
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
    Vector2d*   _knotvs;
};

#endif // TOUCHVG_SPLINES_SHAPE_H_
