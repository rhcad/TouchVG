//! \file mgshapeslock.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPES_LOCK_H_
#define TOUCHVG_MGSHAPES_LOCK_H_

class MgView;
struct MgLockData;

void giSleep(int ms);

//! 图形列表锁定辅助类
/*! \ingroup CORE_COMMAND
*/
class MgShapesLock
{
    MgLockData* locker;
    int         mode;
public:
    enum { NotNotify = 0x100, ReadOnly = 0,
        Add = 0x1, Remove = 0x2, Edit = 0x4, Load = 0x8 };
    
    MgShapesLock(int flags, MgView* view, int timeout = 200);
    MgShapesLock(MgView* view, int timeout = 200);
    ~MgShapesLock();

    bool locked() const;
    void unlock();
    static bool lockedForRead(MgView* view);
    static bool lockedForWrite(MgView* view);

    int getEditFlags() const;
    void resetEditFlags();
};

//! 动态图形锁定辅助类
/*! \ingroup CORE_COMMAND
*/
class MgDynShapeLock
{
    MgLockData* locker;
    int         mode;
public:
    MgDynShapeLock(bool forWrite, MgView* view, int timeout = 200);
    ~MgDynShapeLock();

    bool locked() const;
    static bool lockedForRead(MgView* view);
    static bool lockedForWrite(MgView* view);
};

#endif // TOUCHVG_MGSHAPES_LOCK_H_
