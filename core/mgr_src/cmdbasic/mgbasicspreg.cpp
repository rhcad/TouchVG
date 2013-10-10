// mgbasicspreg.cpp: 实现基本图形的工厂类 MgBasicShapes
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicspreg.h"
#include <mgspfactory.h>
#include <mgshapet.h>
#include <mgcomposite.h>
#include <mggrid.h>

void MgBasicShapes::registerShapes(MgShapeFactory* factory)
{
    MgShapeT<MgGroup>::registerCreator(factory);
    MgShapeT<MgLine>::registerCreator(factory);
    MgShapeT<MgRect>::registerCreator(factory);
    MgShapeT<MgEllipse>::registerCreator(factory);
    MgShapeT<MgRoundRect>::registerCreator(factory);
    MgShapeT<MgDiamond>::registerCreator(factory);
    MgShapeT<MgParallel>::registerCreator(factory);
    MgShapeT<MgLines>::registerCreator(factory);
    MgShapeT<MgSplines>::registerCreator(factory);
    MgShapeT<MgGrid>::registerCreator(factory);
    MgShapeT<MgImageShape>::registerCreator(factory);
    MgShapeT<MgArc>::registerCreator(factory);
    MgShapeT<MgGrid>::registerCreator(factory);
}
