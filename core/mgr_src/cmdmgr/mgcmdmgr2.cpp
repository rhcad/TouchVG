// mgcmdmgr2.cpp
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr_.h"
#include <mgshapet.h>
#include <mgbasicsp.h>
#include "mgcmdselect.h"
#include <string.h>
#include <mglog.h>

MgShape* MgCmdManagerImpl::addImageShape(const MgMotion* sender, 
                                         const char* name, float width, float height)
{
    Box2d rect(sender->view->xform()->getWndRectW());
    rect *= sender->view->xform()->worldToDisplay();
    return addImageShape(sender, name, rect.center().x, rect.center().y, width, height);
}

MgShape* MgCmdManagerImpl::addImageShape(const MgMotion* sender, const char* name,
                                         float xc, float yc, float w, float h)
{
    if (!name || *name == 0 || w < 1 || h < 1)
        return NULL;
    
    Vector2d size(Vector2d(w, h) * sender->view->xform()->displayToWorld());
    while (fabsf(size.x) > 200.f || fabsf(size.y) > 200.f) {
        size *= 0.95f;
    }
    size *= sender->view->xform()->worldToDisplay();
    
    Box2d rect(xc - size.x / 2, yc - size.y / 2, xc + size.x / 2, yc + size.y / 2);
    LOGD("addImageShape %s x:%.0f y:%.0f w:%.0f h:%.0f", 
         name, rect.xmin, rect.ymin, rect.width(), rect.height());
    rect *= sender->view->xform()->displayToModel();
    
    MgShapeT<MgImageShape> shape;
    MgImageShape* imagesp = (MgImageShape*)shape.shape();
    
    shape.context()->setLineStyle(kGiLineNull);         // 默认没有边框
    shape.context()->setFillColor(GiColor::White());    // 设为实填充，避免在中心无法点中
    imagesp->setName(name);
    imagesp->setRect2P(rect.leftTop(), rect.rightBottom());
    
    MgShapesLock locker(MgShapesLock::Add, sender->view);
    if (sender->view->shapeWillAdded(&shape)) {
        MgShape* newsp = sender->view->shapes()->addShape(shape);
        sender->view->shapeAdded(newsp);
        
        sender->view->setNewShapeID(newsp->getID());
        sender->toSelectCommand();
        
        return newsp;
    }
    
    return NULL;
}

void MgCmdManagerImpl::getBoundingBox(Box2d& box, const MgMotion* sender)
{
    MgCommand* cmd = sender->cmds()->getCommand();
    Box2d selbox;
    
    if (cmd && strcmp(cmd->getName(), MgCmdSelect::Name()) == 0) {
        MgCmdSelect* sel = (MgCmdSelect*)cmd;
        selbox = sel->getBoundingBox(sender);
    }
    
    box = selbox.isEmpty() ? Box2d(sender->pointM, 0, 0) : selbox;
    box *= sender->view->xform()->modelToDisplay();
    box.normalize();
}

void MgCmdManagerImpl::eraseWnd(const MgMotion* sender)
{
    Box2d snap(sender->view->xform()->getWndRectM());
    std::vector<int> delIds;
    void *it = NULL;
    MgShapes* s = sender->view->shapes();
    
    for (MgShape* shape = s->getFirstShape(it); shape; shape = s->getNextShape(it)) {
        if (shape->shape()->hitTestBox(snap)) {
            delIds.push_back(shape->getID());
        }
    }
    s->freeIterator(it);
    
    if (!delIds.empty()
        && sender->view->shapeWillDeleted(s->findShape(delIds.front()))) {
        MgShapesLock locker(MgShapesLock::Remove, sender->view);
        
        for (std::vector<int>::iterator i = delIds.begin(); i != delIds.end(); ++i) {
            MgShape* shape = s->findShape(*i);
            if (shape && sender->view->removeShape(shape)) {
                shape->release();
            }
        }
        sender->view->regenAll();
    }
}

static int s_useFinger = -1;

//! 得到屏幕上的毫米长度对应的模型长度.
/*! 对于显示比例不同的多个视图，本函数可确保在相应视图中毫米长度相同.
    \ingroup CORE_COMMAND
 */
float MgCmdManagerImpl::displayMmToModel(float mm, GiGraphics* gs)
{
    return gs->xf().displayToModel(s_useFinger ? mm : mm / 2.f, true);
}

//! 得到屏幕上的毫米长度对应的模型长度.
/* \ingroup CORE_COMMAND
 */
float MgCmdManagerImpl::displayMmToModel(float mm, const MgMotion* sender)
{
    if (s_useFinger < 0) {
        s_useFinger = sender->view->useFinger() ? 1 : 0;
    }
    return sender->view->xform()->displayToModel(s_useFinger ? mm : mm / 2.f, true);
}
