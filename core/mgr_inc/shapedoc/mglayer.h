//! \file mglayer.h
//! \brief 定义图形文档 MgShapeDoc
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_SHAPELAYER_H_
#define TOUCHVG_SHAPELAYER_H_

#include "mgshapes.h"

class MgShapeDoc;

//! 图层类
/*! \ingroup CORE_SHAPE
 */
class MgLayer : public MgShapes
{
public:
    //! 返回本对象的类型
    static int Type() { return 7; }
    
    //! 复制出一个新图层对象
    MgLayer* cloneLayer() const { return (MgLayer*)clone(); }
    
    //! 创建图层对象
    static MgLayer* create(MgShapeDoc* doc, int index);
    
    //! 返回所属文档对象
    MgShapeDoc* doc() const;
    
public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type(); }
    
protected:
    MgLayer(MgShapeDoc* doc, int index);
    virtual ~MgLayer();
    virtual bool saveExtra(MgStorage* s) const;
    virtual bool loadExtra(MgStorage* s);
};

#endif // TOUCHVG_SHAPELAYER_H_
