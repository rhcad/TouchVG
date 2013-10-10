//! \file mgcmd.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_H_
#define TOUCHVG_CMD_H_

#include "mgview.h"

//! 命令接口
/*! \ingroup CORE_COMMAND
    \interface MgCommand
    \see mgRegisterCommand
*/
struct MgCommand {
    virtual ~MgCommand() {}
    virtual const char* getName() const = 0;                    //!< 返回命令名称
    virtual void release() = 0;                                 //!< 销毁对象
    
    virtual bool cancel(const MgMotion* sender) { return !sender; } //!< 取消命令
    virtual bool initialize(const MgMotion* sender, MgStorage* s) { return sender || s; }    //!< 开始命令
    virtual bool backStep(const MgMotion* sender) { return !sender; }   //!< 回退一步
    
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;  //!< 显示动态图形
    virtual int gatherShapes(const MgMotion* sender, MgShapes* shapes) { //!< 得到动态图形
        return sender && shapes ? 0 : 0; }
    
    virtual bool click(const MgMotion* sender) {    //!< 点击
        return sender->view->useFinger() && longPress(sender); }
    virtual bool doubleClick(const MgMotion* sender) { return !sender; } //!< 双击
    virtual bool longPress(const MgMotion* sender) { return !sender; }  //!< 长按
    virtual bool touchBegan(const MgMotion* sender) { return !sender; } //!< 开始滑动
    virtual bool touchMoved(const MgMotion* sender) { return !sender; } //!< 正在滑动
    virtual bool touchEnded(const MgMotion* sender) { return !sender; } //!< 滑动结束
    virtual bool mouseHover(const MgMotion* sender) { return !sender; } //!< 鼠标掠过
    virtual bool twoFingersMove(const MgMotion* sender) { return !sender; } //!< 双指触摸
    
    virtual bool isDrawingCommand() { return false; }       //!< 是否为绘图命令
    virtual bool isFloatingCommand() { return false; }      //!< 是否可嵌套在其他命令中
    virtual bool doContextAction(const MgMotion* sender, int action) { return !sender && !action; } //!< 执行上下文动作
};

//! 图形列表锁定辅助类
/*! \ingroup CORE_COMMAND
*/
class MgShapesLock
{
    MgLockData* locker;
    int         mode;
public:
    enum { NotNotify = 0x100, ReadOnly = 0,
        Add = 0x1, Remove = 0x2, Edit = 0x4, Load = 0x8 };
    MgShapesLock(int flags, MgView* view, int timeout = 200);
    MgShapesLock(MgView* view, int timeout = 200);
    ~MgShapesLock();

    bool locked() const;
    static bool lockedForRead(MgView* view);
    static bool lockedForWrite(MgView* view);

    int getEditFlags() const;
    void resetEditFlags();
};

//! 动态图形锁定辅助类
/*! \ingroup CORE_COMMAND
*/
class MgDynShapeLock
{
    MgLockData* locker;
    int         mode;
public:
    MgDynShapeLock(bool forWrite, MgView* view, int timeout = 200);
    ~MgDynShapeLock();

    bool locked() const;
    static bool lockedForRead(MgView* view);
    static bool lockedForWrite(MgView* view);
};

#endif // TOUCHVG_CMD_H_
