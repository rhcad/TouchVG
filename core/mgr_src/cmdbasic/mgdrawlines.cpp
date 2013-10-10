// mgdrawlines.cpp: 实现折线绘图命令类 MgCmdDrawLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawlines.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

MgCmdDrawLines::MgCmdDrawLines()
{
}

MgCmdDrawLines::~MgCmdDrawLines()
{
}

bool MgCmdDrawLines::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgLines>::create, sender);
}

bool MgCmdDrawLines::backStep(const MgMotion* sender)
{
    if (m_step > 2) {                   // 去掉倒数第二个点，倒数第一点是临时动态点
        ((MgBaseLines*)dynshape()->shape())->removePoint(
            m_index == m_step ? m_step - 1 : m_index);
        dynshape()->shape()->update();
    }
    return MgCommandDraw::backStep(sender);
}

bool MgCmdDrawLines::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > (needEnded() ? 3 : 2) && !sender->dragging()) {
        gs->drawHandle(dynshape()->shapec()->getExtent().center(), 6);
    }
    return MgCommandDraw::draw(sender, gs);
}

bool MgCmdDrawLines::click(const MgMotion* sender)
{
    if (m_step > (needEnded() ? 3 : 2)
        && sender->pointM.distanceTo(dynshape()->shapec()->getExtent().center())
        < sender->displayMmToModel(5.f)) {
        return cancel(sender);
    }
    return MgCommandDraw::click(sender);
}

static bool _lastClicked = false;

bool MgCmdDrawLines::touchBegan(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender, true));
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    if (0 == m_step) {
        m_step = 1;
        m_index = 1;
        lines->resize(2);
        dynshape()->shape()->setPoint(0, pnt);
        dynshape()->shape()->setPoint(1, pnt);
    }
    else {
        if (m_step >= dynshape()->shape()->getPointCount()) {
            if (m_step > 2) {
                Point2d nearpt;
                int segment, hitType = 0;
                bool inside;
                
                mgnear::linesHit(lines->getPointCount(),
                    lines->getPoints(), lines->isClosed(), sender->pointM,
                    sender->displayMmToModel(5.f),
                    nearpt, segment, &inside, &hitType);
                if (hitType == mglnrel::kPtOnEdge) {
                    lines->insertPoint(segment, pnt);
                    m_index = segment + 1;
                }
                else if (hitType == mglnrel::kPtAtVertex) {
                    m_index = segment;
                }
                else {
                    lines->addPoint(pnt);
                    m_step = mgMin(m_step, lines->getPointCount() - 1);
                    m_index = m_step;
                }
            }
            if (m_step <= 2) {
                lines->addPoint(pnt);
                m_index = m_step;
            }
        }
        dynshape()->shape()->setPoint(m_index, pnt);
    }
    dynshape()->shape()->update();
    _lastClicked = true;
    
    return MgCommandDraw::touchBegan(sender);
}

bool MgCmdDrawLines::touchMoved(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    dynshape()->shape()->setPoint(m_index, pnt);
    
    checkClosed(sender, pnt);
    
    dynshape()->shape()->update();
    _lastClicked = false;
    
    return MgCommandDraw::touchMoved(sender);
}

bool MgCmdDrawLines::touchEnded(const MgMotion* sender)
{
    Point2d pnt(snapPoint(sender));
    dynshape()->shape()->setPoint(m_index, pnt);
    
    bool closed = checkClosed(sender, pnt);
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    dynshape()->shape()->update();
    
    if (canAddPoint(sender, pnt)) {
        if (closed || needEnded()) {
            if (closed) {
                lines->removePoint(m_step);
            }
            addShape(sender);
            delayClear();
            m_step = 0;
            _lastClicked = false;
        }
        else if (m_step <= dynshape()->shape()->getPointCount()) {
            m_step++;
        }
    }
    else if (m_step > 1) {
        if (m_step >= dynshape()->shape()->getPointCount()) {
            m_step--;
        }
        lines->removePoint(m_index);
    }
    
    return MgCommandDraw::touchEnded(sender);
}

bool MgCmdDrawLines::checkClosed(const MgMotion* sender, const Point2d& pnt)
{
    bool closed = false;
    
    if (m_index == m_step && needCheckClosed()) {
        float distmin = sender->displayMmToModel(2.f);
        closed = m_step > 2 && pnt.distanceTo(dynshape()->shape()->getPoint(0)) < distmin;
        ((MgBaseLines*)dynshape()->shape())->setClosed(closed);
    }
    
    return closed;
}

bool MgCmdDrawLines::canAddPoint(const MgMotion* sender, const Point2d& pnt)
{
    float distmin = sender->displayMmToModel(3.f);
    int maxIndex = ((MgBaseLines*)dynshape()->shape())->maxEdgeIndex();
    Point2d prevPt(dynshape()->shape()->getPoint(m_index - 1));
    
    if (prevPt.distanceTo(pnt) < distmin) {
        return false;
    }
    if (m_index < maxIndex) {
        if (dynshape()->shape()->getPoint(m_index + 1).distanceTo(pnt) < distmin) {
            return false;
        }
    }
    if (dynshape()->shape()->isClosed() || m_index < maxIndex) {
        int n = dynshape()->shape()->getPointCount();
        Point2d nextPt(dynshape()->shape()->getPoint( (m_index + 1) % n) );
        Point2d nearpt;
        
        if (mglnrel::ptToLine(prevPt, nextPt, pnt, nearpt)
            < sender->displayMmToModel(1.f)) {
            return false;
        }
    }
    
    return true;
}

bool MgCmdDrawLines::doubleClick(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    Point2d pnt(_lastClicked ? lines->getPoint(m_index) : sender->pointM);
    
    if (m_step > (lines->isClosed() ? 2 : 1)) {
        if (lines->getPointCount() > (lines->isClosed() ? 2 : 1)
               && sender->displayMmToModel(5.f) > pnt.distanceTo(lines->getPoint(m_index))) {
            lines->removePoint(m_index--);
        }
        addShape(sender);
        delayClear();
        m_step = 0;
    }
    return true;
}

bool MgCmdDrawLines::cancel(const MgMotion* sender)
{
    MgBaseLines* lines = (MgBaseLines*)dynshape()->shape();
    
    if (m_step > (lines->isClosed() ? 2 : 1)) {
        addShape(sender);
        delayClear();
        m_step = 0;
        return true;
    }
    
    return MgCommandDraw::cancel(sender);
}
