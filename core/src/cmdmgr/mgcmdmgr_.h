//! \file mgcmdmgr_.h
//! \brief 定义命令管理器实现类 MgCmdManagerImpl
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_MANAGER_IMPL_H_
#define TOUCHVG_CMD_MANAGER_IMPL_H_

#include "mgcmdmgr.h"
#include "mgsnap.h"
#include "mgaction.h"
#include <map>
#include <string>

struct SnapItem;
class CmdSubjectImpl;

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
    virtual const char* getCommandName() const;
    virtual const char* getCommandName(int index) const;
    virtual MgCommand* getCommand() const;
    virtual MgCommand* findCommand(const char* name);
    virtual bool setCommand(const MgMotion* sender,
        const char* name, MgStorage* s);
    virtual bool switchCommand(const MgMotion* sender);
    virtual bool cancel(const MgMotion* sender);
    virtual void unloadCommands();
    virtual int getNewShapeID() const { return _newShapeID; }
    virtual void setNewShapeID(int sid) { _newShapeID = sid; }
    virtual float displayMmToModel(float mm, GiGraphics* gs) const;
    virtual float displayMmToModel(float mm, const MgMotion* sender) const;
    virtual int getSelection(MgView* view, int count, const MgShape** shapes);
    virtual int getSelectionForChange(MgView* view, int count, MgShape** shapes);
    virtual bool dynamicChangeEnded(MgView* view, bool apply);
    virtual MgSelection* getSelection();
    virtual MgActionDispatcher* getActionDispatcher();
    virtual bool doContextAction(const MgMotion* sender, int action);
    virtual MgSnap* getSnap();
    virtual CmdSubject* getCmdSubject();
    virtual MgShape* addImageShape(const MgMotion* sender, const char* name,
                                   float width, float height);
    virtual MgShape* addImageShape(const MgMotion* sender, const char* name,
                                   float xc, float yc, float w, float h, int tag);
    virtual void getBoundingBox(Box2d& box, const MgMotion* sender);
    
private:
    virtual bool drawSnap(const MgMotion* sender, GiGraphics* gs) const;
    virtual bool drawPerpMark(GiGraphics* gs, const GiContext& ctx,
                              const Point2d& a, const Point2d& b,
                              const Point2d& perp, const Point2d& c, float len) const;
    virtual Point2d snapPoint(const MgMotion* sender, 
        const Point2d& orignPt, const MgShape* shape,
        int hotHandle, int ignoreHandle = -1, const int* ignoreids = NULL);
    virtual int getSnapOptions(MgView* view) const;
    virtual void setSnapOptions(MgView* view, int bits);
    virtual int getSnappedType() const;
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt) const;
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt,
                                Point2d& startPt, Point2d& guildPt) const;
    virtual void setIgnoreStartPoint(const Point2d& pt);
    virtual bool getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc) const;
    virtual void clearSnap(const MgMotion* sender);
    
    virtual bool showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox);
    virtual bool showInDrawing(const MgMotion* sender, const MgShape* shape);
    virtual bool doAction(const MgMotion* sender, int action);
    
private:
    void eraseWnd(const MgMotion* sender);
    void checkResult(SnapItem arr[3], int hotHandle);
    void freeSubject();
    void drawPerpMark(const MgMotion* sender, GiGraphics* gs, GiContext& ctx) const;

private:
    typedef std::map<std::string, MgCommand*> CMDS;
    typedef std::map<std::string, MgCommand* (*)()> Factories;
    Factories       _factories;
    CMDS            _cmds;
    std::string     _cmdname;
    std::string     _drawcmd;
    int             _newShapeID;
    CmdSubjectImpl* _subject;
    
    Point2d         _ignoreStart;
    Point2d         _ptSnap;
    Point2d         _startpt;
    Point2d         _guildpt;
    Point2d         _snapBase[2];
    int             _snapType[2];
    int             _snapShapeId;
    int             _snapHandle;
    int             _snapHandleSrc;
};

#endif // TOUCHVG_CMD_MANAGER_IMPL_H_
