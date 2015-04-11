// mgdrawellipse.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawellipse.h"
#include "mgbasicsps.h"

bool MgCmdDrawEllipse::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgEllipse::Type(), sender, s);
}

bool MgCmdDrawEllipse::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() > 0 && sender->dragging()) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), GiContext::kDashLine);
        gs->drawRect(&ctxshap, dynshape()->shape()->getExtent());
    }
    return MgCmdDrawRect::draw(sender, gs);
}
