//! \file mgdrawgrid.h
//! \brief 定义网格绘图命令类 MgCmdDrawGrid
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_GRID_H_
#define TOUCHVG_CMD_DRAW_GRID_H_

#include "mgdrawrect.h"

//! 网格绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgGrid
*/
class MgCmdDrawGrid : public MgCmdDrawRect
{
public:
    static const char* Name() { return "grid"; }
    static MgCommand* Create() { return new MgCmdDrawGrid; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    
    virtual void addRectShape(const MgMotion* sender);
};

#endif // TOUCHVG_CMD_DRAW_GRID_H_
