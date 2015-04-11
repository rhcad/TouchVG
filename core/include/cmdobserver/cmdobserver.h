//! \file cmdobserver.h
//! \brief 定义命令扩展观察者接口 CmdObserver
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMDOBSERVER_H_
#define TOUCHVG_CMDOBSERVER_H_

#include "mgvector.h"

class MgShape;
class MgBaseShape;
class MgMotion;
class GiGraphics;
class MgCommand;
struct MgCmdManager;

//! 命令扩展观察者接口
/*! \ingroup CORE_COMMAND
    \see CmdObserverDefault
 */
struct CmdObserver {
    virtual ~CmdObserver() {}
    
    //! 图形文档内容加载后的通知，Undo/Redo也触发此通知
    virtual void onDocLoaded(const MgMotion* sender, bool forUndo) = 0;

    //! 进入选择命令时的通知
    virtual void onEnterSelectCommand(const MgMotion* sender) = 0;

    //! 视图销毁前、所有命令卸载后的通知，可重载本函数释放命令观察者
    virtual void onUnloadCommands(MgCmdManager* sender) = 0;

    //! 选择命令中的上下文操作是否隐藏的回调通知
    virtual bool selectActionsNeedHided(const MgMotion* sender) = 0;

    //! 对选中的图形增加上下文操作的回调通知
    /*! 重载实现示例: \code
        actions.set(n++, kMgAction3Views); return n;
        \endcode
        \param sender 视图上下文信息
        \param actions 待增加上下文操作码(MgContextAction 或扩展值)的数组
        \param n actions 中已有的元素个数
        \param sp 当前选中的图形
        \return 增加上下文操作后 actions 中已有的元素个数
     */
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
    //! 添加不捕捉的图形, sp为NULL时添加不在静态图形中显示的图形的ID
    virtual void onGatherSnapIgnoredID(const MgMotion* sender, const MgShape* sp,
                                       int* ids, int& i, int n) = 0;
#endif

    virtual void onSelectionChanged(const MgMotion* sender) = 0;               //!< 选择集改变的通知
    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* sp) = 0;    //!< 通知将添加图形
    virtual void onShapeAdded(const MgMotion* sender, MgShape* sp) = 0;        //!< 通知已添加图形
    virtual bool onShapeWillDeleted(const MgMotion* sender, const MgShape* sp) = 0;  //!< 通知将删除图形
    virtual int onShapeDeleted(const MgMotion* sender, const MgShape* sp) = 0;       //!< 通知已删除图形
    virtual bool onShapeCanRotated(const MgMotion* sender, const MgShape* sp) = 0;   //!< 通知是否能旋转图形
    virtual bool onShapeCanTransform(const MgMotion* sender, const MgShape* sp) = 0; //!< 通知是否能对图形变形
    virtual bool onShapeCanUnlock(const MgMotion* sender, const MgShape* sp) = 0;    //!< 通知是否能对图形解锁
    virtual bool onShapeCanUngroup(const MgMotion* sender, const MgShape* sp) = 0;   //!< 通知是否能对成组图形解散
    virtual bool onShapeCanMovedHandle(const MgMotion* sender, const MgShape* sp, int index) = 0; //!< 通知是否能移动点
    virtual void onShapeMoved(const MgMotion* sender, MgShape* sp, int segment) = 0; //!< 通知图形已拖动
    virtual bool onShapeWillChanged(const MgMotion* sender, MgShape* sp, const MgShape* oldsp) = 0; //!< 通知将修改图形
    virtual void onShapeChanged(const MgMotion* sender, MgShape* shape) = 0;   //!< 通知已拖动图形

    virtual MgBaseShape* createShape(const MgMotion* sender, int type) = 0; //!< 创建自定义的图形
    virtual MgCommand* createCommand(const MgMotion* sender, const char* name) = 0; //!< 创建命令
    
    virtual bool onPreGesture(MgMotion* sender) = 0;                        //!< 向命令分发手势前
    virtual void onPostGesture(const MgMotion* sender) = 0;                 //!< 向命令分发手势后
    virtual void onPointSnapped(const MgMotion* sender, const MgShape* sp) = 0; //!< 捕捉到特征点
};

//! 命令扩展观察者接口的默认实现
/*! \ingroup CORE_COMMAND
 */
class CmdObserverDefault : public CmdObserver
{
public:
    CmdObserverDefault() {}
    virtual ~CmdObserverDefault() {}

    virtual void onDocLoaded(const MgMotion* sender, bool forUndo) {}
    virtual void onEnterSelectCommand(const MgMotion* sender) {}
    virtual void onUnloadCommands(MgCmdManager* sender) {}
    virtual bool selectActionsNeedHided(const MgMotion* sender) { return false; }
    virtual bool doAction(const MgMotion* sender, int action) { return false; }
    virtual bool doEndAction(const MgMotion* sender, int action) { return false; }
    virtual void drawInShapeCommand(const MgMotion* sender, MgCommand* cmd, GiGraphics* gs) {}
    virtual void drawInSelectCommand(const MgMotion* sender, const MgShape* sp,
                                     int handleIndex, GiGraphics* gs) {}
    virtual void onSelectionChanged(const MgMotion* sender) {}
    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* sp) { return true; }
    virtual void onShapeAdded(const MgMotion* sender, MgShape* sp) {}
    virtual bool onShapeWillDeleted(const MgMotion* sender, const MgShape* sp) { return true; }
    virtual int onShapeDeleted(const MgMotion* sender, const MgShape* sp) { return 0; }
    virtual bool onShapeCanRotated(const MgMotion* sender, const MgShape* sp) { return true; }
    virtual bool onShapeCanTransform(const MgMotion* sender, const MgShape* sp) { return true; }
    virtual bool onShapeCanUnlock(const MgMotion* sender, const MgShape* sp) { return true; }
    virtual bool onShapeCanUngroup(const MgMotion* sender, const MgShape* sp) { return true; }
    virtual bool onShapeCanMovedHandle(const MgMotion* sender, const MgShape* sp, int index) { return true; }
    virtual void onShapeMoved(const MgMotion* sender, MgShape* sp, int segment) {}
    virtual bool onShapeWillChanged(const MgMotion* sender, MgShape* sp, const MgShape* oldsp) { return true; }
    virtual void onShapeChanged(const MgMotion* sender, MgShape* shape) {}
    virtual MgBaseShape* createShape(const MgMotion* sender, int type) { return (MgBaseShape*)0; }
    virtual MgCommand* createCommand(const MgMotion* sender, const char* name) { return (MgCommand*)0; }
    virtual int addShapeActions(const MgMotion* sender, mgvector<int>& actions,int n, const MgShape* sp) { return n; }
#ifndef SWIG
    virtual void onSelectTouchEnded(const MgMotion* sender, int shapeid,
                                    int handleIndex, int snapid, int snapHandle,
                                    int count, const int* ids) {}
    virtual void onGatherSnapIgnoredID(const MgMotion* sender, const MgShape* sp, int* ids, int& i, int n) {}
#endif
    virtual bool onPreGesture(MgMotion* sender) { return true; }
    virtual void onPostGesture(const MgMotion* sender) {}
    virtual void onPointSnapped(const MgMotion* sender, const MgShape* sp) {}
};

#endif // TOUCHVG_CMDOBSERVER_H_
