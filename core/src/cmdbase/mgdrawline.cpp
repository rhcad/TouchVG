// mgdrawlines.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawline.h"
#include "mgbasicsps.h"
#include "mgstorage.h"

bool MgCmdDrawLine::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgLine::Type(), sender);
    
    if (s) {
        MgLine *line = (MgLine*)dynshape()->shape();
        
        line->setBeeline(false);
        if (s->readBool("rayline", false))
            line->setRayline(true);
        if (s->readBool("beeline", false))
            line->setBeeline(true);
    }
    
    return _initialize(0, sender, s);
}

bool MgCmdDrawRayLine::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgLine::Type(), sender);
    
    ((MgLine*)dynshape()->shape())->setRayline(true);
    
    return _initialize(0, sender, s);
}

bool MgCmdDrawBeeLine::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgLine::Type(), sender);
    
    ((MgLine*)dynshape()->shape())->setBeeline(true);
    
    return _initialize(0, sender, s);
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
    ignoreStartPoint(sender, 0);
    dynshape()->shape()->setPoint(1, snapPoint(sender));
    dynshape()->shape()->update();

    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawLine::touchEnded(const MgMotion* sender)
{
    if ( ((MgLine*)dynshape()->shape())->length() > sender->displayMmToModel(2.f)) {
        addShape(sender);
    } else {
        sender->view->showMessage("@shape_too_small");
    }
    m_step = 0;

    return MgCommandDraw::touchEnded(sender);
}

// MgCmdDrawDot
//

bool MgCmdDrawDot::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgDot::Type(), sender);
    
    if (s) {
        ((MgDot*)dynshape()->shape())->setPointType(s->readInt("pttype", 0));
    }
    
    return _initialize(0, sender, s);
}

bool MgCmdDrawDot::click(const MgMotion* sender)
{
    return touchBegan(sender) && touchEnded(sender);
}

bool MgCmdDrawDot::touchBegan(const MgMotion* sender)
{
    m_step = 1;
    dynshape()->shape()->setPoint(0, snapPoint(sender, true));
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawDot::touchMoved(const MgMotion* sender)
{
    dynshape()->shape()->setPoint(0, snapPoint(sender));
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawDot::touchEnded(const MgMotion* sender)
{
    addShape(sender);
    m_step = 0;
    
    return MgCommandDraw::touchEnded(sender);
}
