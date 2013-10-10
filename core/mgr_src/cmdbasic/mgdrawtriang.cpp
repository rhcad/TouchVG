// mgdrawtriang.cpp: 实现三角形绘图命令 MgCmdDrawTriangle
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawtriang.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawTriangle::MgCmdDrawTriangle()
{
}

MgCmdDrawTriangle::~MgCmdDrawTriangle()
{
}

bool MgCmdDrawTriangle::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgLines>::create, sender);
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
    Point2d pnt(snapPoint(sender));
    float distmin = sender->displayMmToModel(2.f);
    
    dynshape()->shape()->setPoint(m_step, pnt);
    dynshape()->shape()->update();
    
    if (pnt.distanceTo(dynshape()->shape()->getPoint(m_step - 1)) > distmin) {
        m_step++;
        if (3 == m_step) {
            addShape(sender);
            delayClear();
            m_step = 0;
        }
    }

    return MgCommandDraw::touchEnded(sender);
}
