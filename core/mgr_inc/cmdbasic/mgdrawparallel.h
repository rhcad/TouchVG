//! \file mgdrawparallel.h
//! \brief 定义平行四边形绘图命令类 MgCmdParallel
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_DRAW_PARALLEL_H_
#define TOUCHVG_CMD_DRAW_PARALLEL_H_

#include "mgcmddraw.h"

//! 平行四边形绘图命令类
/*! \ingroup CORE_COMMAND
    \see MgParallel
*/
class MgCmdParallel : public MgCommandDraw
{
protected:
    MgCmdParallel() {}
public:
    static const char* Name() { return "parallel"; }
    static MgCommand* Create() { return new MgCmdParallel; }
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool touchBegan(const MgMotion* sender) { return touchBeganStep(sender); }
    virtual bool touchMoved(const MgMotion* sender) { return touchMovedStep(sender); }
    virtual bool touchEnded(const MgMotion* sender) { return touchEndedStep(sender); }
};

#endif // TOUCHVG_CMD_DRAW_PARALLEL_H_
