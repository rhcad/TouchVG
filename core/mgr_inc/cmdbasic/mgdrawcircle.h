//! \file mgdrawcircle.h
//! \brief 定义圆绘图命令类 MgCmdDrawCircle
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_CIRCLE_H_
#define TOUCHVG_CMD_DRAW_CIRCLE_H_

#include "mgdrawellipse.h"

//! 圆绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgEllipse
*/
class MgCmdDrawCircle : public MgCmdDrawEllipse
{
public:
    static const char* Name() { return "circle"; }
    static MgCommand* Create() { return new MgCmdDrawCircle; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

#endif // TOUCHVG_CMD_DRAW_CIRCLE_H_
