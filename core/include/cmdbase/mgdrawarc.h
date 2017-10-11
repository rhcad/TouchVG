//! \file mgdrawarc.h
//! \brief 定义圆弧绘图命令
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_ARC_H_
#define TOUCHVG_CMD_DRAW_ARC_H_

#include "mgcmddraw.h"

//! 三点圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
 */
class MgCmdArc3P : public MgCommandDraw
{
public:
    MgCmdArc3P(const char* name = Name()) : MgCommandDraw(name) {}
#ifndef SWIG
    static const char* Name() { return "arc3p"; }
    static MgCommand* Create() { return new MgCmdArc3P; }
#endif
    virtual void release() { delete this; }
    virtual bool touchBegan(const MgMotion* sender) { return touchBeganStep(sender); }
    virtual bool touchMoved(const MgMotion* sender) { return touchMovedStep(sender); }
    virtual bool touchEnded(const MgMotion* sender) { return touchEndedStep(sender); }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);

protected:
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
    virtual void drawArcHandle(const MgMotion* sender, GiGraphics* gs);
    
protected:
    Point2d _points[3];
};

//! 圆心+起点+终点圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
 */
class MgCmdArcCSE : public MgCmdArc3P
{
public:
    MgCmdArcCSE(const char* name = Name()) : MgCmdArc3P(name) {}
#ifndef SWIG
    static const char* Name() { return "arc_cse"; }
    static MgCommand* Create() { return new MgCmdArcCSE; }
#endif
    virtual void release() { delete this; }
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);

protected:
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
};

//! 扇形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
 */
class MgCmdSector : public MgCmdArcCSE
{
public:
    MgCmdSector(const char* name = Name()) : MgCmdArcCSE(name) {}
#ifndef SWIG
    static const char* Name() { return "sector"; }
    static MgCommand* Create() { return new MgCmdSector; }
#endif
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
};

//! 圆规圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
 */
class MgCmdCompass : public MgCmdArc3P
{
public:
    MgCmdCompass(const char* name = Name()) : MgCmdArc3P(name), _decimal(0) {}
#ifndef SWIG
    static const char* Name() { return "compass"; }
    static MgCommand* Create() { return new MgCmdCompass; }
#endif
    virtual void release() { delete this; }
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    
protected:
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
    
private:
    int     _decimal;
    float   _radius;
};

//! 切线圆弧绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgArc
 */
class MgCmdArcTan : public MgCmdArc3P
{
public:
    MgCmdArcTan(const char* name = Name()) : MgCmdArc3P(name) {}
#ifndef SWIG
    static const char* Name() { return "arc_tan"; }
    static MgCommand* Create() { return new MgCmdArcTan; }
#endif
    virtual void release() { delete this; }
protected:
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
};

#endif // TOUCHVG_CMD_DRAW_ARC_H_
