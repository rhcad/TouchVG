// mgdrawtriang.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawtriang.h"
#include "mgbasicsps.h"

bool MgCmdDrawTriangle::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgLines::Type(), sender, s);
}

bool MgCmdDrawTriangle::touchBegan(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    if (0 == m_step) {
        m_step = 1;
        lines->setClosed(true);
        lines->resize(3);
        Point2d pnt(snapPoint(sender, true));
        for (int i = 0; i < 3; i++) {
            dynshape()->shape()->setPoint(i, pnt);
        }
    }
    else {
        dynshape()->shape()->setPoint(m_step, snapPoint(sender));
    }
    
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawTriangle::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(m_step, snapPoint(sender));
    dynshape()->shape()->update();

    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawTriangle::touchEnded(const MgMotion* sender)
{
    float distmin = sender->displayMmToModel(2.f);
    Point2d lastpt(dynshape()->getPoint(m_step - 1));
    
    if (dynshape()->getPoint(m_step).distanceTo(lastpt) > distmin) {
        m_step++;
        if (3 == m_step) {
            addShape(sender);
            m_step = 0;
        }
    }

    return MgCommandDraw::touchEnded(sender);
}
