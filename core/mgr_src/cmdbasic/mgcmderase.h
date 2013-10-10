//! \file mgcmderase.h
//! \brief 定义橡皮擦命令类 MgCmdErase
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_ERASE_H_
#define TOUCHVG_CMD_ERASE_H_

#include <mgcmd.h>
#include <vector>

//! 橡皮擦命令类
/*! \ingroup CORE_COMMAND
*/
class MgCmdErase : public MgCommand
{
protected:
    MgCmdErase();
    virtual ~MgCmdErase();

public:
    static const char* Name() { return "erase"; }
    static MgCommand* Create() { return new MgCmdErase; }
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual int gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);

private:
    int getStep() { return 0; }
    MgShape* hitTest(const MgMotion* sender);
    bool isIntersectMode(const MgMotion* sender);
    
    std::vector<int>     m_delIds;
    bool                    m_boxsel;
};

#endif // TOUCHVG_CMD_ERASE_H_
