//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_H_
#define TOUCHVG_CMD_DRAW_H_

#include "mgcmd.h"

//! 绘图命令基类
/*! \ingroup CORE_COMMAND
 */
class MgCommandDraw : public MgCommand
{
public:
    MgCommandDraw();
    virtual ~MgCommandDraw();
    
    MgShape* addShape(const MgMotion* sender, MgShape* shape = NULL, bool autolock = true);
    void delayClear();
    bool touchBeganStep(const MgMotion* sender);
    bool touchMovedStep(const MgMotion* sender);
    bool touchEndedStep(const MgMotion* sender);

protected:
    virtual MgShape* createShape(MgShapeFactory*) { return (MgShape*)0; }
    bool _initialize(MgShape* (*creator)(), const MgMotion* sender);
    bool _click(const MgMotion* sender);
    virtual int getMaxStep() { return 3; }
    virtual void setStepPoint(int step, const Point2d& pt);

private:
    virtual bool isDrawingCommand() { return true; }
    
public:
    virtual bool initialize(const MgMotion* sender, MgStorage*) { return _initialize(NULL, sender); }
    virtual bool backStep(const MgMotion* sender);
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual int gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);

    int getStep() { return m_needClear ? 0 : m_step; }
    MgShape* dynshape() { return m_shape; }
    Point2d snapPoint(const MgMotion* sender, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep = false);
    
protected:
    int         m_step;
private:
    MgShape*    m_shape;
    bool        m_needClear;
};

#endif // TOUCHVG_CMD_DRAW_H_
