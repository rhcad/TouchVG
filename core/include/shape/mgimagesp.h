//! \file mgimagesp.h
//! \brief 定义图像矩形类 MgImageShape
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_IMAGE_SHAPE_H_
#define TOUCHVG_IMAGE_SHAPE_H_

#include "mgrect.h"
#include "mgshape.h"

//! 图像矩形类
/*! \ingroup CORE_SHAPE
 */
class MgImageShape : public MgBaseRect
{
    MG_INHERIT_CREATE(MgImageShape, MgBaseRect, 18)
public:
#ifndef SWIG
    const char* getName() const { return _name; }
#endif
    void setName(const char* name);
    
    Vector2d getImageSize() const { return _size; }
    void setImageSize(Vector2d size);
    
    static const MgShape* findShapeByImageID(const MgShapes* shapes, const char* name);
    
protected:
    void _copy(const MgImageShape& src);
    bool _equals(const MgImageShape& src) const;
    void _clear();
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    char    _name[64];
    Vector2d _size;
};

#endif // TOUCHVG_IMAGE_SHAPE_H_
