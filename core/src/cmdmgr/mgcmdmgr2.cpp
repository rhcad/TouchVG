// mgcmdmgr2.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcmdmgr_.h"
#include "mgshapet.h"
#include "mgimagesp.h"
#include "mgcmdselect.h"
#include "mglog.h"
#include "mglocal.h"

MgShape* MgCmdManagerImpl::addImageShape(const MgMotion* sender, 
                                         const char* name, float width, float height)
{
    Box2d rect(sender->view->xform()->getWndRect());
    return addImageShape(sender, name, rect.center().x, rect.center().y, width, height, 0);
}

MgShape* MgCmdManagerImpl::addImageShape(const MgMotion* sender, const char* name,
                                         float xc, float yc, float w, float h, int tag)
{
    if (!name || *name == 0 || w < 1 || h < 1) {
        LOGE("Invalid parameters for addImageShape: %s, %f x %f", name, w, h);
        return NULL;
    }
    
    Vector2d size(Vector2d(w, h) * sender->view->xform()->displayToWorld());
    while (fabsf(size.x) > 100.f || fabsf(size.y) > 100.f) {
        size *= 0.95f;
    }
    size *= sender->view->xform()->worldToDisplay();
    
    Box2d rect(xc - size.x / 2, yc - size.y / 2, xc + size.x / 2, yc + size.y / 2);
    LOGD("addImageShape %s x:%.0f y:%.0f w:%.0f h:%.0f", 
         name, rect.xmin, rect.ymin, rect.width(), rect.height());
    rect *= sender->view->xform()->displayToModel();
    
    MgShapeT<MgImageShape> shape;
    
    shape._context.setFillColor(GiColor::White());    // avoid can't hitted inside
    shape._shape.setName(name);
    shape._shape.setRect2P(rect.leftTop(), rect.rightBottom());
    shape._shape.setImageSize(Vector2d(w, h));
    shape.setTag(tag);
    
    if (sender->view->shapeWillAdded(&shape)) {
        MgShape* newsp = sender->view->shapes()->addShape(shape);
        sender->view->shapeAdded(newsp);
        sender->view->setNewShapeID(newsp->getID());
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
    MgShapes* s = sender->view->shapes();
    MgShapeIterator it(s);
    
    while (const MgShape* shape = it.getNext()) {
        if (!shape->shapec()->isLocked()
            && !shape->shapec()->getFlag(kMgNoDel)
            && shape->shapec()->hitTestBox(snap)) {
            delIds.push_back(shape->getID());
        }
    }
    
    if (!delIds.empty()
        && sender->view->shapeWillDeleted(s->findShape(delIds.front()))) {
        std::vector<int>::iterator i = delIds.begin();
        int n = 0;
        
        for (; i != delIds.end(); ++i) {
            n += sender->view->removeShape(s->findShape(*i));
        }
        if (n > 0) {
            sender->view->regenAll(true);
            char buf[31];
            MgLocalized::formatString(buf, sizeof(buf), sender->view, "@shape_n_deleted", n);
            sender->view->showMessage(buf);
        }
    }
}

static int s_useFinger = -1;

float MgCmdManagerImpl::displayMmToModel(float mm, GiGraphics* gs) const
{
    return gs->xf().displayToModel(s_useFinger ? mm : mm * 0.7f, true);
}

float MgCmdManagerImpl::displayMmToModel(float mm, const MgMotion* sender) const
{
    if (s_useFinger < 0) {
        s_useFinger = sender->view->useFinger() ? 1 : 0;
    }
    return sender->view->xform()->displayToModel(s_useFinger ? mm : mm * 0.7f, true);
}
