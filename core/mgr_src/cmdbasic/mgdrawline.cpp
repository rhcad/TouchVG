// mgdrawlines.cpp: 实现直线段绘图命令
// Author: pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawline.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawLine::MgCmdDrawLine()
{
}

MgCmdDrawLine::~MgCmdDrawLine()
{
}

bool MgCmdDrawLine::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgLine>::create, sender);
}

bool MgCmdDrawLine::backStep(const MgMotion* sender)
{
    return MgCommandDraw::backStep(sender);
}

bool MgCmdDrawLine::touchBegan(const MgMotion* sender)
{
    m_step = 1;

    Point2d pnt(snapPoint(sender, true));
    dynshape()->shape()->setPoint(0, pnt);
    dynshape()->shape()->setPoint(1, pnt);
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawLine::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(1, snapPoint(sender));
    dynshape()->shape()->update();

    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawLine::touchEnded(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(1, snapPoint(sender));
    dynshape()->shape()->update();

    if ( ((MgLine*)dynshape()->shape())->length() > sender->displayMmToModel(2.f)) {
        addShape(sender);
    }
    delayClear();

    return MgCommandDraw::touchEnded(sender);
}

bool MgCmdDrawFixedLine::initialize(const MgMotion* sender, MgStorage*)
{
    bool ret = _initialize(MgShapeT<MgLine>::create, sender);
    if (ret) {
        dynshape()->shape()->setFlag(kMgFixedLength, true);
    }
    return ret;
}
