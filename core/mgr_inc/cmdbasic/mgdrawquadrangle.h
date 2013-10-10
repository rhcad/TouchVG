//! \file mgdrawquadrangle.h
//! \brief 定义四边形绘图命令类 MgCmdDrawQuadrangle
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_QUADRANGLE_H_
#define TOUCHVG_CMD_DRAW_QUADRANGLE_H_

#include "mgdrawpolygon.h"

//! 四边形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgLines
*/
class MgCmdDrawQuadrangle : public MgCmdDrawPolygon
{
public:
    static const char* Name() { return "quadrangle"; }
    static MgCommand* Create() { return new MgCmdDrawQuadrangle; }
    
private:
    MgCmdDrawQuadrangle() : MgCmdDrawPolygon(4) {}
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
};

#endif // TOUCHVG_CMD_DRAW_QUADRANGLE_H_
