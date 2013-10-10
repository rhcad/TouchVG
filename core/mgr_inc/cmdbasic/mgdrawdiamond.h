//! \file mgdrawdiamond.h
//! \brief 定义菱形绘图命令类 MgCmdDrawDiamond
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_DIAMOND_H_
#define TOUCHVG_CMD_DRAW_DIAMOND_H_

#include "mgdrawellipse.h"

//! 菱形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgDiamond
*/
class MgCmdDrawDiamond : public MgCmdDrawEllipse
{
public:
    static const char* Name() { return "diamond"; }
    static MgCommand* Create() { return new MgCmdDrawDiamond; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

#endif // TOUCHVG_CMD_DRAW_DIAMOND_H_
