//! \file mgcmddraw.h
//! \brief 定义绘图命令基类 MgCommandDraw 和 MgCmdBaseLines
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_DRAW_H_
#define TOUCHVG_CMD_DRAW_H_

#include "mgcmd.h"

//! 绘图命令基类
/*! \ingroup CORE_COMMAND
 */
class MgCommandDraw : public MgCommand
{
public:
    MgCommandDraw(const char* name);
    virtual ~MgCommandDraw();
    
    MgShape* addShape(const MgMotion* sender, MgShape* shape = MgShape::Null());
    
    bool touchBeganStep(const MgMotion* sender);
    bool touchMovedStep(const MgMotion* sender);
    bool touchEndedStep(const MgMotion* sender);

    //! 返回新图形的类型，供其他语言重载使用
    virtual int getShapeType() { return m_shape ? m_shape->shapec()->getType() : 0; }
    
    //! 为当前捕捉设置上一线段的坐标，以避免与上一点重合
    void ignoreStartPoint(const MgMotion* sender, int handle);
    
    //! 返回当前捕捉结果类型, >=kMgSnapGrid
    int getSnappedType(const MgMotion* sender) const;
    
    //! 返回上次捕捉到的位置
    static Point2d getLastSnappedPoint() { return m_lastSnapped[0]; }
    
    //! 返回上次捕捉到对象坐标时的原始位置
    static Point2d getLastSnappedOriginPoint() { return m_lastSnapped[1]; }
    
protected:
    bool _initialize(int shapeType, const MgMotion* sender, MgStorage* s = (MgStorage*)0);
    bool _click(const MgMotion* sender);
    virtual int getMaxStep() { return 3; }
    virtual void setStepPoint(const MgMotion* sender, int step, const Point2d& pt);
    virtual bool isStepPointAccepted(const MgMotion* sender, const Point2d& pt);
    virtual int snapOptionsForStep(const MgMotion* sender, int step) { return -1; }

private:
    virtual bool isDrawingCommand() { return true; }
    
public:
    virtual bool initialize(const MgMotion* sender, MgStorage* s) {
        return _initialize(getShapeType(), sender, s); }
    virtual bool backStep(const MgMotion* sender);
    virtual bool cancel(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender);
#ifndef SWIG
    virtual const MgShape* getShape(const MgMotion* sender) { return m_shape; }
#endif
    int getStep() { return m_step; }
    MgShape* dynshape() { return m_shape; }
    void setStep(int step) { m_step = step; }
    Point2d snapPointWidhOptions(const MgMotion* sender, int options, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep = false);
    Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, bool firstStep, int handle);
    
protected:
    int         m_step;
private:
    MgShape*    m_shape;
    int         m_flags;
    static Point2d m_lastSnapped[2];
};

#endif // TOUCHVG_CMD_DRAW_H_
