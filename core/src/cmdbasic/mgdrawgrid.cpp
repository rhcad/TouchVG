// mgdrawgrid.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawgrid.h"
#include "mgbasicsps.h"

bool MgCmdDrawGrid::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgGrid::Type(), sender, s);
}

bool MgCmdDrawGrid::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() == 2) {
        gs->drawHandle(dynshape()->getHandlePoint(8), kGiHandleVertex);
    }
    return MgCmdDrawRect::draw(sender, gs);
}

bool MgCmdDrawGrid::touchBegan(const MgMotion* sender)
{
    if (m_step == 0) {
        return MgCmdDrawRect::touchBegan(sender);
    }
    m_step = 3;
    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawGrid::touchMoved(const MgMotion* sender)
{
    if (m_step == 1) {
        return MgCmdDrawRect::touchMoved(sender);
    }
    
    dynshape()->shape()->setHandlePoint(8, snapPoint(sender), 0);
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawGrid::touchEnded(const MgMotion* sender)
{
    if (m_step == 1) {
        return MgCmdDrawRect::touchEnded(sender);
    }
    
    if ( ((MgGrid*)dynshape()->shape())->isValid(sender->displayMmToModel(1.f)) ) {
        dynshape()->setContext(GiContext(), GiContext::kFillARGB);
        addShape(sender);
        m_step = 0;
        sender->view->toSelectCommand();
    }
    else {
        dynshape()->shape()->setHandlePoint(8, dynshape()->getPoint(3), 0);
        m_step = 2;
        sender->view->showMessage("@invalid_gridcell");
    }
    
    return MgCommandDraw::touchEnded(sender);
}

void MgCmdDrawGrid::addRectShape(const MgMotion* sender)
{
    m_step = 2;
    sender->view->redraw();
}
