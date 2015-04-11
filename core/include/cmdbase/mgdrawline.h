//! \file mgdrawline.h
//! \brief 定义直线段绘图命令
//! \author pengjun, 2012.6.4
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_LINE_H_
#define TOUCHVG_CMD_DRAW_LINE_H_

#include "mgcmddraw.h"

//! 直线段绘图命令
/*! \ingroup CORE_COMMAND
    \see MgLine
*/
class MgCmdDrawLine : public MgCommandDraw
{
public:
    MgCmdDrawLine(const char* name = Name()) : MgCommandDraw(name) {}
#ifndef SWIG
    static const char* Name() { return "line"; }
    static MgCommand* Create() { return new MgCmdDrawLine; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

//! 射线绘图命令
class MgCmdDrawRayLine : public MgCmdDrawLine
{
public:
    MgCmdDrawRayLine(const char* name = Name()) : MgCmdDrawLine(name) {}
#ifndef SWIG
    static const char* Name() { return "rayline"; }
    static MgCommand* Create() { return new MgCmdDrawRayLine; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

//! 无穷直线绘图命令
class MgCmdDrawBeeLine : public MgCmdDrawLine
{
public:
    MgCmdDrawBeeLine(const char* name = Name()) : MgCmdDrawLine(name) {}
#ifndef SWIG
    static const char* Name() { return "beeline"; }
    static MgCommand* Create() { return new MgCmdDrawBeeLine; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

//! 点绘图命令
/*! \ingroup CORE_COMMAND
    \see MgDot
*/
class MgCmdDrawDot : public MgCommandDraw
{
public:
    MgCmdDrawDot(const char* name = Name()) : MgCommandDraw(name) {}
#ifndef SWIG
    static const char* Name() { return "dot"; }
    static MgCommand* Create() { return new MgCmdDrawDot; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool click(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
};

#endif // TOUCHVG_CMD_DRAW_LINE_H_
