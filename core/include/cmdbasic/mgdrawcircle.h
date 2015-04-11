//! \file mgdrawcircle.h
//! \brief 定义圆绘图命令类 MgCmdDrawCircle
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_CIRCLE_H_
#define TOUCHVG_CMD_DRAW_CIRCLE_H_

#include "mgdrawellipse.h"

//! 圆绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgEllipse
*/
class MgCmdDrawCircle : public MgCmdDrawEllipse
{
public:
    MgCmdDrawCircle(const char* name = Name()) : MgCmdDrawEllipse(name) {}
#ifndef SWIG
    static const char* Name() { return "circle2p"; }
    static MgCommand* Create() { return new MgCmdDrawCircle(); }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

//! 三点画圆命令类
/*! \ingroup CORE_COMMAND
    \see MgEllipse
 */
class MgCmdDrawCircle3P : public MgCommandDraw
{
public:
    MgCmdDrawCircle3P(const char* name = Name()) : MgCommandDraw(name) {}
#ifndef SWIG
    static const char* Name() { return "circle3p"; }
    static MgCommand* Create() { return new MgCmdDrawCircle3P; }
#endif
    virtual void release() { delete this; }
    virtual bool touchBegan(const MgMotion* sender) { return touchBeganStep(sender); }
    virtual bool touchMoved(const MgMotion* sender) { return touchMovedStep(sender); }
    virtual bool touchEnded(const MgMotion* sender) { return touchEndedStep(sender); }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    
protected:
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
    virtual void drawHandles(const MgMotion* sender, GiGraphics* gs);
    
protected:
    Point2d _points[3];
};

#endif // TOUCHVG_CMD_DRAW_CIRCLE_H_
