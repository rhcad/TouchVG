// HitTestCmd.h: 定义点中测试命令类
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_TEST_HITTESTCMD_H
#define TOUCHVG_TEST_HITTESTCMD_H

#include "mgcmd.h"
#include <list>

class HitTestCmd : public MgCommand
{
public:
    static const char* Name() { return "hittest"; }
    static MgCommand* Create() { return new HitTestCmd; }
private:
    HitTestCmd() : MgCommand(Name()), _tol(0), _curid(0) { _cur.dist = 1e10f; }
    virtual void release() { delete this; }
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool mouseHover(const MgMotion* sender) { return touchMoved(sender); }

private:
    struct Item {
        float   dist;
        Point2d nearpt;
        Point2d pt;
    };
    float   _tol;
    Item    _cur;
    std::list<Item> _items;
    int     _curid;
    MgHitResult _hit;
};

#endif // TOUCHVG_TEST_HITTESTCMD_H
