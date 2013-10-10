//! \file mgdrawline.h
//! \brief 定义直线段绘图命令
//! \author pengjun, 2012.6.4
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_LINE_H_
#define TOUCHVG_CMD_DRAW_LINE_H_

#include "mgcmddraw.h"

//! 直线段绘图命令
/*! \ingroup CORE_COMMAND
    \see MgLine
*/
class MgCmdDrawLine : public MgCommandDraw
{
protected:
    MgCmdDrawLine();
    virtual ~MgCmdDrawLine();
    
public:
    static const char* Name() { return "line"; }
    static MgCommand* Create() { return new MgCmdDrawLine; }

private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

//! 固定长度直线段绘图命令
/*! \ingroup CORE_COMMAND
    \see MgEllipse
*/
class MgCmdDrawFixedLine : public MgCmdDrawLine
{
public:
    static const char* Name() { return "fixedline"; }
    static MgCommand* Create() { return new MgCmdDrawFixedLine; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

#endif // TOUCHVG_CMD_DRAW_LINE_H_
