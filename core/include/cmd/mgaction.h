//! \file mgaction.h
//! \brief 定义上下文动作接口 MgActionDispatcher
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGACTION_H_
#define TOUCHVG_MGACTION_H_

//! 默认上下文动作
/*! \ingroup CORE_COMMAND
 */
typedef enum {
    kMgActionInvalid = 0,
    kMgActionSelAll = 1,            //!< 全选
    kMgActionSelReset = 2,          //!< 重选
    kMgActionDraw = 3,              //!< 绘图
    kMgActionCancel = 4,            //!< 取消
    kMgActionDelete = 5,            //!< 删除
    kMgActionClone = 6,             //!< 克隆
    kMgActionFixedLength = 7,       //!< 定长
    kMgActionFreeLength = 8,        //!< 不定长
    kMgActionLocked = 9,            //!< 锁定
    kMgActionUnlocked = 10,         //!< 解锁
    kMgActionEditVertex = 11,       //!< 编辑顶点
    kMgActionHideVertex = 12,       //!< 隐藏顶点
    kMgActionClosed = 13,           //!< 闭合
    kMgActionOpened = 14,           //!< 不闭合
    kMgActionAddVertex = 15,        //!< 加点
    kMgActionDelVertex = 16,        //!< 删点
    kMgActionGroup = 17,            //!< 成组
    kMgActionUngroup = 18,          //!< 解组
    kMgActionOverturn = 19,         //!< 翻转
    kMgActionCustomized = 100       //!< 定制的起始值
} MgContextAction;

class MgMotion;
class MgShape;
class Box2d;

//! 上下文动作分发接口
/*! \ingroup CORE_COMMAND
    \interface MgActionDispatcher
    \see MgCmdManager, MgContextAction
*/
struct MgActionDispatcher {
    virtual ~MgActionDispatcher() {}
    virtual bool showInSelect(const MgMotion* sender, int selState, const MgShape* shape, const Box2d& selbox) = 0;
    virtual bool showInDrawing(const MgMotion* sender, const MgShape* shape) = 0;
    virtual bool doAction(const MgMotion* sender, int action) = 0;
};

#endif // TOUCHVG_MGACTION_H_
