//! \file spfactoryimpl.h
//! \brief 定义图形工厂接口的默认实现类 MgShapeFactoryImpl
// Copyright (c) 2013, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_SHAPEFACTORY_IMPL_H_
#define TOUCHVG_SHAPEFACTORY_IMPL_H_

#include "mgspfactory.h"
#include <map>

//! 图形工厂接口的默认实现类
/*! \ingroup CORE_SHAPE
 */
class MgShapeFactoryImpl : public MgShapeFactory
{
    std::map<int, MgShape* (*)()>   _shapeCreators;
public:
    MgShapeFactoryImpl();
    virtual ~MgShapeFactoryImpl();
    
    virtual void registerShape(int type, MgShape* (*creator)());
    virtual MgShape* createShape(int type);
};

#endif // TOUCHVG_SHAPEFACTORY_IMPL_H_
