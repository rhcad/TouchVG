//! \file mgdrawtriang.h
//! \brief 定义三角形绘图命令 MgCmdDrawTriangle
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_TRIANGLE_H_
#define TOUCHVG_CMD_DRAW_TRIANGLE_H_

#include "mgcmddraw.h"

//! 三角形绘图命令
/*! \ingroup CORE_COMMAND
*/
class MgCmdDrawTriangle : public MgCommandDraw
{
public:
    static const char* Name() { return "triangle"; }
    static MgCommand* Create() { return new MgCmdDrawTriangle; }

private:
    MgCmdDrawTriangle(const char* name = Name()) : MgCommandDraw(name) {}
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

#endif // TOUCHVG_CMD_DRAW_TRIANGLE_H_
