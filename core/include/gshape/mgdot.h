//! \file mgdot.h
//! \brief 定义点图形类 MgDot
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_DOT_SHAPE_H_
#define TOUCHVG_DOT_SHAPE_H_

#include "mgbasesp.h"

//! 点图形类
/*! \ingroup CORE_SHAPE
 */
class MgDot : public MgBaseShape
{
    MG_DECLARE_CREATE(MgDot, MgBaseShape, 31)
public:
    //! 返回点图案类型
    int getPointType() const { return _type; }
    
    //! 设置点图案类型. 0:默认, 1-19: GiHandleTypes+1, kGiHandleCustom~99:应用自定义图片
    void setPointType(int type) { _type = type; }

protected:
    bool _isClosed() const { return false; }
    void _output(MgPath& path) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
private:
    Point2d     _point;
    int         _type;
};

#endif // TOUCHVG_DOT_SHAPE_H_
