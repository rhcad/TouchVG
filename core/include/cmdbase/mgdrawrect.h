//! \file mgdrawrect.h
//! \brief 定义矩形绘图命令类 MgCmdDrawRect
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_RECT_H_
#define TOUCHVG_CMD_DRAW_RECT_H_

#include "mgcmddraw.h"

//! 矩形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgRect
*/
class MgCmdDrawRect : public MgCommandDraw
{
public:
    MgCmdDrawRect(const char* name = Name()) : MgCommandDraw(name) {}
#ifndef SWIG
    static const char* Name() { return "rect"; }
    static MgCommand* Create() { return new MgCmdDrawRect; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    
protected:
    virtual void addRectShape(const MgMotion* sender);
    
protected:
    Point2d     m_startPt;
};

#endif // TOUCHVG_CMD_DRAW_RECT_H_
