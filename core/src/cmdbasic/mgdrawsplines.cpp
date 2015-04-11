// mgdrawsplines.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawsplines.h"
#include "mgshapet.h"
#include "mgbasicsps.h"

MgCmdDrawSplines::MgCmdDrawSplines(const char* name, bool freehand)
    : MgCommandDraw(name), m_freehand(freehand)
{
}

bool MgCmdDrawSplines::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgSplines::Type(), sender, s);
}

bool MgCmdDrawSplines::backStep(const MgMotion* sender)
{
    if (m_step > 1) {                   // freehand: 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)dynshape()->shape())->removePoint(m_freehand ? m_step - 1 : m_step);
        dynshape()->shape()->update();
    }
    
    return MgCommandDraw::backStep(sender);
}

bool MgCmdDrawSplines::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (getStep() > 0 && !m_freehand) {
        GiContext ctx(0, GiColor(64, 128, 64, 172), GiContext::kSolidLine, GiColor(0, 64, 64, 128));
        float radius = sender->displayMmToModel(0.8f, gs);
        
        for (int i = 1, n = dynshape()->getPointCount(); i < 6 && n >= i; i++) {
            gs->drawCircle(&ctx, dynshape()->getPoint(n - i), radius);
        }
        gs->drawCircle(&ctx, dynshape()->getPoint(0), radius * 1.5f);
    }
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawSplines::touchBegan(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    Point2d pnt(m_freehand ? sender->startPtM : snapPoint(sender, true));
    
    if (m_step > 0 && !m_freehand) {
        m_step++;
        if (m_step >= dynshape()->getPointCount()) {
            lines->addPoint(pnt);
            dynshape()->shape()->update();
        }
        
        return MgCommandDraw::touchMoved(sender);
    }
    else {
        lines->resize(m_freehand ? 1 : 2);
        lines->setClosed(false);
        m_step = 1;
        dynshape()->shape()->setPoint(0, pnt);
        if (!m_freehand)
            dynshape()->shape()->setPoint(1, pnt);
        dynshape()->shape()->update();
        
        return MgCommandDraw::touchBegan(sender);
    }
}

bool MgCmdDrawSplines::mouseHover(const MgMotion* sender)
{
    return !m_freehand && m_step > 0 && touchMoved(sender);
}

bool MgCmdDrawSplines::touchMoved(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    Point2d pnt(!m_freehand ? snapPoint(sender) : (sender->pointM + sender->lastPtM) / 2.f);
    
    if (m_freehand) {
        if (canAddPoint(sender, false)) {
            lines->addPoint(pnt);
            m_step++;
        }
    } else {
        dynshape()->shape()->setPoint(m_step, pnt);
        if (m_step > 0 && canAddPoint(sender, false)) {
            m_step++;
            if (m_step >= dynshape()->getPointCount()) {
                lines->addPoint(pnt);
            }
        }
    }
    dynshape()->shape()->update();
    
    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawSplines::touchEnded(const MgMotion* sender)
{
    MgSplines* lines = (MgSplines*)dynshape()->shape();
    
    if (m_freehand) {
        Tol tol(sender->displayMmToModel(1.f));
        if (m_step > 0 && !dynshape()->shape()->getExtent().isEmpty(tol, false)) {
            MgShape* newsp = addShape(sender);
            if (newsp) {/*
                m_step = 0;
                sender->view->regenAppend(0);
                newsp = newsp->cloneShape();
                lines = (MgSplines*)newsp->shape();
                lines->smooth(sender->view->xform()->modelToDisplay(),
                              sender->view->xform()->getWorldToDisplayY() * 0.5f);
                sender->view->shapes()->updateShape(newsp);
                sender->view->regenAppend(newsp->getID())*/
            }
        }
        else {
            click(sender);  // add a point
        }
        m_step = 0;
    }
    else {
        float dist = lines->endPoint().distanceTo(dynshape()->getPoint(0));

        while (m_step > 1 && dist < sender->displayMmToModel(1.f)) {
            lines->setClosed(true);
            lines->removePoint(m_step--);
            dist = lines->endPoint().distanceTo(dynshape()->getPoint(0));
        }
        if (m_step > 1 && lines->isClosed()) {
            addShape(sender);
            m_step = 0;
        }
    }
    
    return MgCommandDraw::touchEnded(sender);
}

bool MgCmdDrawSplines::cancel(const MgMotion* sender)
{
    if (!m_freehand && m_step > 1) {
        MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
        lines->removePoint(m_step--);
        addShape(sender);
    }
    return MgCommandDraw::cancel(sender);
}

bool MgCmdDrawSplines::canAddPoint(const MgMotion* sender, bool ended)
{
    if (!m_freehand && !ended)
        return false;
    
    if (m_step > 0) {
        MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
        float dist = sender->pointM.distanceTo(lines->endPoint());
        if (dist < sender->displayMmToModel(ended ? 0.3f : 0.5f))
            return false;
    }
    
    return true;
}

bool MgCmdDrawSplines::click(const MgMotion* sender)
{
    if (m_freehand) {
        MgShapeT<MgLine> line(*sender->view->context());
        Point2d pt (sender->pointM);
        
        if (sender->point.distanceTo(sender->startPt) < 1.f) {
            pt = (sender->point + Vector2d(1.f, 1.f)) * sender->view->xform()->displayToModel();
        }
        line.shape()->setPoint(0, sender->startPtM);
        line.shape()->setPoint(1, pt);
        
        MgShape* newsp = addShape(sender, &line);
        if (newsp) {
            dynshape()->shape()->clear();
            sender->view->regenAppend(newsp->getID());
        }
        
        return true;
    }
    
    return MgCommandDraw::click(sender);
}
