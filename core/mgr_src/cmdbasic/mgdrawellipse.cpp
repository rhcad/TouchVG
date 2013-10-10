// mgdrawellipse.cpp: 实现椭圆绘图命令类 MgCmdDrawEllipse
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawellipse.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdDrawEllipse::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgEllipse>::create, sender);
}

bool MgCmdDrawEllipse::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() > 0 && sender->dragging()) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
        gs->drawRect(&ctxshap, dynshape()->shape()->getExtent());
    }
    return MgCmdDrawRect::draw(sender, gs);
}
