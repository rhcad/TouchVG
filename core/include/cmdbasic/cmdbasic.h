//! \file cmdbasic.h
//! \brief 定义基本绘图命令工厂类 MgBasicCommands
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_BASIC_CMDS_H_
#define TOUCHVG_BASIC_CMDS_H_

struct MgView;

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
