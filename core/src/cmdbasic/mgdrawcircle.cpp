// mgdrawcircle.cpp: 实现圆绘图命令类 MgCmdDrawCircle
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawcircle.h"
#include "mgbasicsps.h"

bool MgCmdDrawCircle::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgEllipse::Type(), sender);
    
    ((MgBaseRect*)dynshape()->shape())->setSquare(true);
    
    return _initialize(0, sender, s);
}

bool MgCmdDrawCircle3P::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgEllipse::Type(), sender);
    
    ((MgBaseRect*)dynshape()->shape())->setSquare(true);
    
    return _initialize(0, sender, s);
}

void MgCmdDrawCircle3P::drawHandles(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0 && m_step < 3 && !getSnappedType(sender)) {
        gs->drawHandle(_points[m_step], kGiHandleHotVertex);
    }
}

bool MgCmdDrawCircle3P::draw(const MgMotion* sender, GiGraphics* gs)
{
    drawHandles(sender, gs);
    if (m_step > 0) {
        GiContext ctx(0, dynshape()->context().getLineColor(), GiContext::kDotLine);
        gs->drawLine(&ctx, _points[0], _points[1]);
    }
    return MgCommandDraw::draw(sender, gs);
}

void MgCmdDrawCircle3P::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    MgEllipse* c = (MgEllipse*)dynshape()->shape();
    
    if (step == 0) {
        _points[0] = pt;
    }
    else if (step == 1) {
        _points[1] = pt;
        _points[2] = pt;
        c->setCircle2P(_points[0], pt);
    }
    else if (step == 2) {
        _points[2] = pt;
        c->setCircle3P(_points[0], _points[1], pt);
    }
}
