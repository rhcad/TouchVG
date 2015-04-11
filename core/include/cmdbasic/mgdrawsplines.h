//! \file mgdrawsplines.h
//! \brief 定义样条曲线绘图命令类 MgCmdDrawSplines
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_SPLINES_H_
#define TOUCHVG_CMD_DRAW_SPLINES_H_

#include "mgcmddraw.h"

//! 样条曲线绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgSplines
*/
class MgCmdDrawSplines : public MgCommandDraw
{
protected:
    MgCmdDrawSplines(const char* name = Name(), bool freehand = true);
    virtual ~MgCmdDrawSplines() {}
    
public:
    static const char* Name() { return "splines"; }
    static MgCommand* Create() { return new MgCmdDrawSplines; }
    
private:
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
    virtual bool cancel(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);
    
private:
    bool canAddPoint(const MgMotion* sender, bool ended);
    
    bool    m_freehand;
};

//! 用点击绘制样条曲线的命令类
/*! \ingroup CORE_COMMAND
    \see MgSplines
*/
class MgCmdDrawSplineMouse : public MgCmdDrawSplines
{
public:
    static const char* Name() { return "spline_mouse"; }
    static MgCommand* Create() { return new MgCmdDrawSplineMouse; }
private:
    MgCmdDrawSplineMouse() : MgCmdDrawSplines(Name(), false) {}
};

#endif // TOUCHVG_CMD_DRAW_SPLINES_H_
