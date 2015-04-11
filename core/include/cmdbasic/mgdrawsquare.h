//! \file mgdrawsquare.h
//! \brief 定义正方形绘图命令类 MgCmdDrawSquare
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_SQUARE_H_
#define TOUCHVG_CMD_DRAW_SQUARE_H_

#include "mgdrawrect.h"

//! 正方形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgRect
*/
class MgCmdDrawSquare : public MgCmdDrawRect
{
public:
    static const char* Name() { return "square"; }
    static MgCommand* Create() { return new MgCmdDrawSquare; }
    
private:
    MgCmdDrawSquare(const char* name = Name()) : MgCmdDrawRect(name) {}
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

#endif // TOUCHVG_CMD_DRAW_SQUARE_H_
