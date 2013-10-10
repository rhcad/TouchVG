//! \file mgdrawpolygon.h
//! \brief 定义多边形绘图命令类 MgCmdDrawPolygon
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_POLYGON_H_
#define TOUCHVG_CMD_DRAW_POLYGON_H_

#include "mgdrawlines.h"

//! 多边形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgLines
*/
class MgCmdDrawPolygon : public MgCmdDrawLines
{
protected:
    MgCmdDrawPolygon(int maxEdges = 20) : _maxEdges(maxEdges) {}
    
public:
    static const char* Name() { return "polygon"; }
    static MgCommand* Create() { return new MgCmdDrawPolygon; }
    
private:
    virtual bool needCheckClosed() { return false; }
    virtual bool needEnded() { return (int)m_step >= _maxEdges - 1; }
    
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    
    int     _maxEdges;
};

#endif // TOUCHVG_CMD_DRAW_POLYGON_H_
