//! \file mgshapet.h
//! \brief 定义矢量图形模板类 MgShapeT
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPE_TEMPL_H_
#define TOUCHVG_MGSHAPE_TEMPL_H_

#include "mgshape.h"

//! 矢量图形模板类
/*! \ingroup CORE_SHAPE
    使用 MgShapeT<ShapeClass>::registerCreator() 登记图形种类;
 */
template <class ShapeT, class ContextT = GiContext>
class MgShapeT : public MgShape
{
    typedef MgShapeT<ShapeT, ContextT> ThisClass;
public:
    ShapeT      _shape;
    ContextT    _context;
    int         _id;
    MgShapes*   _parent;
    int         _tag;
    
    MgShapeT() : _id(0), _parent(NULL), _tag(0) {
    }
    
    MgShapeT(const ContextT& ctx) : _id(0), _parent(NULL), _tag(0) {
        _context = ctx;
    }
    
    virtual ~MgShapeT() {
    }

    GiContext* context() {
        return &_context;
    }
    
    const GiContext* contextc() const {
        return &_context;
    }
    
    MgBaseShape* shape() {
        return &_shape;
    }
    
    const MgBaseShape* shapec() const {
        return &_shape;
    }

#ifdef TOUCHVG_SHAPE_FACTORY_H_
    //! 登记类型号对应的图形创建函数
    static void registerCreator(MgShapeFactory* factory) {
        factory->registerShape(Type(), create);
    }
#endif
    
    static MgShape* create() {
        return new ThisClass;
    }
    
    static int Type() { return 0x10000 | ShapeT::Type(); }
    int getType() const { return Type(); }
    
    void release() {
        delete this;
    }
    
    MgObject* clone() const {
        ThisClass *p = new ThisClass;
        p->copy(*this);
        return p;
    }
    
    int getID() const {
        return _id;
    }
    
    MgShapes* getParent() const {
        return _parent;
    }
    
    void setParent(MgShapes* p, int sid) {
        _parent = p;
        _id = sid;
        shape()->setOwner(this);
    }

    int getTag() const {
        return _tag;
    }

    void setTag(int tag) {
        _tag = tag;
    }
};

#endif // TOUCHVG_MGSHAPE_TEMPL_H_
