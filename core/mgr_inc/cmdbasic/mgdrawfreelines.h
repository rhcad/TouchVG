//! \file mgdrawfreelines.h
//! \brief 定义自由折线绘图命令类 MgCmdDrawFreeLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_FREELINES_H_
#define TOUCHVG_CMD_DRAW_FREELINES_H_

#include "mgcmddraw.h"

//! 自由折线绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgLines
*/
class MgCmdDrawFreeLines : public MgCommandDraw
{
protected:
    MgCmdDrawFreeLines();
    virtual ~MgCmdDrawFreeLines();
    
public:
    static const char* Name() { return "freelines"; }
    static MgCommand* Create() { return new MgCmdDrawFreeLines; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    
private:
    bool canAddPoint(const MgMotion* sender, bool ended);
};

#endif // TOUCHVG_CMD_DRAW_FREELINES_H_
