//! \file mgbasicspreg.h
//! \brief 定义基本图形的工厂类 MgBasicShapes
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_BASICSHAPE_FACTORY_H_
#define TOUCHVG_BASICSHAPE_FACTORY_H_

struct MgShapeFactory;

//! 基本图形的工厂类
/*! \ingroup CORE_SHAPE
*/
class MgBasicShapes
{
public:
    //! 登记图形类型
    static void registerShapes(MgShapeFactory* factory);
};

#endif // TOUCHVG_BASICSHAPE_FACTORY_H_
