//! \file mgcmdmgr.h
//! \brief 定义命令管理器接口 MgCmdManager
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMDMANAGER_H_
#define TOUCHVG_CMDMANAGER_H_

#include "mgcmd.h"

struct MgSelection;

//! 命令管理器接口
/*! \ingroup CORE_COMMAND
    \interface MgCmdManager
    \see mgRegisterCommand(), MgShapeT<T>::registerCreator()
*/
struct MgCmdManager {

#ifndef SWIG
    //! 注册外部命令, 为NULL则取消注册
    virtual bool registerCommand(const char* name, MgCommand* (*creator)()) = 0;
#endif

    virtual void release() = 0;                             //!< 销毁管理器
    
    virtual const char* getCommandName() = 0;               //!< 得到当前命令名称
    virtual MgCommand* getCommand() = 0;                    //!< 得到当前命令
    virtual MgCommand* findCommand(const char* name) = 0;   //!< 查找命令
    virtual bool setCommand(const MgMotion* sender,
        const char* name, MgStorage* s) = 0;                //!< 启动命令
    virtual bool cancel(const MgMotion* sender) = 0;        //!< 取消当前命令
    virtual void unloadCommands() = 0;                      //!< 退出时卸载命令
    
    virtual int getNewShapeID() = 0;                        //!< 返回新绘图形的ID
    virtual void setNewShapeID(int sid) = 0;                //!< 设置新绘图形的ID

    //! 返回屏幕毫米长度对应的模型长度，在命令显示函数中使用
    virtual float displayMmToModel(float mm, GiGraphics* gs) = 0;
    //! 返回屏幕毫米长度对应的模型长度
    virtual float displayMmToModel(float mm, const MgMotion* sender) = 0;

#ifndef SWIG
    //! 得到当前选择的图形
    /*!
        \param view 当前操作的视图
        \param count 最多获取多少个图形，为0时返回实际个数
        \param shapes 填充当前选择的图形对象
        \param forChange 是否用于修改，用于修改时将复制临时图形，动态修改完后要调用 dynamicChangeEnded()
        \return 获取多少个图形，或实际个数
    */
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false) = 0;
#endif
    
    //! 结束动态修改，提交或放弃所改的临时图形
    virtual bool dynamicChangeEnded(MgView* view, bool apply) = 0;
    
    //! 返回选择集对象，可能为NULL
    virtual MgSelection* getSelection() = 0;
    
    //! 返回上下文动作分发对象
    virtual MgActionDispatcher* getActionDispatcher() = 0;
    
    //! 执行默认的上下文动作
    virtual bool doContextAction(const MgMotion* sender, int action) = 0;
    
    //! 返回图形特征点捕捉器
    virtual MgSnap* getSnap() = 0;

    //! 返回命令扩展目标对象
    virtual CmdSubject* getCmdSubject() = 0;

    //! 添加一个容纳图像的矩形图形，供 GiCoreView 使用
    virtual MgShape* addImageShape(const MgMotion* sender, const char* name, float width, float height) = 0;
    
    //! 添加一个容纳图像的矩形图形，供 GiCoreView 使用
    virtual MgShape* addImageShape(const MgMotion* sender, const char* name,
                                   float xc, float yc, float w, float h) = 0;

    //! 返回选择包络框，显示坐标，供 GiCoreView 使用
    virtual void getBoundingBox(Box2d& box, const MgMotion* sender) = 0;
};

#endif // TOUCHVG_CMDMANAGER_H_
