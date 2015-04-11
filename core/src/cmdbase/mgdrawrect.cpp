// mgdrawrect.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawrect.h"
#include "mgbasicsps.h"

bool MgCmdDrawRect::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgRect::Type(), sender, s);
}

bool MgCmdDrawRect::backStep(const MgMotion* sender)
{
    return MgCommandDraw::backStep(sender);
}

bool MgCmdDrawRect::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    m_startPt = snapPoint(sender, true);
    ((MgBaseRect*)dynshape()->shape())->setRect2P(m_startPt, m_startPt);
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawRect::touchMoved(const MgMotion* sender)
{
    Point2d pt1(m_startPt);
    Point2d pt2(snapPoint(sender));
    MgBaseRect* shape = (MgBaseRect*)dynshape()->shape();
    
    shape->setRect2P(pt1, pt2);
    dynshape()->shape()->update();

    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawRect::touchEnded(const MgMotion* sender)
{
    MgBaseRect* shape = (MgBaseRect*)dynshape()->shape();
    float minDist = sender->displayMmToModel(2.f);

    if (shape->getWidth() > minDist && shape->getHeight() > minDist
        && shape->getDiagonalLength() > 2 * minDist) {
        addRectShape(sender);
    } else if (sender->point.distanceTo(sender->startPt) < 2) {
        return _click(sender);
    } else {
        shape->clear();
        m_step = 0;
        sender->view->showMessage("@shape_too_small");
    }

    return MgCommandDraw::touchEnded(sender);
}

void MgCmdDrawRect::addRectShape(const MgMotion* sender)
{
    addShape(sender);
    m_step = 0;
}
