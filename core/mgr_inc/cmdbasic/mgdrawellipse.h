//! \file mgdrawellipse.h
//! \brief 定义椭圆绘图命令类 MgCmdDrawEllipse
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_ELLIPSE_H_
#define TOUCHVG_CMD_DRAW_ELLIPSE_H_

#include "mgdrawrect.h"

//! 椭圆绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgEllipse
*/
class MgCmdDrawEllipse : public MgCmdDrawRect
{
public:
    static const char* Name() { return "ellipse"; }
    static MgCommand* Create() { return new MgCmdDrawEllipse; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
};

#endif // TOUCHVG_CMD_DRAW_ELLIPSE_H_
