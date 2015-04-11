//! \file mgcshapes.h
//! \brief 定义基本图形的工厂类 MgCoreShapeFactory
// Copyright (c) 2004-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORESHAPE_FACTORY_H_
#define TOUCHVG_CORESHAPE_FACTORY_H_

class MgBaseShape;

//! 基本图形的工厂类
/*! \ingroup CORE_SHAPE
*/
class MgCoreShapeFactory
{
public:
    MgCoreShapeFactory();
    ~MgCoreShapeFactory();
    
    //! 根据指定的图形类型创建图形对象
    MgBaseShape* createShape(int type);
    
#ifndef SWIG
    //! 登记类型号对应的图形创建函数，creator为NULL则取消登记
    void registerShape(int type, MgBaseShape* (*creator)());
#endif
    
private:
    struct Impl;
    Impl*   impl;
};

#endif // TOUCHVG_CORESHAPE_FACTORY_H_
