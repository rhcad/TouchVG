//! \file spfactoryimpl.cpp
//! \brief 实现图形工厂接口的默认实现类 MgShapeFactoryImpl
// Copyright (c) 2013, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "spfactoryimpl.h"
#include "mgbasicspreg.h"
#include "mgshape.h"

MgShapeFactoryImpl::MgShapeFactoryImpl()
{
    MgBasicShapes::registerShapes(this);
}

MgShapeFactoryImpl::~MgShapeFactoryImpl()
{
}

void MgShapeFactoryImpl::registerShape(int type, MgShape* (*creator)())
{
    type = type & 0xFFFF;
    if (creator) {
        _shapeCreators[type] = creator;
    } else {
        _shapeCreators.erase(type);
    }
}

MgShape* MgShapeFactoryImpl::createShape(int type)
{
    std::map<int, MgShape* (*)()>::const_iterator it = _shapeCreators.find(type & 0xFFFF);
    return it != _shapeCreators.end() ? (it->second)() : MgShape::Null();
}
