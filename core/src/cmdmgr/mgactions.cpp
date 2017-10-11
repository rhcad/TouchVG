// mgactions.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcmdmgr_.h"
#include "mgselect.h"
#include "mgbasicsps.h"
#include "mglog.h"
#include "cmdsubject.h"

bool MgCmdManagerImpl::showInDrawing(const MgMotion* sender, const MgShape* shape)
{
    return showInSelect(sender, kMgSelDraw, shape, Box2d(sender->pointM, 0, 0));
}

bool MgCmdManagerImpl::showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox)
{
    if ((shape && shape->shapec()->getFlag(kMgNoAction))
        || sender->view->getCmdSubject()->selectActionsNeedHided(sender)) {
        return false;
    }
    
    int actions[12] = { 0 };
    int n = 0;
    bool issplines = (shape && shape->shapec()->isKindOf(kMgShapeSplines));
    bool isLines = (shape && shape->shapec()->isKindOf(kMgShapeLines));
    bool locked = shape && shape->shapec()->isLocked();
    bool fixedLength = shape && shape->shapec()->getFlag(kMgFixedLength);
    
    switch (selState) {
        case kMgSelNone:
            break;
            
        case kMgSelMultiShapes:
            if (shape && shape->getParent()
                && shape->getParent()->getOwner()->isKindOf(kMgShapeDoc)) {
                actions[n++] = kMgActionGroup;
            }
        case kMgSelOneShape:
            if (!locked && shape && !shape->shapec()->getFlag(kMgNoDel)) {
                actions[n++] = kMgActionDelete;
            }
            if (shape && !shape->shapec()->getFlag(kMgNoClone)) {
                actions[n++] = kMgActionClone;
            }
            if (!locked && shape
                && (shape->shapec()->isKindOf(kMgShapeImage)
                    || shape->shapec()->isKindOf(kMgShapeLine))) {
                actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            }
            if (shape) {
                actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            }
            if (selState == kMgSelOneShape && !locked) {
                actions[n++] = kMgActionEditVertex;
            }
            else {
                //actions[n++] = kMgActionSelReset;
            }
            if (selState == kMgSelOneShape && shape
                && !shape->shapec()->getFlag(kMgRotateDisnable)
                && !shape->shapec()->isLocked()
                && (shape->shapec()->isKindOf(kMgShapeParallel)
                    || shape->shapec()->isKindOf(kMgShapeBaseLines)
                    || shape->shapec()->isKindOf(kMgShapeComposite)) ) {
                actions[n++] = kMgActionOverturn;
            }
            break;
            
        case kMgSelVertexes:
            if ((issplines || isLines) && shape && !shape->shapec()->isLocked()) {
                if (shape->shapec()->getFlag(kMgCanAddVertex)) {
                    //actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                    actions[n++] = kMgActionAddVertex;
                }
            }
            if (!locked && shape && shape->shapec()->isKindOf(kMgShapeLine)) {
                actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            }
            actions[n++] = kMgActionHideVertex;
            break;
            
        case kMgSelVertex:
            if ((issplines || isLines) && shape && !shape->shapec()->isLocked()) {
                if (shape->shapec()->getFlag(kMgCanAddVertex)
                    && shape->getPointCount() > 3) {
                    //actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                    actions[n++] = kMgActionDelVertex;
                }
            }
            if (!locked && shape && shape->shapec()->isKindOf(kMgShapeLine)) {
                actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            }
            //actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            actions[n++] = kMgActionHideVertex;
            break;
            
        case kMgSelDraw:
            break;
    }
    
    if (selState > kMgSelNone && selState <= kMgSelVertex && shape) {
        mgvector<int> arr(actions, sizeof(actions)/sizeof(actions[0]));
        n = sender->view->getCmdSubject()->addShapeActions(sender, arr, n, shape);
        for (int i = 0; i < n; i++) {
            actions[i] = arr.get(i);
        }

        if (sender->view->shapeCanUngroup(shape)) {
            actions[n++] = kMgActionUngroup;
        }
    }
    
    actions[n++] = 0;
    
    if (!selbox.isEmpty()) {
        sender->view->redraw();
    }
    
    Box2d box(selbox.isEmpty() ? Box2d(sender->pointM, 0, 0) : selbox);
    box *= sender->view->xform()->modelToDisplay();
    
    return (sender->view->showContextActions(selState, actions, box, shape)
            && sender->view->isContextActionsVisible());
}

bool MgCmdManagerImpl::doAction(const MgMotion* sender, int action)
{
    MgView* view = sender->view;
    MgSelection *sel = getSelection();
    bool ret = sender->view->getCmdSubject()->doAction(sender, action);
    
    if (ret) {
        action = -action;
    }
    switch (action) {
        case kMgActionSelAll:
            ret = sel && sel->selectAll(sender);
            break;
            
        case kMgActionSelReset:
            if (sel) sel->resetSelection(sender);
            break;
            
        case kMgActionDraw:
            ret = setCommand(sender, "@draw", NULL);
            break;
            
        case kMgActionCancel:
            ret = setCommand(sender, "select", NULL);
            break;
            
        case kMgActionDelete:
            ret = sel && sel->deleteSelection(sender);
            break;
            
        case kMgActionClone:
            ret = sel && sel->cloneSelection(sender);
            break;

        case kMgActionGroup:
            ret = sel && sel->groupSelection(sender);
            break;

        case kMgActionUngroup:
            ret = sel && sel->ungroupSelection(sender);
            break;
            
        case kMgActionFixedLength:
        case kMgActionFreeLength:
            ret = sel && sel->setFixedLength(sender, !sel->isFixedLength(view));
            break;
            
        case kMgActionLocked:
        case kMgActionUnlocked:
            ret = sel && sel->setLocked(sender, !sel->isLocked(view));
            break;
            
        case kMgActionEditVertex:
        case kMgActionHideVertex:
            ret = sel && sel->setEditMode(sender, !sel->isEditMode(view));
            break;
            
        case kMgActionClosed:
        case kMgActionOpened:
            ret = sel && sel->switchClosed(sender);
            break;
            
        case kMgActionAddVertex:
            ret = sel && sel->insertVertex(sender);
            break;
            
        case kMgActionDelVertex:
            ret = sel && sel->deleteVertex(sender);
            break;
            
        case kMgActionOverturn:
            ret = sel && sel->overturnPolygon(sender);
            break;
            
        default: {
            MgCommand* cmd = getCommand();
            
            action = action < 0 ? -action : action;
            ret = ret || (cmd && cmd->doContextAction(sender, action));
            
            if (!ret && cmd && !cmd->isDrawingCommand()) {
                ret = sender->view->getCmdSubject()->doEndAction(sender, action);
                cmd = findCommand(_drawcmd.c_str());
                ret = ret || (cmd && cmd->doContextAction(sender, action));
            }
            break;
        }
    }
    LOGD("doAction(%d) %s", action, ret ? "ok" : "fail");
    
    return ret;
}
