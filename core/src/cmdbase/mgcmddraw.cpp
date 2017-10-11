// mgcmddraw.cpp: 实现绘图命令基类
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcmddraw.h"
#include "mgsnap.h"
#include "mgaction.h"
#include "cmdsubject.h"
#include <string.h>
#include "mglog.h"
#include "mgstorage.h"

Point2d MgCommandDraw::m_lastSnapped[];

MgCommandDraw::MgCommandDraw(const char* name)
    : MgCommand(name), m_step(0), m_shape(MgShape::Null())
{
}

MgCommandDraw::~MgCommandDraw()
{
    MgObject::release_pointer(m_shape);
}

bool MgCommandDraw::cancel(const MgMotion* sender)
{
    if (m_step > 0) {
        m_step = 0;
        m_shape->shape()->clear();
        sender->view->getSnap()->clearSnap(sender);
        sender->view->redraw();
        return true;
    }
    return false;
}

bool MgCommandDraw::_initialize(int shapeType, const MgMotion* sender, MgStorage* s)
{
    GiContext ctx;
    
    if (sender->view->context()) {
        ctx = *sender->view->context();
    }
    if (!m_shape) {
        m_shape = sender->view->createShapeCtx(shapeType);
        if (!m_shape || !m_shape->shape())
            return false;
        m_shape->setParent(sender->view->shapes(), 0);
    }
    sender->view->setNewShapeID(0);
    m_step = 0;
    m_shape->shape()->clear();
    m_flags = (m_flags & ~1) | (sender->view->getOptionBool("drawOneShape", false) ? 1 : 0);
    sender->view->getSnap()->clearSnap(sender);
    
    if (s) {
        if (s->readBool("fixedlen", false)) {
            m_shape->shape()->setFlag(kMgFixedLength, true);
        }
        if (s->readBool("fixedsize", false)) {
            m_shape->shape()->setFlag(kMgFixedSize, true);
        }
        if (s->readBool("locked", false)) {
            m_shape->shape()->setFlag(kMgLocked, true);
        }
        if (s->readBool("hiden", false)) {
            m_shape->shape()->setFlag(kMgHideContent, true);
        }
        if (s->readFloat("lineWidth", -1e3f) > 1-1e3f) {
            ctx.setLineWidth(s->readFloat("lineWidth", 0), true);
        }
        if (s->readInt("lineStyle", -10) > -10) {
            ctx.setLineStyle(s->readInt("lineStyle", 0));
        }
        if (s->readInt("lineAlpha", -10) > -10) {
            ctx.setLineAlpha(s->readInt("lineAlpha", 0));
        }
        if (s->readInt("lineRGB", 0) != 0) {
            int value = s->readInt("lineRGB", 0);
            ctx.setLineColor((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF);
        }
        if (s->readInt("lineARGB", 0) != 0) {
            int value = s->readInt("lineARGB", 0);
            ctx.setLineColor((value >> 16) & 0xFF, (value >> 8) & 0xFF, value & 0xFF, (value >> 24) & 0xFF);
        }
        if (s->readInt("startArrayHead", -10) > -10) {
            ctx.setStartArrayHead(s->readInt("startArrayHead", 0));
        }
        if (s->readInt("endArrayHead", -10) > -10) {
            ctx.setEndArrayHead(s->readInt("endArrayHead", 0));
        }
    }
    m_shape->setContext(ctx);
    m_flags = (m_flags & ~2) | (sender->view->context() && ctx != *sender->view->context() ? 2 : 0);
    
    int n = s ? s->readFloatArray("points", (float*)0, 0) : 0;
    if (n > 1) {
        MgMotion tmpmotion(*sender);
        Point2d buf[20];
        bool oldSnap = sender->view->getOptionBool("snapEnabled", true);
        
        sender->view->setOptionBool("snapEnabled", false);
        n = s->readFloatArray("points", &buf[0].x, mgMin(n, 20*2)) / 2;
        
        if (s->readBool("multiMoved", false)) {
            tmpmotion.pointM = buf[0];
            tmpmotion.startPtM = tmpmotion.pointM;
            tmpmotion.lastPtM = tmpmotion.pointM;
            touchBegan(&tmpmotion);
            
            for (int i = 1; i < n; i++) {
                tmpmotion.pointM = buf[i];
                touchMoved(&tmpmotion);
                tmpmotion.lastPtM = tmpmotion.pointM;
            }
            touchEnded(&tmpmotion);
        } else {
            for (int i = 0; i < n; i += 2) {
                tmpmotion.startPtM = buf[i];
                tmpmotion.pointM = buf[i];
                touchBegan(&tmpmotion);
                tmpmotion.pointM = buf[i + 1 < n ? i + 1 : i];
                touchMoved(&tmpmotion);
                touchEnded(&tmpmotion);
                m_shape->setContext(ctx);
            }
        }
        cancel(sender);
        sender->view->setOptionBool("snapEnabled", oldSnap);
    }
    
    return true;
}

MgShape* MgCommandDraw::addShape(const MgMotion* sender, MgShape* shape)
{
    shape = shape ? shape : m_shape;
    MgShape* newsp = MgShape::Null();
    
    if (sender->view->getOptionBool("newShapeFixedlen", false)) {
        shape->shape()->setFlag(kMgFixedLength, true);
    }
    if (sender->view->getOptionBool("newShapeFixedsize", false)) {
        shape->shape()->setFlag(kMgFixedSize, true);
    }
    if (sender->view->getOptionBool("newShapeLocked", false)) {
        shape->shape()->setFlag(kMgLocked, true);
    }
    if (sender->view->getOptionBool("newShapeHiden", false)) {
        shape->shape()->setFlag(kMgHideContent, true);
    }
    
    if (sender->view->shapeWillAdded(shape)) {
        newsp = sender->view->shapes()->addShape(*shape);
        if (shape == m_shape) {
            m_shape->shape()->clear();
            sender->view->shapeAdded(newsp);
        } else {
            sender->view->getCmdSubject()->onShapeAdded(sender, newsp);
        }
        if (strcmp(getName(), "splines") != 0) {
            sender->view->setNewShapeID(newsp->getID());
        }
    }
    if (m_shape && sender->view->context() && (m_flags & 2) == 0) {
        m_shape->setContext(*sender->view->context());
    }
    if (m_flags & 1) {
        sender->view->toSelectCommand();
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
    bool ret = m_step > 0 && m_shape->draw(0, *gs, (const GiContext*)0, -1);
    return sender->view->getSnap()->drawSnap(sender, gs) || ret;
}

bool MgCommandDraw::gatherShapes(const MgMotion* /*sender*/, MgShapes* shapes)
{
    if (m_step > 0 && m_shape && m_shape->getPointCount() > 0) {
        shapes->addShape(*m_shape);
    }
    return false; // gather more shapes via draw()
}

bool MgCommandDraw::click(const MgMotion* sender)
{
    return (m_step == 0 ? _click(sender)
            : touchBegan(sender) && touchEnded(sender));
}

bool MgCommandDraw::_click(const MgMotion* sender)
{
    if (sender->view->getOptionBool("notClickSelectInDrawCmd", false)) {
        return true;
    }
    
    Box2d limits(sender->displayMmToModelBox("hitTestTol", 10.f));
    MgHitResult res;
    const MgShape* shape = sender->view->shapes()->hitTest(limits, res);
    
    if (shape) {
        sender->view->setNewShapeID(shape->getID());
        sender->view->toSelectCommand();
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
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::touchMoved(const MgMotion* sender)
{
    sender->view->redraw();
    sender->view->shapeChanged(m_shape);
    return true;
}

bool MgCommandDraw::touchEnded(const MgMotion* sender)
{
    sender->view->getSnap()->clearSnap(sender);
    sender->view->redraw();
    return true;
}

bool MgCommandDraw::mouseHover(const MgMotion* sender)
{
    return m_step > 0 && touchMoved(sender);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, bool firstStep)
{
    return snapPoint(sender, firstStep ? sender->startPtM : sender->pointM, firstStep);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, 
                                 const Point2d& orignPt, bool firstStep)
{
    return snapPoint(sender, orignPt, firstStep, m_step);
}

Point2d MgCommandDraw::snapPoint(const MgMotion* sender, const Point2d& orignPt,
                                 bool firstStep, int handle)
{
    MgSnap *snap = sender->view->getSnap();
    Point2d pt(snap->snapPoint(sender, orignPt, firstStep ? MgShape::Null() : m_shape, handle));
    
    if ( (firstStep || !sender->dragging())
        && snap->getSnappedType() >= kMgSnapPoint) {
        sender->view->getCmdSubject()->onPointSnapped(sender, m_shape);
    }
    if (firstStep || snap->getSnappedType() >= kMgSnapPoint) {
        m_lastSnapped[0] = pt;
        m_lastSnapped[1] = sender->pointM;
    }
    
    return pt;
}

Point2d MgCommandDraw::snapPointWidhOptions(const MgMotion* sender, int options, bool firstStep)
{
    if (options < 0) {
        return snapPoint(sender, firstStep);
    }
    int old = sender->view->getSnap()->getSnapOptions(sender->view);
    sender->view->getSnap()->setSnapOptions(sender->view, options);
    Point2d pt(snapPoint(sender, firstStep));
    sender->view->getSnap()->setSnapOptions(sender->view, old);
    return pt;
}

void MgCommandDraw::ignoreStartPoint(const MgMotion* sender, int handle)
{
    if (handle >= 0 && handle < m_shape->getPointCount()) {
        sender->view->getSnap()->setIgnoreStartPoint(m_shape->getHandlePoint(handle));
    }
}

int MgCommandDraw::getSnappedType(const MgMotion* sender) const
{
    return sender->view->getSnap()->getSnappedType();
}

void MgCommandDraw::setStepPoint(const MgMotion*, int step, const Point2d& pt)
{
    if (step > 0) {
        m_shape->shape()->setHandlePoint(step, pt, 0);
    }
}

bool MgCommandDraw::touchBeganStep(const MgMotion* sender)
{
    if (0 == m_step) {
        m_step = 1;
        int options = snapOptionsForStep(sender, 0);
        Point2d pnt(snapPointWidhOptions(sender, options, true));
        for (int i = m_shape->getPointCount() - 1; i >= 0; i--) {
            m_shape->shape()->setPoint(i, pnt);
        }
        setStepPoint(sender, 0, pnt);
    }
    else {
        int options = snapOptionsForStep(sender, m_step);
        setStepPoint(sender, m_step, snapPointWidhOptions(sender, options));
    }
    m_shape->shape()->update();

    return MgCommandDraw::touchBegan(sender);
}

bool MgCommandDraw::touchMovedStep(const MgMotion* sender)
{
    if (sender->dragging()) {
        int options = snapOptionsForStep(sender, m_step);
        setStepPoint(sender, m_step, snapPointWidhOptions(sender, options));
        m_shape->shape()->update();
    }
    return MgCommandDraw::touchMoved(sender);
}

bool MgCommandDraw::touchEndedStep(const MgMotion* sender)
{
    int options = snapOptionsForStep(sender, m_step);
    Point2d pnt(snapPointWidhOptions(sender, options));
    Tol tol(sender->displayMmToModel(2.f));
    
    setStepPoint(sender, m_step, pnt);
    m_shape->shape()->update();
    
    if (isStepPointAccepted(sender, pnt)) {
        m_step++;
        if (m_step >= getMaxStep()) {
            m_step = 0;
            if (!m_shape->shape()->getExtent().isEmpty(tol, false)) {
                addShape(sender);
            }
        }
    }

    return MgCommandDraw::touchEnded(sender);
}

bool MgCommandDraw::isStepPointAccepted(const MgMotion* sender, const Point2d& pt)
{
    return !pt.isEqualTo(m_shape->getPoint(m_step - 1), Tol(sender->displayMmToModel(2.f)));
}

// MgLocalized
//

#include "mglocal.h"
#include "mgcoreview.h"

//! Callback class to convert text to std::string.
struct LocStringCallback : MgStringCallback {
    std::string result;
    virtual void onGetString(const char* text) { if (text) result = text; }
};

std::string MgLocalized::getString(MgView* view, const char* name)
{
    LocStringCallback c;
    view->getLocalizedString(name, &c);
    return c.result.empty() ? name : c.result;
}

int MgLocalized::formatString(char *buffer, int size, MgView* view, const char *format, ...)
{
    std::string str;
    va_list arglist;

    va_start(arglist, format);
    if (*format == '@') {
        str = getString(view, format + 1);
        format = str.c_str();
    }
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return vsprintf_s(buffer, size, format, arglist);
#else
    return vsprintf(buffer, format, arglist);
#endif
}
