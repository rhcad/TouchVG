// mgdrawarc.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawarc.h"
#include "mgbasicsps.h"
#include "mgstorage.h"
#include "mglocal.h"
#include "mgsnap.h"
#include <sstream>

// MgCmdArc3P
//

bool MgCmdArc3P::initialize(const MgMotion* sender, MgStorage* s)
{
    for (int i = 0; i < 3; i++)
        _points[i] = Point2d();
    return _initialize(MgArc::Type(), sender, s);
}

void MgCmdArc3P::drawArcHandle(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step > 0 && m_step < 3
        && sender->dragging() && !getSnappedType(sender)) {
        gs->drawHandle(_points[m_step], kGiHandleHotVertex);
    }
}

bool MgCmdArc3P::draw(const MgMotion* sender, GiGraphics* gs)
{
    drawArcHandle(sender, gs);
    if (m_step > 0) {
        GiContext ctx(-2, GiColor(0, 126, 0, 32), GiContext::kDotLine);
        gs->drawLine(&ctx, _points[0], _points[1]);
        
        MgArc* arc = (MgArc*)dynshape()->shape();
        gs->drawCircle(&ctx, arc->getCenter(), arc->getRadius());
    }
    return MgCommandDraw::draw(sender, gs);
}

void MgCmdArc3P::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();

    if (step == 0) {
        _points[0] = pt;
    }
    else if (step == 1) {
        _points[1] = pt;
        _points[2] = pt;
        arc->setStartMidEnd(_points[0], pt, pt);
    }
    else if (step == 2) {
        _points[2] = pt;
        arc->setStartMidEnd(_points[0], _points[1], pt);
    }
}

float MgCommand::drawAngleText(const MgMotion* sender, GiGraphics* gs, float angle, void* stdstr)
{
    Point2d pt(sender->pointM + Vector2d(0, sender->displayMmToModel(12.f)));
    pt.y = mgMin(pt.y, sender->view->xform()->getWndRectM().ymax);
    return drawAngleText(sender->view, gs, angle, pt, 1, stdstr, NULL);
}

float MgCommand::drawAngleText(MgView* view, GiGraphics* gs, float angle,
                               const Point2d& pt, int align, void* stdstr, GiTextWidthCallback* c)
{
    std::stringstream ss;
    int decimal = view->getOptionInt("degreeDecimal", 2);
    
    ss << mgbase::roundReal(mgbase::rad2Deg(angle), decimal) << MgLocalized::getString(view, "degrees");
    if (stdstr) {
        *((std::string*)stdstr) = ss.str();
    }
    return gs ? gs->drawTextAt(c, GiColor::Red().getARGB(), ss.str().c_str(), pt, 5.f, align) : 0.f;
}

// MgCmdArcCSE
//

bool MgCmdArcCSE::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step == 2 && sender->dragging()) {    // 画弧时显示圆心与终端连线
        GiContext ctx(0, GiColor(0, 126, 0, 64), GiContext::kDotLine);
        gs->drawLine(&ctx, _points[0], _points[2]);
        drawAngleText(sender, gs, fabsf(((MgArc*)dynshape()->shape())->getSweepAngle()));
    }
    if (_points[0] != _points[1]) {
        gs->drawHandle(_points[0], kGiHandleCenter);
        if (m_step == 0) {
            GiContext ctx(-2, GiColor(0, 126, 0, 32), GiContext::kDashLine);
            gs->drawCircle(&ctx, _points[0], _points[0].distanceTo(_points[1]));
        }
    }
    return MgCmdArc3P::draw(sender, gs);
}

void MgCmdArcCSE::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();
    
    if (step == 0) {
        _points[0] = pt;                    // 记下圆心
        arc->offset(pt - arc->getCenter(), -1);
    }
    else if (step == 1) {
        _points[1] = pt;                    // 记下起点
        _points[2] = pt;                    // 起点与终点重合
        arc->setCenterStartEnd(_points[0], _points[1]); // 初始转角为0
    }
    else if (step == 2) {
        arc->setCenterStartEnd(_points[0], _points[1], pt);
        _points[2] = pt;                    // 记下终点
    }
}

// MgCmdSector
//

bool MgCmdSector::initialize(const MgMotion* sender, MgStorage* s)
{
    bool ret = MgCmdArcCSE::initialize(sender, s);
    if (ret) {
        ((MgArc*)dynshape()->shape())->setSubType(1);
    }
    return ret;
}

// MgCmdCompass
//

bool MgCmdCompass::initialize(const MgMotion* sender, MgStorage* s)
{
    _radius = 0;
    if (s) {
        _decimal = s->readInt("decimal", _decimal);
    }
    return MgCmdArc3P::initialize(sender, s);
}

bool MgCmdCompass::draw(const MgMotion* sender, GiGraphics* gs)
{
    if (m_step == 2 && sender->dragging()) {    // 画弧时显示圆心与终端连线
        GiContext ctx(0, GiColor(0, 126, 0, 64), GiContext::kDotLine);
        gs->drawLine(&ctx, _points[0], _points[2]);
        drawAngleText(sender, gs, fabsf(((MgArc*)dynshape()->shape())->getSweepAngle()));
    }
    if (_points[0] != _points[1]) {
        gs->drawHandle(_points[0], kGiHandleCenter);
        if (m_step == 0) {
            GiContext ctx(-2, GiColor(0, 126, 0, 32), GiContext::kDashLine);
            gs->drawCircle(&ctx, _points[0], _points[0].distanceTo(_points[1]));
        }
    }
    return MgCmdArc3P::draw(sender, gs);
}

bool MgCmdCompass::click(const MgMotion* sender)
{
    Point2d pt(snapPoint(sender));
    MgArc* arc = (MgArc*)dynshape()->shape();
    
    if (_points[1] != _points[2] || m_step > 1) {
        _points[1] += pt - _points[0];  // 半径不变
        _points[2] += pt - _points[0];  // 半径不变
        _points[0] = pt;                // 定圆心
        _points[2] = 2 * _points[0] - _points[1].asVector();    // 保持不同
        arc->offset(pt - arc->getCenter(), -1);
        m_step = 0;
        sender->view->redraw();
    }
    
    return true;
}

void MgCmdCompass::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();
    
    if (step == 0) {
        if (_points[1] == _points[2]) {
            _points[0] = pt;                // 记下圆心
            arc->offset(pt - arc->getCenter(), -1);
        } else {                            // 设置起始方向
            _points[1] = _points[0].rulerPoint(pt, _radius, 0);
            if (_points[1] == _points[2]) {
                _points[2] = _points[0].rulerPoint(pt, -_radius, 0);    // 保持不同
            }
            arc->setCenterStartEnd(_points[0], _points[1]); // 初始转角为0
            m_step = 2;
        }
    }
    else if (step == 1) {
        _points[1] = pt;                    // 记下起点
        _points[2] = pt;                    // 起点与终点重合
        arc->setCenterStartEnd(_points[0], _points[1]); // 初始转角为0
        _radius = arc->getRadius();
    }
    else if (step == 2) {
        arc->setCenterStartEnd(_points[0], _points[1], pt);
        float angle = mgbase::roundReal(arc->getSweepAngle() * _M_R2D, _decimal) * _M_D2R;
        arc->setCenterRadius(arc->getCenter(), _radius, arc->getStartAngle(), angle);
        _points[2] = arc->getEndPoint();    // 记下终点
    }
}

// MgCmdArcTan
//

void MgCmdArcTan::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    MgArc* arc = (MgArc*)dynshape()->shape();

    if (step == 0) {
        _points[0] = pt;
    }
    else if (step == 1) {
        _points[1] = pt;
        _points[2] = pt;
        arc->setTanStartEnd(_points[1] - _points[0], pt, pt);
    }
    else if (step == 2) {
        arc->setTanStartEnd(_points[1] - _points[0], _points[1], pt);
        _points[2] = pt;
    }
}
