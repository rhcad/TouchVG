//! \file mgspfactory.h
//! \brief 定义图形工厂接口 MgShapeFactory
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_SHAPE_FACTORY_H_
#define TOUCHVG_SHAPE_FACTORY_H_

class MgShape;

//! 图形工厂接口
/*! \ingroup CORE_SHAPE
    \interface MgShapeFactory
*/
struct MgShapeFactory
{
    virtual ~MgShapeFactory() {}
#ifndef SWIG
    //! 登记类型号对应的图形创建函数，creator为NULL则取消登记
    virtual void registerShape(int type, MgShape* (*creator)()) = 0;
#endif
    //! 根据类型号创建图形对象
    virtual MgShape* createShape(int type) = 0;
};

#endif // TOUCHVG_SHAPE_FACTORY_H_
