//! \file mgcmdmgr_.h
//! \brief 定义命令管理器实现类 MgCmdManagerImpl
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_MANAGER_IMPL_H_
#define TOUCHVG_CMD_MANAGER_IMPL_H_

#include <mgcmdmgr.h>
#include <mgsnap.h>
#include <mgaction.h>
#include <map>
#include <string>

class SnapItem;

//! 命令管理器实现类
/*! \ingroup CORE_COMMAND
*/
class MgCmdManagerImpl
    : public MgCmdManager
    , public MgSnap
    , public MgActionDispatcher
{
public:
    MgCmdManagerImpl();
    virtual ~MgCmdManagerImpl();
    
private:
    virtual bool registerCommand(const char* name, MgCommand* (*creator)());
    virtual void release() { delete this; }
    virtual const char* getCommandName();
    virtual MgCommand* getCommand();
    virtual MgCommand* findCommand(const char* name);
    virtual bool setCommand(const MgMotion* sender,
        const char* name, MgStorage* s);
    virtual bool cancel(const MgMotion* sender);
    virtual void unloadCommands();
    virtual int getNewShapeID() { return _newShapeID; }
    virtual void setNewShapeID(int sid) { _newShapeID = sid; }
    virtual float displayMmToModel(float mm, GiGraphics* gs);
    virtual float displayMmToModel(float mm, const MgMotion* sender);
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false);
    virtual bool dynamicChangeEnded(MgView* view, bool apply);
    virtual MgSelection* getSelection();
    virtual MgActionDispatcher* getActionDispatcher();
    virtual bool doContextAction(const MgMotion* sender, int action);
    virtual MgSnap* getSnap();
    virtual CmdSubject* getCmdSubject();
    virtual MgShape* addImageShape(const MgMotion* sender, const char* name, float width, float height);
    virtual void getBoundingBox(Box2d& box, const MgMotion* sender);
    
private:
    virtual bool drawSnap(const MgMotion* sender, GiGraphics* gs);
    virtual Point2d snapPoint(const MgMotion* sender, 
        const Point2d& orignPt, const MgShape* shape,
        int hotHandle, int ignoreHandle = -1, const int* ignoreids = NULL);
    virtual int getSnappedType();
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt);
    virtual bool getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc);
    virtual void clearSnap();
    
    virtual bool showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox);
    virtual bool showInDrawing(const MgMotion* sender, const MgShape* shape);
    virtual bool doAction(const MgMotion* sender, int action);
    
private:
    void eraseWnd(const MgMotion* sender);
    void checkResult(SnapItem* arr);

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    CMDS            _cmds;
    std::string     _cmdname;
    std::string     _drawcmd;
    int             _newShapeID;
    
    Point2d         _ptSnap;
    Point2d         _snapBase[2];
    int             _snapType[2];
    int             _snapShapeId;
    int             _snapHandle;
    int             _snapHandleSrc;
};

#endif // TOUCHVG_CMD_MANAGER_IMPL_H_
