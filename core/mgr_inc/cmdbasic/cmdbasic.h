//! \file cmdbasic.h
//! \brief 定义基本绘图命令工厂类 MgBasicCommands
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_BASIC_CMDS_H_
#define TOUCHVG_BASIC_CMDS_H_

class MgView;

//! 基本绘图命令工厂类
/*! \ingroup CORE_COMMAND
*/
class MgBasicCommands
{
public:
    //! 登记绘图命令
    static void registerCmds(MgView* view);
};

#endif // TOUCHVG_BASIC_CMDS_H_
