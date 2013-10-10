// mgactions.cpp: 实现上下文动作分发接口
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr_.h"
#include <mgselect.h>
#include <mgshapetype.h>
#include <mglog.h>
#include <cmdsubject.h>

bool MgCmdManagerImpl::showInDrawing(const MgMotion* sender, const MgShape* shape)
{
    return showInSelect(sender, kMgSelDraw, shape, Box2d(sender->pointM, 0, 0));
}

bool MgCmdManagerImpl::showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox)
{
    if (sender->view->getCmdSubject()->selectActionsNeedHided(sender)) {
        return false;
    }
    
    int actions[12];
    int n = 0;
    bool isslines = (shape && selState == kMgSelOneShape
                     && shape->shapec()->isKindOf(kMgShapeSplines));
    bool isOpenLines = (shape && selState == kMgSelOneShape
                        && shape->shapec()->isKindOf(kMgShapeLines)
                        && !shape->shapec()->isClosed());
    bool locked = shape && shape->shapec()->getFlag(kMgShapeLocked);
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
            if (!locked && !sender->view->useFinger()) {
                actions[n++] = kMgActionDelete;
            }
            actions[n++] = kMgActionClone;
            if (!locked && shape
                && (shape->shapec()->isKindOf(kMgShapeImage)
                    || shape->shapec()->isKindOf(kMgShapeLine))) {
                actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            }
            actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            if (selState == kMgSelOneShape && !locked) {
                actions[n++] = kMgActionEditVertex;
            }
            else {
                //actions[n++] = kMgActionSelReset;
            }
            if (selState == kMgSelOneShape
                && shape && shape->shapec()->isClosed()
                && !shape->shapec()->getFlag(kMgRotateDisnable)
                && !shape->shapec()->getFlag(kMgShapeLocked)
                && (shape->shapec()->isKindOf(kMgShapeParallel)
                    || shape->shapec()->isKindOf(kMgShapeLines)) ) {
                actions[n++] = kMgActionOverturn;
            }
            break;
            
        case kMgSelVertexes:
            if ((isslines || isOpenLines)
                && shape && !shape->shapec()->getFlag(kMgShapeLocked)) {
                //actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                actions[n++] = kMgActionAddVertex;
            }
            if (!locked && shape && shape->shapec()->isKindOf(kMgShapeLine)) {
                actions[n++] = fixedLength ? kMgActionFreeLength : kMgActionFixedLength;
            }
            //actions[n++] = locked ? kMgActionUnlocked : kMgActionLocked;
            actions[n++] = kMgActionHideVertex;
            break;
            
        case kMgSelVertex:
            if ((isslines || isOpenLines)
                && shape && !shape->shapec()->getFlag(kMgShapeLocked)) {
                //actions[n++] = closed ? kMgActionOpened : kMgActionClosed;
                actions[n++] = kMgActionDelVertex;
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

        if (shape->shapec()->isKindOf(kMgShapeGroup)
            && sender->view->shapeCanUngroup(shape)) {
            actions[n++] = kMgActionUngroup;
        }
    }
    
    actions[n++] = 0;
    
    if (!selbox.isEmpty()) {
        sender->view->redraw();
    }
    
    Box2d box(selbox.isEmpty() ? Box2d(sender->pointM, 0, 0) : selbox);
    box *= sender->view->xform()->modelToDisplay();
    
    return sender->view->showContextActions(selState, actions, box, shape);
}

bool MgCmdManagerImpl::doAction(const MgMotion* sender, int action)
{
    MgView* view = sender->view;
    MgSelection *sel = getSelection();
    bool ret = false;
    
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
            ret = sel && sel->insertVertext(sender);
            break;
            
        case kMgActionDelVertex:
            ret = sel && sel->deleteVertext(sender);
            break;
            
        case kMgActionOverturn:
            ret = sel && sel->overturnPolygon(sender);
            break;
            
        default: {
            ret = sender->view->getCmdSubject()->doAction(sender, action);
            MgCommand* cmd = getCommand();
            ret = ret || (cmd && cmd->doContextAction(sender, action));
            
            if (!ret && cmd && !cmd->isDrawingCommand()) {
                ret = sender->view->getCmdSubject()->doEndAction(sender, action);
                cmd = findCommand(_drawcmd.c_str());
                ret = ret || (cmd && cmd->doContextAction(sender, action));
            }
            break;
        }
    }
    
    return ret;
}
