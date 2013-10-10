// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmddraw.h"
#include <mgsnap.h>
#include <mgaction.h>
#include <cmdsubject.h>
#include <string.h>
#include <mglog.h>

MgCommandDraw::MgCommandDraw() : m_step(0), m_shape(NULL), m_needClear(false)
{
}

MgCommandDraw::~MgCommandDraw()
{
    if (m_shape) {
        m_shape->release();
        m_shape = NULL;
    }
}

bool MgCommandDraw::cancel(const MgMotion* sender)
{
    if (m_step > 0) {
        m_step = 0;
        m_shape->shape()->clear();
        sender->view->getSnap()->clearSnap();
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::_initialize(MgShape* (*creator)(), const MgMotion* sender)
{
    if (!m_shape) {
        m_shape = creator ? creator() : createShape(sender->view->getShapeFactory());
        if (!m_shape || !m_shape->shape())
            return false;
        m_shape->setParent(sender->view->shapes(), 0);
    }
    sender->view->setNewShapeID(0);
    m_step = 0;
    m_needClear = false;
    m_shape->shape()->clear();
    *m_shape->context() = *sender->view->context();
    
    return true;
}

MgShape* MgCommandDraw::addShape(const MgMotion* sender, MgShape* shape, bool autolock)
{
    MgShapesLock locker(MgShapesLock::Add, autolock ? sender->view : NULL);
    shape = shape ? shape : m_shape;
    MgShape* newsp = NULL;
    
    if ((locker.locked() || !autolock) && sender->view->shapeWillAdded(shape)) {
        newsp = sender->view->shapes()->addShape(*shape);
        sender->view->shapeAdded(newsp);
        sender->view->getCmdSubject()->onShapeAdded(sender, newsp);
        if (strcmp(getName(), "splines") != 0) {
            sender->view->setNewShapeID(newsp->getID());
        }
    }
    if (m_shape && sender->view->context()) {
        *m_shape->context() = *sender->view->context();
    }
    
    return newsp;
}

bool MgCommandDraw::backStep(const MgMotion* sender)
{
    if (m_step > 1) {
        m_step--;
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_needClear) {
        m_needClear = false;
        m_step = 0;
        m_shape->shape()->clear();
    }
    bool ret = m_step > 0 && m_shape->draw(0, *gs, NULL, -1);
    return sender->view->getSnap()->drawSnap(sender, gs) || ret;
}

int MgCommandDraw::gatherShapes(const MgMotion* /*sender*/, MgShapes* shapes)
{
    return (m_step > 0 && m_shape && shapes->addShape(*m_shape)) ? 1 : 0;
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    return (m_step == 0 ? _click(sender)
            : touchBegan(sender) && touchEnded(sender));
}

bool MgCommandDraw::_click(const MgMotion* sender)
{
    Box2d limits(sender->pointM, sender->displayMmToModel(10.f), 0);
    MgHitResult res;
    MgShape* shape = sender->view->shapes()->hitTest(limits, res);
    
    if (shape) {
        sender->view->setNewShapeID(shape->getID());
        sender->cancel();
        LOGD("Command (%s) cancelled after the shape #%d clicked.", getName(), shape->getID());
    }
    
    return shape || (sender->view->useFinger() && longPress(sender));
}

bool MgCommandDraw::longPress(const MgMotion* sender)
{
    return sender->view->getAction()->showInDrawing(sender, m_shape);
}

bool MgCommandDraw::touchBegan(const MgMotion* sender)
{
    *m_shape->context() = *sender->view->context();
    sender->view->redraw();
    m_needClear = false;
    
    return true;
}

bool MgCommandDraw::touchMoved(const MgMotion* sender)
{
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::touchEnded(const MgMotion* sender)
{
    sender->view->getSnap()->clearSnap();
    sender->view->redraw();
    return true;
}

void MgCommandDraw::delayClear()
{
    m_step = 0;
    m_needClear = true;
}

bool MgCommandDraw::mouseHover(const MgMotion* sender)
{
    return m_step > 0 && touchMoved(sender);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, bool firstStep)
{
    return snapPoint(sender, sender->pointM, firstStep);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, 
                                 const Point2d& orignPt, bool firstStep)
{
    return sender->view->getSnap()->snapPoint(sender, orignPt,
        firstStep ? NULL : m_shape, m_step);
}

void MgCommandDraw::setStepPoint(int step, const Point2d& pt)
{
    if (step > 0) {
        dynshape()->shape()->setHandlePoint(step, pt, 0);
    }
}

bool MgCommandDraw::touchBeganStep(const MgMotion* sender)
{
    if (0 == m_step) {
        m_step = 1;
        Point2d pnt(snapPoint(sender, true));
        for (int i = dynshape()->shape()->getPointCount() - 1; i >= 0; i--) {
            dynshape()->shape()->setPoint(i, pnt);
        }
        setStepPoint(0, pnt);
    }
    else {
        setStepPoint(m_step, snapPoint(sender));
    }
    dynshape()->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCommandDraw::touchMovedStep(const MgMotion* sender)
{
    if (sender->dragging()) {
        setStepPoint(m_step, snapPoint(sender));
        dynshape()->shape()->update();
    }
    return MgCommandDraw::touchMoved(sender);
}

bool MgCommandDraw::touchEndedStep(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    Tol tol(sender->displayMmToModel(2.f));
    
    setStepPoint(m_step, pnt);
    dynshape()->shape()->update();
    
    if (!pnt.isEqualTo(dynshape()->shape()->getPoint(m_step - 1), tol)) {
        m_step++;
        if (m_step >= getMaxStep()) {
            if (!dynshape()->shape()->getExtent().isEmpty(tol, false)) {
                addShape(sender);
                delayClear();
            }
            m_step = 0;
        }
    }

    return MgCommandDraw::touchEnded(sender);
}
