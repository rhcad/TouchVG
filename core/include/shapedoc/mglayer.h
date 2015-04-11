//! \file mglayer.h
//! \brief 定义图形文档 MgShapeDoc
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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
    
    MgShapeDoc* doc() const;            //!< 返回所属文档对象
    
    bool isHided() const { return !!_bits.hided; }          //!< 返回图层是否隐藏
    void setHided(bool hided) { _bits.hided = hided; }      //!< 设置图层是否隐藏
    bool isLocked() const { return !!_bits.locked; }        //!< 返回图层是否锁定
    void setLocked(bool locked) { _bits.locked = locked; }  //!< 设置图层是否锁定
    
public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type() || type == MgShapes::Type(); }
    
protected:
    MgLayer(MgShapeDoc* doc, int index);
    virtual ~MgLayer();
    virtual bool saveExtra(MgStorage* s) const;
    virtual bool loadExtra(MgStorage* s);
    
private:
    union {
        int _flags;
        struct {
            int hided:1;
            int locked:1;
        } _bits;
    };
};

#endif // TOUCHVG_SHAPELAYER_H_
