// mgcshapes.cpp: 实现基本图形的工厂类 MgCoreShapeFactory
// Copyright (c) 2004-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcshapes.h"
#include "mgshapetype.h"
#include "mgarc.h"
#include "mgdiamond.h"
#include "mgdot.h"
#include "mgellipse.h"
#include "mggrid.h"
#include "mgline.h"
#include "mglines.h"
#include "mgparallel.h"
#include "mgpathsp.h"
#include "mgrdrect.h"
#include "mgrect.h"
#include "mgsplines.h"
#include <map>

struct MgCoreShapeFactory::Impl
{
    std::map<int, MgBaseShape* (*)()> creators;
};


MgCoreShapeFactory::MgCoreShapeFactory() : impl(new Impl())
{
}

MgCoreShapeFactory::~MgCoreShapeFactory()
{
    delete impl;
}

void MgCoreShapeFactory::registerShape(int type, MgBaseShape* (*creator)())
{
    if (creator) {
        impl->creators[type] = creator;
    } else {
        impl->creators.erase(type);
    }
}

MgBaseShape* MgCoreShapeFactory::createShape(int type)
{
    switch (type) {
        case kMgShapeGrid:
            return MgGrid::create();
        case kMgShapeDot:
            return MgDot::create();
        case kMgShapeLine:
            return MgLine::create();
        case kMgShapeRect:
            return MgRect::create();
        case kMgShapeEllipse:
            return MgEllipse::create();
        case kMgShapeRoundRect:
            return MgRoundRect::create();
        case kMgShapeDiamond:
            return MgDiamond::create();
        case kMgShapeParallel:
            return MgParallel::create();
        case kMgShapeLines:
            return MgLines::create();
        case kMgShapeSplines:
            return MgSplines::create();
        case kMgShapeArc:
            return MgArc::create();
        case kMgShapePath:
            return MgPathShape::create();
        default:
            break;
    }
    
    std::map<int, MgBaseShape* (*)()>::const_iterator it = impl->creators.find(type);
    return it != impl->creators.end() ? (it->second)() : (MgBaseShape*)0;
}
