//! \file cmdobserver.h
//! \brief 定义命令扩展观察者接口 CmdObserver
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMDOBSERVER_H_
#define TOUCHVG_CMDOBSERVER_H_

#include <mgvector.h>

class MgShape;
class MgBaseShape;
class MgMotion;
class GiGraphics;
struct MgCommand;
struct MgCmdManager;

//! 命令扩展观察者接口
/*! \ingroup CORE_COMMAND
    \see CmdObserverDefault
 */
struct CmdObserver {
    //! 图形文档内容加载后的通知
    virtual void onDocLoaded(const MgMotion* sender) = 0;

    //! 进入选择命令时的通知
    virtual void onEnterSelectCommand(const MgMotion* sender) = 0;

    //! 视图销毁前、所有命令卸载后的通知
    virtual void onUnloadCommands(MgCmdManager* sender) = 0;

    //! 选择命令中的上下文操作是否隐藏的回调通知
    virtual bool selectActionsNeedHided(const MgMotion* sender) = 0;

    //! 对选中的图形增加上下文操作的回调通知
    virtual int addShapeActions(const MgMotion* sender,
        mgvector<int>& actions, int n, const MgShape* sp) = 0;

    //! 执行非内置上下文操作的通知，先于 doEndAction() 被调用
    virtual bool doAction(const MgMotion* sender, int action) = 0;

    //! 在非绘图的命令中执行额外的上下文操作、没有命令响应此操作的通知
    virtual bool doEndAction(const MgMotion* sender, int action) = 0;

    //! 在绘图命令中显示额外内容的通知
    virtual void drawInShapeCommand(const MgMotion* sender, 
        MgCommand* cmd, GiGraphics* gs) = 0;

    //! 在选择命令中显示额外内容的通知
    virtual void drawInSelectCommand(const MgMotion* sender, 
        const MgShape* sp, int handleIndex, GiGraphics* gs) = 0;
#ifndef SWIG
    //! 在选择命令中拖放图形后的通知
    virtual void onSelectTouchEnded(const MgMotion* sender, int shapeid,
        int handleIndex, int snapid, int snapHandle,
        int count, const int* ids) = 0;
#endif

    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* sp) = 0;    //!< 通知将添加图形
    virtual void onShapeAdded(const MgMotion* sender, MgShape* sp) = 0;        //!< 通知已添加图形
    virtual bool onShapeWillDeleted(const MgMotion* sender, MgShape* sp) = 0;  //!< 通知将删除图形
    virtual void onShapeDeleted(const MgMotion* sender, MgShape* sp) = 0;      //!< 通知已删除图形
    virtual bool onShapeCanRotated(const MgMotion* sender, const MgShape* sp) = 0;   //!< 通知是否能旋转图形
    virtual bool onShapeCanTransform(const MgMotion* sender, const MgShape* sp) = 0; //!< 通知是否能对图形变形
    virtual bool onShapeCanUnlock(const MgMotion* sender, const MgShape* sp) = 0;    //!< 通知是否能对图形解锁
    virtual bool onShapeCanUngroup(const MgMotion* sender, const MgShape* sp) = 0;   //!< 通知是否能对成组图形解散
    virtual void onShapeMoved(const MgMotion* sender, MgShape* sp, int segment) = 0;   //!< 通知图形已拖动

    virtual MgBaseShape* createShape(const MgMotion* sender, int type) = 0; //!< 创建自定义的图形
    virtual MgCommand* createCommand(const MgMotion* sender, const char* name) = 0; //!< 创建命令
};

class CmdObserverDefault : public CmdObserver
{
public:
    CmdObserverDefault() {}
    virtual ~CmdObserverDefault() {}

    virtual void onDocLoaded(const MgMotion* sender) {
        _ignore(sender); }
    virtual void onEnterSelectCommand(const MgMotion* sender) {
        _ignore(sender); }
    virtual void onUnloadCommands(MgCmdManager* sender) {
        if (sender) sender = sender; }
    virtual bool selectActionsNeedHided(const MgMotion* sender) {
        return !sender; } // false
    virtual bool doAction(const MgMotion* sender, int action) {
        return !sender && action; } // false
    virtual bool doEndAction(const MgMotion* sender, int action) {
        return !sender && action; } // false
    virtual void drawInShapeCommand(const MgMotion* sender,
        MgCommand* cmd, GiGraphics* gs) { if (sender && cmd) gs = gs; }
    virtual void drawInSelectCommand(const MgMotion* sender, 
        const MgShape* sp, int handleIndex, GiGraphics* gs) {
            if (sender && sp && gs) handleIndex++; }

    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* sp) {
        return sender || sp; } // true
    virtual void onShapeAdded(const MgMotion* sender, MgShape* sp) {
        if (sender) sp = sp; }
    virtual bool onShapeWillDeleted(const MgMotion* sender, MgShape* sp) {
        return sender || sp; } // true
    virtual void onShapeDeleted(const MgMotion* sender, MgShape* sp) {
        if (sender) sp = sp; }
    virtual bool onShapeCanRotated(const MgMotion* sender, const MgShape* sp) {
        return sender || sp; } // true
    virtual bool onShapeCanTransform(const MgMotion* sender, const MgShape* sp) {
        return sender || sp; } // true
    virtual bool onShapeCanUnlock(const MgMotion* sender, const MgShape* sp) {
        return sender || sp; } // true
    virtual bool onShapeCanUngroup(const MgMotion* sender, const MgShape* sp) {
        return sender || sp; } // true
    virtual void onShapeMoved(const MgMotion* sender, MgShape* sp, int segment) {
        if (sender && sp) segment++; }
    virtual MgBaseShape* createShape(const MgMotion* sender, int type) {
        if (sender) type++; return (MgBaseShape*)0; }
    virtual MgCommand* createCommand(const MgMotion* sender, const char* name) {
        if (sender && name) name=name; return (MgCommand*)0; }
    virtual int addShapeActions(const MgMotion*,mgvector<int>&,int n, const MgShape*) {
        return n; }
#ifndef SWIG
    virtual void onSelectTouchEnded(const MgMotion*,int,int,int,int,int,const int*) {}
#endif

private:
    void _ignore(const MgMotion* sender) { if (sender) sender = sender; }
};

#endif // TOUCHVG_CMDOBSERVER_H_
