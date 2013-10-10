// mgcmdselect.cpp: 实现选择命令类
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdselect.h"
#include <mgcmdmgr.h>
#include <mgbasicsp.h>
#include <string.h>
#include <algorithm>
#include <functional>
#include <mgshapet.h>
#include <mgsnap.h>
#include <mgaction.h>
#include <mgshapetype.h>
#include <mgcomposite.h>
#include <mglockdata.h>
#include <cmdsubject.h>

#if defined(_WIN32) && !defined(ENABLE_DRAG_SELBOX)
#define ENABLE_DRAG_SELBOX
#endif

int MgCmdSelect::getSelection(MgView* view, int count, 
                              MgShape** shapes, bool forChange)
{
    if (forChange && m_clones.empty())  // forChange让此后的图形修改是在临时图形上进行的
        cloneShapes(view);
    
    int ret = 0;
    int maxCount = (int)(m_clones.empty() ? m_selIds.size() : m_clones.size());
    
    if (count < 1 || !shapes)
        return maxCount;
    
    count = mgMin(count, maxCount);
    MgShapesLock locker(MgShapesLock::ReadOnly, 
        m_clones.empty() && count > 0 ? view : NULL);

    for (int i = 0; i < count; i++) {
        if (m_clones.empty()) {
            MgShape* shape = view->shapes()->findShape(m_selIds[i]);
            if (shape)
                shapes[ret++] = shape;
        }
        else {
            shapes[ret++] = m_clones[i];
        }
    }
    if (forChange) {
        m_showSel = false;  // 禁止亮显选中图形，以便外部可动态修改图形属性并原样显示
    }
    
    return ret;
}

bool MgCmdSelect::dynamicChangeEnded(MgView* view, bool apply)
{
    if (apply) {
        m_showSel = false;
    }
    return applyCloneShapes(view, apply);
}

MgCmdSelect::MgCmdSelect()
{
    m_editMode = true;
}

MgCmdSelect::~MgCmdSelect()
{
}

bool MgCmdSelect::cancel(const MgMotion* sender)
{
    bool ret = backStep(sender);
    ret = backStep(sender) || ret;
    return backStep(sender) || ret;
}

bool MgCmdSelect::initialize(const MgMotion* sender, MgStorage*)
{
    m_boxsel = false;
    m_id = 0;
    m_hit.segment = -1;
    m_handleIndex = 0;
    m_rotateHandle = 0;
    m_editMode = false;
    m_showSel = true;
    m_selIds.clear();
    
    sender->view->getCmdSubject()->onEnterSelectCommand(sender);
    
    MgShape* shape = getShape(sender->view->getNewShapeID(), sender);
    if (shape) {
        m_selIds.push_back(shape->getID());         // 选中最新绘制的图形
        m_id = shape->getID();
        sender->view->redraw();
        if (!sender->view->getLockData()->lockedForWrite()) {   // Loading
            sender->view->selectionChanged();
            longPress(sender);
        }
    }
    
    return true;
}

bool MgCmdSelect::backStep(const MgMotion* sender)
{
    m_boxsel = false;
    m_boxHandle = 99;
    
    if (!m_clones.empty()) {                        // 正在拖改
        for (std::vector<MgShape*>::iterator it = m_clones.begin();
             it != m_clones.end(); ++it) {
            (*it)->release();
        }
        m_clones.clear();
        m_insertPt = false;
        sender->view->redraw();
        return true;
    }
    if (!m_selIds.empty()) {                        // 图形整体选中状态
        m_id = 0;
        m_hit.segment = -1;
        m_handleIndex = 0;
        m_rotateHandle = 0;
        m_selIds.clear();
        sender->view->redraw();
        sender->view->selectionChanged();
        return true;
    }
    return false;
}

bool MgCmdSelect::draw(const MgMotion* sender, GiGraphics* gs)
{
    std::vector<MgShape*> selection;
    const std::vector<MgShape*>& shapes = m_clones.empty() ? selection : m_clones;
    std::vector<MgShape*>::const_iterator it;
    Point2d pnt;
    GiContext ctxhd(0, GiColor(128, 128, 64, 172), 
                    kGiLineSolid, GiColor(172, 172, 172, 64));
    float radius = sender->displayMmToModel(sender->view->useFinger() ? 1.f : 3.f, gs);
    float r2x = radius * 2.5f;
    bool rorate = (!isEditMode(sender->view)
        && m_boxHandle >= 8 && m_boxHandle < 12);
    
    // 从 m_selIds 得到临时图形数组 selection
    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = getShape(*its, sender);
        if (shape)
            selection.push_back(shape);
    }
    if (selection.empty() && !m_selIds.empty()) {   // 意外情况导致m_selIds部分ID无效
        m_selIds.clear();
        sender->view->selectionChanged();
    }
    
    if (!m_showSel || (!m_clones.empty() && !isCloneDrag(sender))) {
        GiContext ctxbk(0, gs->getBkColor());       // 背景色、原线型
        for (it = selection.begin(); it != selection.end(); ++it) {
            if (! (*it)->shapec()->isKindOf(kMgShapeImage)) {
                (*it)->draw(1, *gs, &ctxbk, -1);    // 用背景色擦掉原图形
            }
        }
        
        if (m_showSel && !rorate) {                 // 拖动提示的参考线
            GiContext ctxshap(-1.05f, GiColor(0, 0, 255, 32), kGiLineDot);
            gs->drawLine(&ctxshap, m_ptStart, m_ptSnap);
        }
    }
    
    // 外部动态改变图形属性时，或拖动时
    if (!m_showSel || !m_clones.empty()) {
        for (it = shapes.begin(); it != shapes.end(); ++it) {
            (*it)->draw(m_showSel ? 2 : 0, *gs, NULL, -1);  // 原样显示
        }
    }
    else if (m_clones.empty()) {                    // 蓝色显示选中的图形
        GiContext ctx(-1.f, GiColor(0, 0, 255, 48));
        for (it = shapes.begin(); it != shapes.end(); ++it) {
            (*it)->draw(1, *gs, &ctx, m_hit.segment);
        }
    }
    
    if (sender->view->shapes()->getOwner()->isKindOf(kMgShapeComposite)) {
        GiContext ctxshap(0, GiColor(0, 0, 255, 64), kGiLineDot);
        Box2d rect(sender->view->shapes()->getExtent());
        rect.inflate(sender->displayMmToModel(2.f));
        gs->drawRect(&ctxshap, rect);
    }
    
    if (m_boxsel) {                 // 显示框选半透明蓝色边框
        GiContext ctxshap(0, GiColor(0, 0, 255, 80), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid,
                          GiColor(0, 0, 255, 24));
        gs->drawRect(&ctxshap, Box2d(sender->startPtM, sender->pointM));
    }
    else if (sender->view->isContextActionsVisible() && !selection.empty()) {
        Box2d selbox(getBoundingBox(sender));
        GiContext ctxshap(0, GiColor(0, 0, 255, 80), kGiLineDash);  // 蓝色虚线包络框
        gs->drawRect(&ctxshap, selbox);
    }
    else if (!selection.empty() && m_showSel
             && (selection.size() > 1 || !isEditMode(sender->view))) {
        Box2d selbox(getBoundingBox(sender));
        
        if (!selbox.isEmpty()) {
            GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
            gs->drawRect(&ctxshap, selbox);
        }
        if (m_clones.empty() && !shapes.empty()) {
#ifdef ENABLE_DRAG_SELBOX
            for (int i = canTransform(shapes.front(), sender) ? 7 : -1; i >= 0; i--) {
                mgnear::getRectHandle(selbox, i, pnt);
                if (!gs->drawHandle(pnt, 0))
                    gs->drawEllipse(&ctxhd, pnt, radius);
            }
            for (int j = canRotate(shapes.front(), sender) ? 1 : -1;
                j >= 0; j--) {
                mgnear::getRectHandle(selbox, j == 0 ? 7 : 5, pnt);
                pnt = pnt.rulerPoint(selbox.center(),
                                     -sender->displayMmToModel(10.f), 0);
                
                float w = -1.f * gs->xf().getWorldToDisplayY(false);
                float r = pnt.distanceTo(selbox.center());
                float sangle = mgMin(30.f, mgMax(10.f, mgbase::rad2Deg(12.f / r)));
                GiContext ctxarc(w, GiColor(0, 255, 0, 128),
                                 j ? kGiLineSolid : kGiLineDot);
                gs->drawArc(&ctxarc, selbox.center(), r, r,
                            j ? -mgbase::deg2Rad(sangle) : mgbase::deg2Rad(180.f - sangle), 
                            mgbase::deg2Rad(2.f * sangle));
                
                if (!gs->drawHandle(pnt, 0))
                    gs->drawEllipse(&ctxhd, pnt, radius);
            }
#endif // ENABLE_DRAG_SELBOX
        }
        else if (!selbox.isEmpty()) {   // 正在拖动临时图形
            if (rorate) {               // 旋转提示的参考线
                if (!gs->drawHandle(selbox.center(), 0))
                    gs->drawEllipse(&ctxhd, selbox.center(), radius);
                GiContext ctxshap(0, GiColor(0, 0, 255, 128), kGiLineDash);
                gs->drawLine(&ctxshap, selbox.center(), m_ptSnap);
            }
            else {
                if (!gs->drawHandle(m_ptSnap, 0))
                    gs->drawEllipse(&ctxhd, m_ptSnap, radius);
            }
        }
    }
    
    // 下面显示控制点. 此时仅选中一个图形、有活动控制点
    if (shapes.size() == 1 && m_showSel
        && (isEditMode(sender->view) || (
            m_handleIndex > 0 || m_rotateHandle > 0))) {
        const MgShape* shape = shapes.front();
        int n = shape->shapec()->getHandleCount();
        
        for (int i = 0; i < n; i++) {
            if (shape->shapec()->isHandleFixed(i)
                || !isEditMode(sender->view)) {
                continue;
            }
            pnt = shape->shapec()->getHandlePoint(i);
            if (sender->dragging() || !gs->drawHandle(pnt, 0))
                gs->drawEllipse(&ctxhd, pnt, radius);
        }
        
        if ((m_handleIndex > 0 || m_rotateHandle > 0)
            && !sender->dragging()  // 不是(还未拖动但可插新点)，显示当前控制点
            && (!m_clones.empty() || !m_insertPt)) {
            int t = m_rotateHandle > 0 ? m_rotateHandle - 1 : m_handleIndex - 1;
            pnt = shape->shapec()->getHandlePoint(t);
            if (!gs->drawHandle(pnt, m_rotateHandle > 0 ? 2 : 1))
                gs->drawEllipse(&ctxhd, pnt, r2x);
        }
        if (m_insertPt && !m_clones.empty()) {  // 在临时图形上显示新插入顶点
            GiContext insertctx(ctxhd);
            insertctx.setFillColor(GiColor(255, 0, 0, 64));
            gs->drawEllipse(&insertctx, m_hit.nearpt, r2x);
            gs->drawHandle(m_hit.nearpt, 1);
        }
    }
    if (shapes.size() == 1 && m_clones.empty()
        && m_hit.nearpt.distanceTo(pnt) > r2x * 2) {
        gs->drawEllipse(&ctxhd, m_hit.nearpt, radius / 2);  // 显示线上的最近点，以便用户插入新点
    }
    
    sender->cmds()->getSnap()->drawSnap(sender, gs); // 显示拖动捕捉提示线
    sender->view->getCmdSubject()->drawInSelectCommand(sender,
        shapes.empty() ? NULL : shapes.front(), m_handleIndex - 1, gs);
    
    return true;
}

int MgCmdSelect::gatherShapes(const MgMotion* sender, MgShapes* shapes)
{
    int ret = 0;

    if (m_boxsel) {
        MgShapeT<MgRect> shape;
        GiContext ctxshap(0, GiColor(0, 0, 255, 128), 
                          isIntersectMode(sender) ? kGiLineDash : kGiLineSolid,
                          GiColor(0, 0, 255, 32));
        *shape.context() = ctxshap;
        ((MgRect*)shape.shape())->setRect2P(sender->startPtM, sender->pointM);
        shapes->addShape(shape);
        ret++;
    }
    for (std::vector<MgShape*>::const_iterator it = m_clones.begin();
         it != m_clones.end(); ++it) {
        shapes->addShape(*(*it));
        ret++;
    }

    return ret;
}

MgCmdSelect::sel_iterator MgCmdSelect::getSelectedPostion(MgShape* shape)
{
    sel_iterator it = m_selIds.end();
    if (shape) {
        it = std::find_if(m_selIds.begin(), m_selIds.end(),
                          std::bind2nd(std::equal_to<int>(), shape->getID()));
    }
    return it;
}

MgShape* MgCmdSelect::getShape(int id, const MgMotion* sender) const
{
    return sender->view->shapes()->findShape(id);
}

bool MgCmdSelect::isSelected(MgShape* shape)
{
    return getSelectedPostion(shape) != m_selIds.end();
}

MgShape* MgCmdSelect::hitTestAll(const MgMotion* sender, MgHitResult& res)
{
    Box2d limits(sender->pointM, sender->displayMmToModel(8.f), 0);
    return sender->view->shapes()->hitTest(limits, res);
}

MgShape* MgCmdSelect::getSelectedShape(const MgMotion* sender)
{
    MgShape* p = getShape(m_id, sender);
    return (!p && !m_selIds.empty()) ? getShape(m_selIds.front(), sender) : p;
}

bool MgCmdSelect::canSelect(MgShape* shape, const MgMotion* sender)
{
    Box2d limits(sender->startPtM, sender->displayMmToModel(10.f), 0);
    float d = _FLT_MAX;
    
    if (shape) {
        d = shape->shape()->hitTest(limits.center(), limits.width() / 2, m_hit);
        if (m_hit.inside && shape->hasFillColor()) {
            return true;
        }
        if (d > limits.width() / 2) {
            int n = m_editMode ? shape->shapec()->getHandleCount() : 0;
            while (--n >= 0 && d > limits.width() / 2) {
                d = shape->shapec()->getHandlePoint(n).distanceTo(sender->startPtM);
            }
        }
    }
    
    return d <= limits.width() / 2;
}

int MgCmdSelect::hitTestHandles(MgShape* shape, const Point2d& pointM,
                                const MgMotion* sender, float tolmm)
{
    if (!shape) {
        return 0;
    }
    
    int handleIndex = 0;
    float minDist = sender->displayMmToModel(tolmm);
    float nearDist = m_hit.nearpt.distanceTo(pointM);
    int n = shape->shapec()->getHandleCount();
    
    for (int i = 0; i < n; i++) {
        float d = pointM.distanceTo(shape->shapec()->getHandlePoint(i));
        if (minDist > d && !shape->shapec()->isHandleFixed(i)) {
            minDist = d;
            handleIndex = i + 1;
        }
    }
    
    if (sender->pressDrag && nearDist < minDist / 3
        && minDist > sender->displayMmToModel(8.f)
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        m_insertPt = true;
    }
    
    return handleIndex;
}

Point2d MgCmdSelect::snapPoint(const MgMotion* sender, const MgShape* shape)
{
    std::vector<int> ignoreids(1 + m_clones.size(), 0);
    for (unsigned i = 0; i < m_clones.size(); i++)
        ignoreids[i] = m_clones[i]->getID();
    
    MgSnap* snap = sender->cmds()->getSnap();
    return snap->snapPoint(sender, sender->pointM, shape, m_handleIndex - 1, 
                           m_rotateHandle - 1, (const int*)&ignoreids.front());
}

bool MgCmdSelect::click(const MgMotion* sender)
{
    m_boxHandle = 99;
    m_rotateHandle = 0;
    if (sender->pressDrag)
        return false;
    
    MgHitResult res;
    MgShape *shape = NULL;
    bool    canSelAgain;
    
    if (!m_showSel) {                   // 上次是禁止亮显
        m_showSel = true;               // 恢复亮显选中的图形
        sender->view->regenAll();          // 可能图形属性已变，重新构建显示
    }
    applyCloneShapes(sender->view, false);
    
    m_insertPt = false;                 // 默认不是插入点，在hitTestHandles中设置
    shape = getSelectedShape(sender);   // 取上次选中的图形
    canSelAgain = (m_selIds.size() == 1 // 多选时不进入热点状态
                   && canSelect(shape, sender));    // 仅检查这个图形能否选中
    
    if (!canSelAgain) {                 // 没有选中或点中其他图形
        shape = hitTestAll(sender, res);
        bool changed = ((int)m_selIds.size() != (shape ? 1 : 0))
            || (shape && shape->getID() != m_id);

        m_selIds.clear();               // 清除选择集
        if (shape)
            m_selIds.push_back(shape->getID()); // 选中新图形
        m_id = shape ? shape->getID() : 0;
        
        m_hit = res;
        m_handleIndex = 0;

        if (changed) {
            sender->view->selectionChanged();
        }
        else if (shape && m_selIds.size() == 1 && !shape->shape()->isKindOf(MgSplines::Type())) {
            bool issmall = (shape->shape()->getExtent().width() < sender->displayMmToModel(5.f)
                            && shape->shape()->getExtent().height() < sender->displayMmToModel(5.f));
            m_handleIndex = (m_editMode || !issmall ?
                             hitTestHandles(shape, sender->pointM, sender) : 0);
        }
    }
    else {
        m_handleIndex = 0;
        if (isEditMode(sender->view) || (canRotate(shape, sender)
                && !shape->shapec()->isKindOf(MgSplines::Type()))) {
            m_handleIndex = hitTestHandles(shape, sender->pointM, sender);
        }
    }
    if (!isEditMode(sender->view) && canRotate(shape, sender)
        && !shape->shapec()->isKindOf(MgSplines::Type())) {
        m_rotateHandle = m_handleIndex;
    }
    sender->view->setNewShapeID(m_id);
    sender->view->redraw();
    
    if (!sender->pressDrag && (m_editMode || m_handleIndex == 0)) {
        MgActionDispatcher* dispatcher = sender->cmds()->getActionDispatcher();
        dispatcher->showInSelect(sender, getSelectState(sender->view),
                                 shape, getBoundingBox(sender));
        return true;
    }
    
    return m_id != 0;
}

bool MgCmdSelect::doubleClick(const MgMotion* sender)
{
    MgActionDispatcher* dispatcher = sender->cmds()->getActionDispatcher();
    MgShape* shape = getSelectedShape(sender);
    Box2d box(getBoundingBox(sender));

    if (dispatcher->showInSelect(sender, getSelectState(sender->view), shape, box)) {
        return shape != NULL;
    }

    return setEditMode(sender, !isEditMode(sender->view));  // 如果没实现菜单
}

bool MgCmdSelect::longPress(const MgMotion* sender)
{
    bool ret = false;
    
    if (m_selIds.empty()) {
        ret = click(sender);
    }
    
    MgShape* shape = getSelectedShape(sender);
    int selState = getSelectState(sender->view);    
    MgActionDispatcher* dispatcher = sender->cmds()->getActionDispatcher();

    if (shape && m_handleIndex > 0) {
        m_handleIndex = hitTestHandles(shape, sender->pointM, sender);
        sender->view->redraw();
    }
    if (dispatcher->showInSelect(sender, selState, shape, getBoundingBox(sender))) {
        ret = true;
    }
    
    return ret;
}

static int _dragData = 0;

bool MgCmdSelect::touchBegan(const MgMotion* sender)
{
    if (!sender->switchGesture) {
        MgHitResult res;
        MgShape* newshape = hitTestAll(sender, res);
        MgShape* oldshape = getSelectedShape(sender);
        
        if (newshape && newshape->getID() != m_id
            && !sender->view->shapes()->getOwner()->isKindOf(kMgShapeComposite)
            && (!oldshape || !canSelect(oldshape, sender))) {
            m_hit = res;
            m_id = newshape->getID();
            m_selIds.clear();
            m_selIds.push_back(m_id);
            m_handleIndex = 0;
            m_rotateHandle = 0;
            sender->view->selectionChanged();
        }
    }
    
    cloneShapes(sender->view);
    MgShape* shape = m_clones.empty() ? NULL : m_clones.front();
    
    if (!m_showSel) {
        m_showSel = true;
        sender->view->redraw();
    }
    
    m_insertPt = false;                     // setted in hitTestHandles
    if (m_clones.size() == 1) {
        canSelect(shape, sender);           // calc m_hit.nearpt
    }
    
    _dragData = 0;
    m_handleIndex = (m_clones.size() == 1 && (m_handleIndex > 0 || m_editMode) ?
                     hitTestHandles(shape, sender->pointM, sender) : 0);
    
    if (m_insertPt && shape && shape->shape()->isKindOf(MgBaseLines::Type())) {
        MgBaseLines* lines = (MgBaseLines*)(shape->shape());
        lines->insertPoint(m_hit.segment, m_hit.nearpt);
        shape->shape()->update();
        m_handleIndex = hitTestHandles(shape, m_hit.nearpt, sender);
    }
    
    if (m_clones.empty()) {
        m_boxsel = true;
    }
    m_boxHandle = 99;
    
    int tmpindex = hitTestHandles(shape, sender->startPtM, sender, 5);
    if (tmpindex < 1) {
        if (sender->startPtM.distanceTo(m_hit.nearpt) < sender->displayMmToModel(3.f)) {
            m_ptStart = m_hit.nearpt;
        }
        else {
            m_ptStart = sender->startPtM;
        }
    }
    else if (shape) {
        m_ptStart = shape->shape()->getHandlePoint(tmpindex - 1);
    }
    
    sender->view->redraw();
    
    return true;
}

bool MgCmdSelect::isIntersectMode(const MgMotion*)
{
    return true;
    //return (sender->startPt.x < sender->point.x
    //        && sender->startPt.y < sender->point.y);
}

Box2d MgCmdSelect::getBoundingBox(const MgMotion* sender)
{
    Box2d selbox;
    
    for (size_t i = 0; i < m_selIds.size(); i++) {
        MgShape* shape = getShape(m_selIds[i], sender);
        if (shape) {
            selbox.unionWith(shape->shape()->getExtent());
        }
    }

    float minDist = sender->view->xform()->displayToModel(8, true);
    if (!m_selIds.empty() && selbox.width() < minDist)
        selbox.inflate(minDist / 2, 0);
    if (!m_selIds.empty() && selbox.height() < minDist)
        selbox.inflate(0, minDist / 2);
    if (!m_selIds.empty())
        selbox.inflate(minDist / 8, minDist / 8);
    
    Box2d rcview(sender->view->xform()->getWndRectM());
    rcview.deflate(sender->displayMmToModel(1.f));
    selbox.intersectWith(rcview);
    
    Box2d selbox2(selbox);
    rcview.deflate(sender->displayMmToModel(12.f));
    selbox2.intersectWith(rcview);
    
    return selbox2.isEmpty(sender->displayMmToModel(5.f)) ? selbox : selbox2;
}

bool MgCmdSelect::isSelectedByType(MgView* view, int type)
{
    MgShape* sp[2] = { NULL };
    return getSelection(view, 2, sp) == 1 && sp[0] && sp[0]->shapec()->isKindOf(type);
}

bool MgCmdSelect::canTransform(MgShape* shape, const MgMotion* sender)
{
    return (shape && !shape->shape()->getFlag(kMgFixedLength)
            && !shape->shape()->getFlag(kMgShapeLocked)
            && sender->view->shapeCanTransform(shape));
}

bool MgCmdSelect::canRotate(MgShape* shape, const MgMotion* sender)
{
    return (shape && !shape->shape()->getFlag(kMgRotateDisnable)
            && !shape->shape()->getFlag(kMgShapeLocked)
            && sender->view->shapeCanRotated(shape));
}

bool MgCmdSelect::isDragRectCorner(const MgMotion* sender, Matrix2d& mat)
{
    m_boxHandle = 99;
    m_ptSnap = sender->pointM;
    mat = Matrix2d::kIdentity();
    
#ifdef ENABLE_DRAG_SELBOX
    if (isEditMode(sender->view) || m_selIds.empty() || m_boxsel)
        return false;
    
    Box2d selbox(getBoundingBox(sender));
    if (selbox.isEmpty())
        return false;
    
    Point2d pnt;
    int i;
    float mindist = sender->displayMmToModel(5.f);
    
    for (i = canTransform(getShape(m_selIds[0], sender), sender) ? 7 : -1; i >= 0; i--) {
        mgnear::getRectHandle(selbox, i, pnt);
        float addlen = i < 4 ? 0.f : sender->displayMmToModel(1.f); // 边中点优先1毫米
        if (mindist > sender->startPtM.distanceTo(pnt) - addlen) {
            mindist = sender->startPtM.distanceTo(pnt) - addlen;
            m_boxHandle = i;
        }
    }

    for (i = canRotate(getShape(m_selIds[0], sender), sender) ? 1 : -1;
        i >= 0; i--) {
        mgnear::getRectHandle(selbox, i == 0 ? 7 : 5, pnt);
        pnt = pnt.rulerPoint(selbox.center(), -sender->displayMmToModel(10.f), 0);
        if (mindist > sender->startPtM.distanceTo(pnt)) {
            mindist = sender->startPtM.distanceTo(pnt);
            m_boxHandle = 8 + i;
        }
    }
    if (m_boxHandle < 8) {
        Box2d newbox(selbox);
        mgnear::moveRectHandle(newbox, m_boxHandle, sender->pointM);
        
        if (!selbox.isEmpty() && !newbox.isEmpty()) {
            mat = Matrix2d::scaling((newbox.xmax - newbox.xmin) / selbox.width(),
                                    (newbox.ymax - newbox.ymin) / selbox.height(),
                                    selbox.leftBottom())
            * Matrix2d::translation(newbox.leftBottom() - selbox.leftBottom());
        }
    }
    else if (m_boxHandle < 10) {
        mgnear::getRectHandle(selbox, m_boxHandle == 8 ? 7 : 5, pnt);
        pnt = pnt.rulerPoint(selbox.center(), -sender->displayMmToModel(10.f), 0);
        float angle = (pnt - selbox.center()).angleTo2(sender->pointM - selbox.center());
        
        if (m_boxHandle == 8) {
            angle = mgbase::deg2Rad(mgRound(mgbase::rad2Deg(angle)) / 15 * 15.f);
        }
        mat = Matrix2d::rotation(angle, selbox.center());
        m_ptSnap = selbox.center().polarPoint(angle + (pnt - selbox.center()).angle2(),
                                              sender->pointM.distanceTo(selbox.center()));
    }
#endif // ENABLE_DRAG_SELBOX
    
    return m_boxHandle < 10;
}

static bool moveIntoLimits(MgBaseShape* shape, const MgMotion* sender)
{
    Box2d limits(sender->view->xform()->getWorldLimits()
                 * sender->view->xform()->worldToModel());
    Box2d rect(shape->getExtent());
    bool outside = false;
    
    limits.normalize();
    if (rect.xmin < limits.xmin) {
        rect.offset(limits.xmin - rect.xmin, 0);
        outside = true;
    }
    if (rect.xmax > limits.xmax) {
        rect.offset(limits.xmax - rect.xmax, 0);
        outside = true;
    }
    if (rect.ymin < limits.ymin) {
        rect.offset(0, limits.ymin - rect.ymin);
        outside = true;
    }
    if (rect.ymax > limits.ymax) {
        rect.offset(0, limits.ymax - rect.ymax);
        outside = true;
    }
    
    if (outside) {
        shape->offset(rect.center() - shape->getExtent().center(), -1);
        shape->update();
    }
    
    return outside;
}

bool MgCmdSelect::touchMoved(const MgMotion* sender)
{
    Point2d pointM(sender->pointM);
    Matrix2d mat;
    bool dragCorner = isDragRectCorner(sender, mat);
    
    if (m_insertPt && pointM.distanceTo(m_hit.nearpt) < sender->displayMmToModel(5.f)) {
        pointM = m_hit.nearpt;  // 拖动刚新加的点到起始点时取消新增
    }
    
    Vector2d minsnap(1e8f, 1e8f);
    int snapindex = -1;
    
    // 拖动多个图形则循环两遍：第一遍在每个选中图形中找捕捉距离最近的点，第二遍应用此最近点拖动
    for (int t = m_clones.size() > 1 && !dragCorner ? 2 : 1; t > 0; t--) {
        for (size_t i = 0; i < m_clones.size(); i++) {      // 对每个选中图形的临时图形
            MgBaseShape* shape = m_clones[i]->shape();
            MgShape* basesp = getShape(m_selIds[i], sender); // 对应的原始图形
            
            if (!basesp || shape->getFlag(kMgShapeLocked))  // 锁定图形不参与变形
                continue;
            shape->copy(*basesp->shape());                  // 先重置为原始位置
            
            bool oldFixedLength = shape->getFlag(kMgFixedLength);
            bool oldFixedSize = shape->getFlag(kMgFixedSize);
            
            if (!isEditMode(sender->view)) {
                shape->setFlag(kMgFixedLength, true);
                shape->setFlag(kMgFixedSize, true);
            }
            
            if (m_insertPt && shape->isKindOf(MgBaseLines::Type())) {
                MgBaseLines* lines = (MgBaseLines*)shape;
                lines->insertPoint(m_hit.segment, m_hit.nearpt);    // 插入新顶点
            }
            if (m_rotateHandle > 0 && canRotate(basesp, sender)) {
                int oldRotateHandle = m_rotateHandle;
                Point2d center(basesp->shapec()->getHandlePoint(m_rotateHandle - 1));
                
                if (center == m_ptStart || m_handleIndex == m_rotateHandle) {
                    m_rotateHandle = m_rotateHandle > 1 ? m_rotateHandle - 1 : m_rotateHandle + 1;
                    center = basesp->shapec()->getHandlePoint(m_rotateHandle - 1);
                }
                    
                float angle = (m_ptStart - center).angleTo2(pointM - center);
                shape->transform(Matrix2d::rotation(angle, center));
                
                Point2d fromPt, toPt;
                snapPoint(sender, m_clones[i]);
                if (sender->cmds()->getSnap()->getSnappedPoint(fromPt, toPt) > 0) {
                    angle = (fromPt - center).angleTo2(toPt - center);
                    shape->transform(Matrix2d::rotation(angle, center));
                }
                m_rotateHandle = oldRotateHandle;
            }
            else if (m_handleIndex > 0 && isEditMode(sender->view)) { // 拖动顶点
                float tol = sender->displayMmToModel(3.f);
                shape->setHandlePoint2(m_handleIndex - 1, 
                    snapPoint(sender, m_clones[i]), tol, _dragData);
            }
            else if (dragCorner) {                          // 拖动变形框的特定点
                shape->transform(mat);
            }
            else {                                          // 拖动整个图形
                int segment = (!m_editMode &&
                    shape->isKindOf(kMgShapeComposite)) ? -1 : m_hit.segment;

                shape->offset(pointM - m_ptStart, segment); // 先从起始点拖到当前点
                if (t > 1 || m_clones.size() == 1) {        // 不是第二遍循环
                    Vector2d snapvec(snapPoint(sender, m_clones[i]) - pointM);
                    shape->offset(snapvec, segment);        // 再从当前点拖到捕捉点
                    if (t > 1) {                            // 是拖动多个图形的第一遍
                        if (!snapvec.isZeroVector() && minsnap.length() > snapvec.length()) {
                            minsnap = snapvec;              // 找捕捉距离最近的点
                            snapindex = (int)i;
                        }
                    }
                }
                if (m_clones.size() > 1 && t == 1           // 是拖动多个图形的第二遍
                    && minsnap != Vector2d(1e8f, 1e8f)) {   // 第一遍捕捉到点
                    if (snapindex >= 0) {
                        snapPoint(sender, m_clones[snapindex]); // 切换到对应图形的捕捉状态
                        snapindex = -1;                     // 第二遍只切换一次
                    }
                    shape->offset(minsnap, segment);        // 这些图形都移动相同距离
                }
                if (t == 1) {
                    sender->view->shapeMoved(m_clones[i], segment); // 通知已移动
                }
            }
            shape->update();
            moveIntoLimits(shape, sender);                  // 限制图形在视图范围内
            
            if (!isEditMode(sender->view)) {
                shape->setFlag(kMgFixedLength, oldFixedLength);
                shape->setFlag(kMgFixedSize, oldFixedSize);
            }
        }
        sender->view->redraw();
    }
    
    if (m_clones.empty() && m_boxsel) {    // 没有选中图形时就滑动多选
        Box2d snap(sender->startPtM, sender->pointM);
        void *it = NULL;
        MgShape* shape = sender->view->shapes()->getFirstShape(it);
        
        m_selIds.clear();
        m_id = 0;
        m_hit.segment = -1;
        for (; shape; shape = sender->view->shapes()->getNextShape(it)) {
            if (isIntersectMode(sender) ? shape->shape()->hitTestBox(snap)
                : snap.contains(shape->shape()->getExtent())) {
                m_selIds.push_back(shape->getID());
                m_id = shape->getID();
            }
        }
        sender->view->shapes()->freeIterator(it);
        sender->view->redraw();
    }
    
    return true;
}

bool MgCmdSelect::isCloneDrag(const MgMotion* sender)
{
    float dist = sender->pointM.distanceTo(sender->startPtM);
    return (!isEditMode(sender->view)
            && m_boxHandle > 16 && sender->pressDrag
            && dist > sender->displayMmToModel(5.f));
}

bool MgCmdSelect::touchEnded(const MgMotion* sender)
{
    // 拖动刚新加的点到起始点时取消新增
    if (m_insertPt && m_clones.size() == 1
        && sender->pointM.distanceTo(m_hit.nearpt) < sender->displayMmToModel(5.f)) {
        m_clones[0]->release();
        m_clones.clear();
    }
    
    int shapeid = 0;
    int handleIndex, handleIndexSrc;
    
    sender->cmds()->getSnap()->getSnappedHandle(shapeid, handleIndex, handleIndexSrc);
    
    applyCloneShapes(sender->view, true, isCloneDrag(sender));
    sender->cmds()->getSnap()->clearSnap();
    
    m_insertPt = false;
    m_hit.nearpt = sender->pointM;
    m_boxHandle = 99;
    
    if (isEditMode(sender->view) && m_handleIndex > 0) {
        m_handleIndex = hitTestHandles(getShape(m_selIds[0], sender), 
                                       sender->pointM, sender);
        sender->view->redraw();
    }
    if (m_boxsel) {
        m_boxsel = false;
        if (!m_selIds.empty())
            sender->view->selectionChanged();
    }
    if (!m_selIds.empty()) {
        sender->view->getCmdSubject()->onSelectTouchEnded(sender, m_id, handleIndexSrc, shapeid, handleIndex,
                                     (int)m_selIds.size(), &m_selIds.front());
    }
    
    return sender->switchGesture || longPress(sender);
}

void MgCmdSelect::cloneShapes(MgView* view)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, view);
    for (std::vector<MgShape*>::iterator it = m_clones.begin();
         it != m_clones.end(); ++it) {
        (*it)->release();
    }
    m_clones.clear();
    
    for (sel_iterator its = m_selIds.begin(); its != m_selIds.end(); ++its) {
        MgShape* shape = view->shapes()->findShape(*its);
        if (shape) {
            shape = shape->cloneShape();
            if (shape)
                m_clones.push_back(shape);
        }
    }
}

bool MgCmdSelect::applyCloneShapes(MgView* view, bool apply, bool addNewShapes)
{
    bool changed = false;
    bool cloned = !m_clones.empty();
    size_t i;
    
    if (apply) {
        apply = false;
        for (i = 0; i < m_clones.size() && !apply; i++) {
            MgShape* shape = view->shapes()->findShape(m_clones[i]->getID());
            if (shape && !shape->equals(*(m_clones[i]))) {
                apply = true;
            }
        }
    }
    
    if (!apply && !m_clones.empty()) {
        MgShapesLock locker(MgShapesLock::ReadOnly, view);
        for (i = 0; i < m_clones.size(); i++) {
            m_clones[i]->release();
            m_clones[i] = NULL;
        }
        m_clones.clear();
    }
    else if (!m_clones.empty()) {
        MgShapesLock locker(addNewShapes ? MgShapesLock::Add
            : MgShapesLock::Edit, view);
        if (addNewShapes) {
            m_selIds.clear();
            m_id = 0;
        }
        for (i = 0; i < m_clones.size(); i++) {
            if (addNewShapes) {
                MgShape* newsp = view->shapes()->addShape(*(m_clones[i]));
                if (newsp) {
                    view->shapeAdded(newsp);
                    m_selIds.push_back(newsp->getID());
                    m_id = newsp->getID();
                    changed = true;
                }
            }
            else {
                MgShape* shape = view->shapes()->findShape(m_clones[i]->getID());
                if (shape) {
                    shape->copy(*m_clones[i]);
                    shape->shape()->update();
                    changed = true;
                }
            }
            
            m_clones[i]->release();
            m_clones[i] = NULL;
        }
        m_clones.clear();
    }
    if (changed) {
        view->regenAll();
        if (addNewShapes)
            view->selectionChanged();
    }
    else {
        view->redraw();
    }
    m_boxsel = false;
    
    return changed || cloned;
}

MgSelState MgCmdSelect::getSelectState(MgView* view)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, view);
    MgSelState state = kMgSelNone;
    
    if (isEditMode(view)) {
        MgShape* shape = view->shapes()->findShape(m_id);
        state = m_handleIndex > 0 && shape && shape->shape()->isKindOf(MgBaseLines::Type()) ?
            kMgSelVertex : kMgSelVertexes;
    }
    else if (!m_selIds.empty()) {
        state = m_selIds.size() > 1 ? kMgSelMultiShapes : kMgSelOneShape;
    }
    
    return state;
}

int MgCmdSelect::getSelectType(MgView* view)
{
    int n = getSelection(view, 0, NULL);
    int type = 0;
    std::vector<MgShape*> arr(n, (MgShape*)0);

    if (n > 0) {
        n = getSelection(view, n, (MgShape**)&arr.front());
        for (int i = 0; i < n; i++) {
            if (type == 0) {
                type = arr[i]->shapec()->getType();
            }
            else if (type != arr[i]->shapec()->getType()) {
                type = kMgShapeMultiType;
                break;
            }
        }
    }

    return type;
}

bool MgCmdSelect::selectAll(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, sender->view);
    size_t oldn = m_selIds.size();
    void* it = NULL;
    
    m_selIds.clear();
    m_handleIndex = 0;
    m_rotateHandle = 0;
    m_insertPt = false;
    m_boxsel = false;
    m_hit.segment = -1;
    
    for (MgShape* shape = sender->view->shapes()->getFirstShape(it);
         shape; shape = sender->view->shapes()->getNextShape(it)) {
        m_selIds.push_back(shape->getID());
        m_id = shape->getID();
    }
    sender->view->shapes()->freeIterator(it);
    sender->view->redraw();

    if (oldn != m_selIds.size() || !m_selIds.empty()) {
        sender->view->selectionChanged();
    }
    longPress(sender);
    
    return oldn != m_selIds.size();
}

bool MgCmdSelect::deleteSelection(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Remove, sender->view);
    MgShape* shape = (m_selIds.empty() ? NULL
                      : sender->view->shapes()->findShape(m_selIds.front()));
    int count = 0;
    
    if (shape && sender->view->shapeWillDeleted(shape)) {        
        applyCloneShapes(sender->view, false);

        for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
            shape = sender->view->shapes()->findShape(*it);
            if (shape && sender->view->removeShape(shape)) {
                shape->release();
                count++;
            }
        }
        
        m_selIds.clear();
        m_id = 0;
        m_handleIndex = 0;
        m_rotateHandle = 0;
    }
    
    if (count > 0) {
        sender->view->regenAll();
        sender->view->selectionChanged();
    }
    
    return count > 0;
}

bool MgCmdSelect::groupSelection(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Remove, sender->view);
    MgShape* shape = (m_selIds.empty() ? NULL
                      : sender->view->shapes()->findShape(m_selIds.front()));
    int count = 0;
    
    if (shape && m_selIds.size() > 1
        && sender->view->shapeWillDeleted(shape)) {        
        applyCloneShapes(sender->view, false);

        MgShape* newgroup = sender->view->shapes()->addShapeByType(
            sender->view->getShapeFactory(), MgGroup::Type());
        MgGroup* group = (MgGroup*)newgroup->shape();

        for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
            shape = sender->view->shapes()->findShape(*it);
            if (group->addShape(shape)) {
                count++;
            }
        }
        group->update();
        
        m_id = newgroup->getID();
        m_selIds.clear();
        m_selIds.push_back(m_id);
        m_handleIndex = 0;
        m_rotateHandle = 0;
    }
    
    if (count > 0) {
        sender->view->regenAll();
        sender->view->selectionChanged();
        longPress(sender);
    }
    
    return count > 0;
}

bool MgCmdSelect::ungroupSelection(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Remove, sender->view);
    MgShape* shape = (m_selIds.empty() ? NULL
                      : sender->view->shapes()->findShape(m_selIds.front()));
    int count = 0;
    
    if (shape && sender->view->shapeWillDeleted(shape)) {
        applyCloneShapes(sender->view, false);

        for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
            shape = sender->view->shapes()->findShape(*it);
            if (shape && shape->shape()->isKindOf(MgGroup::Type())) {
                if (sender->view->shapeCanUngroup(shape)) {
                    MgGroup* group = (MgGroup*)shape->shape();
                    
                    group->shapes()->moveAllShapesTo(shape->getParent());
                    if (sender->view->removeShape(shape)) {
                        shape->release();
                        count++;
                    }
                }
            }
        }
        
        m_id = 0;
        m_selIds.clear();
        m_handleIndex = 0;
        m_rotateHandle = 0;
    }
    
    if (count > 0) {
        sender->view->regenAll();
        sender->view->selectionChanged();
        longPress(sender);
    }
    
    return count > 0;
}

bool MgCmdSelect::cloneSelection(const MgMotion* sender)
{
    cloneShapes(sender->view);
    
    if (!m_clones.empty()) {
        MgShapesLock locker(MgShapesLock::ReadOnly, sender->view);
        float dist = sender->displayMmToModel(10, sender->view->graph());
        for (size_t i = 0; i < m_clones.size(); i++) {
            m_clones[i]->shape()->offset(Vector2d(dist, -dist), -1);
        }
    }
    
    return applyCloneShapes(sender->view, true, true) && longPress(sender);
}

void MgCmdSelect::resetSelection(const MgMotion* sender)
{
    bool has = !m_selIds.empty();
    applyCloneShapes(sender->view, false);
    m_selIds.clear();
    m_id = 0;
    m_handleIndex = 0;
    m_rotateHandle = 0;
    if (has) {
        sender->view->selectionChanged();
    }
}

bool MgCmdSelect::addSelection(const MgMotion* sender, int shapeID)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, sender->view);
    MgShape* shape = sender->view->shapes()->findShape(shapeID);
    
    if (shape && !isSelected(shape))
    {
        m_selIds.push_back(shape->getID());
        m_id = shape->getID();
        m_hit.segment = -1;
        sender->view->redraw();
        sender->view->selectionChanged();
    }

    return shape != NULL;
}

bool MgCmdSelect::deleteVertext(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    MgShape* shape = getSelectedShape(sender);
    bool ret = false;
    
    if (shape && m_handleIndex > 0
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        
        ret = lines->removePoint(m_handleIndex - 1);
        if (ret) {
            shape->shape()->update();
            sender->view->regenAll();
            m_handleIndex = hitTestHandles(shape, m_hit.nearpt, sender);
        }
    }
    m_insertPt = false;
    longPress(sender);
    
    return ret;
}

bool MgCmdSelect::insertVertext(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    MgShape* shape = getSelectedShape(sender);
    bool ret = false;
    
    if (shape && isEditMode(sender->view)
        && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        float dist = m_hit.nearpt.distanceTo(shape->shape()->getPoint(m_hit.segment));
        
        ret = (dist > sender->displayMmToModel(1.f)
               && lines->insertPoint(m_hit.segment, m_hit.nearpt));
        if (ret) {
            shape->shape()->update();
            sender->view->regenAll();
            m_handleIndex = hitTestHandles(shape, m_hit.nearpt, sender);
        }
    }
    m_insertPt = false;
    longPress(sender);
    
    return ret;
}

bool MgCmdSelect::switchClosed(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    MgShape* shape = sender->view->shapes()->findShape(m_id);
    bool ret = false;
    
    if (shape && shape->shape()->isKindOf(MgBaseLines::Type()))
    {
        MgBaseLines *lines = (MgBaseLines *)shape->shape();
        
        lines->setClosed(!lines->isClosed());
        shape->shape()->update();
        sender->view->regenAll();
        longPress(sender);
        ret = true;
    }
    
    return ret;
}

bool MgCmdSelect::isFixedLength(MgView* view)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, view);
    MgShape* shape = view->shapes()->findShape(m_id);
    return shape && shape->shape()->getFlag(kMgFixedLength);
}

bool MgCmdSelect::setFixedLength(const MgMotion* sender, bool fixed)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    int count = 0;
    
    for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
        MgShape* shape = sender->view->shapes()->findShape(*it);
        if (shape && shape->shape()->getFlag(kMgFixedLength) != fixed) {
            shape->shape()->setFlag(kMgFixedLength, fixed);
            count++;
        }
    }
    if (count > 0) {
        sender->view->regenAll();
        longPress(sender);
    }
    
    return count > 0;
}

bool MgCmdSelect::isLocked(MgView* view)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, view);
    MgShape* shape = view->shapes()->findShape(m_id);
    return shape && shape->shape()->getFlag(kMgShapeLocked);
}

bool MgCmdSelect::setLocked(const MgMotion* sender, bool locked)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    int count = 0;
    
    for (sel_iterator it = m_selIds.begin(); it != m_selIds.end(); ++it) {
        MgShape* shape = sender->view->shapes()->findShape(*it);
        if (shape && shape->shape()->getFlag(kMgShapeLocked) != locked) {
            if (locked || sender->view->shapeCanUnlock(shape)) {
                shape->shape()->setFlag(kMgShapeLocked, locked);
                count++;
            }
        }
    }
    if (count > 0) {
        sender->view->regenAll();
        longPress(sender);
    }
    
    return count > 0;
}

bool MgCmdSelect::isEditMode(MgView* view)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, view);
    MgObject* owner = view->shapes()->getOwner();
    if (owner && owner->isKindOf(kMgShapeComposite)) {
        return true;
    }
    return m_editMode && m_selIds.size() == 1;
}

bool MgCmdSelect::setEditMode(const MgMotion* sender, bool editMode)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, sender->view);
    MgObject* owner = sender->view->shapes()->getOwner();

    if (owner && owner->isKindOf(kMgShapeComposite)) {
        editMode = false;
        sender->view->setCurrentShapes(NULL);
    }
    else {
        MgShape* sp = sender->view->shapes()->findShape(m_id);
        if (sp && sp->shapec()->isKindOf(kMgShapeComposite)) {
            MgShapes* shapes = ((MgComposite*)sp->shapec())->shapes();
            sender->view->setCurrentShapes(shapes);
            selectAll(sender);
        }
    }
    m_editMode = editMode;
    m_handleIndex = 0;
    m_rotateHandle = 0;
    sender->view->redraw();
    longPress(sender);
    
    return true;
}

bool MgCmdSelect::overturnPolygon(const MgMotion* sender)
{
    MgShapesLock locker(MgShapesLock::Edit, sender->view);
    MgShape* shape = getShape(m_id, sender);
    
    if (shape && locker.locked()) {
        Point2d cen(shape->shapec()->getExtent().center());
        shape->shape()->transform(Matrix2d::mirroring(cen, Vector2d(0, 1)));
        shape->shape()->update();
        sender->view->regenAll();
        longPress(sender);
        return true;
    }
    
    return false;
}

bool MgCmdSelect::twoFingersMove(const MgMotion* sender)
{
    if (sender->gestureState == kMgGesturePossible) {
        return !m_selIds.empty() && !mgIsZero(sender->distanceM());  // 选择了图形，且双指未重合
    }
    if (sender->gestureState == kMgGestureBegan) {
        if (m_selIds.empty()) {                     // 没有选择图形
            return false;
        }
        cloneShapes(sender->view);
        return m_clones.size() == m_selIds.size() && !mgIsZero(sender->distanceM());
    }
    if (sender->gestureState == kMgGestureMoved    // 正在移动
             && !mgIsZero(sender->distanceM())) {
        for (size_t i = 0; i < m_clones.size(); i++) {
            MgBaseShape* shape = m_clones[i]->shape();
            MgShape* basesp = getShape(m_selIds[i], sender);
            
            if (!basesp || shape->getFlag(kMgShapeLocked))
                continue;
            shape->copy(*basesp->shape());                  // 先重置为原始形状
            
            float dist0 = sender->startDistanceM();         // 起始触点距离
            float a0 = (sender->startPt2M - sender->startPtM).angle2(); // 起始触点角度
            Matrix2d mat (Matrix2d::translation(sender->pointM - sender->startPtM));    // 平移起点
            
            if ((m_editMode || shape->isKindOf(kMgShapeImage))
                && !shape->getFlag(kMgFixedLength)) {       // 比例放缩
                
                float a = fabsf(a0) / _M_PI_2;              // [0,2]
                if (!canRotate(basesp, sender) && fabsf(a - floorf(a + 0.5f)) < 0.3f) {
                    float d0x = fabsf(sender->startPtM.x - sender->startPt2M.x);
                    float d0y = fabsf(sender->startPtM.y - sender->startPt2M.y);
                    float d1x = fabsf(sender->pointM.x - sender->point2M.x);
                    float d1y = fabsf(sender->pointM.y - sender->point2M.y);
                    bool vert = mgRound(a) % 2 == 1;
                    float sx = (vert || mgIsZero(d0x) || mgIsZero(d1x)) ? 1.f : d1x / d0x;
                    float sy = (!vert || mgIsZero(d0y) || mgIsZero(d1y)) ? 1.f : d1y / d0y;
                    
                    // 忽略平移，以初始中点为中心单方向放缩
                    mat = Matrix2d::scaling(sx, sy, sender->startCenterM() / 2);
                }
                else {
                    mat *= Matrix2d::scaling(sender->distanceM() / dist0, sender->pointM);
                }
            }
            if (canRotate(basesp, sender)) {            // 以新起点为中心旋转
                a0 = (sender->point2M - sender->pointM).angle2() - a0;
                a0 = mgbase::deg2Rad(mgbase::roundReal(mgbase::rad2Deg(a0), 0));  // 整度变化
                mat *= Matrix2d::rotation(a0, sender->pointM);
            }
            
            shape->transform(mat);                      // 应用变形矩阵
            
            if (m_clones.size() == 1) {
                Vector2d snapvec(snapPoint(sender, m_clones[i]) - sender->pointM);
                shape->offset(snapvec, -1);             // 再从当前点拖到捕捉点
            }
            shape->update();
        }
        sender->view->redraw();
    }
    else {
        applyCloneShapes(sender->view, sender->gestureState == kMgGestureEnded);
        if (!sender->switchGesture) {
            longPress(sender);
        }
    }
    
    return true;
}
