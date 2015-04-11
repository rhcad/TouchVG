//! \file mgdrawdiamond.h
//! \brief 定义菱形绘图命令类 MgCmdDrawDiamond
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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
    MgCmdDrawDiamond() : MgCmdDrawEllipse(Name()) {}
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

#endif // TOUCHVG_CMD_DRAW_DIAMOND_H_
