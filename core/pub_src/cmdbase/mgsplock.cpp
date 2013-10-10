// mgsplock.cpp: 实现 MgShapesLock, MgDynShapeLock
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include <mgcmd.h>
#include <mglockdata.h>

// MgShapesLock
//

MgShapesLock::MgShapesLock(int flags, MgView* view, int timeout)
{
    locker = view ? view->getLockData() : NULL;
    mode = locker && locker->lockData(flags, timeout) ? (flags ? 2 : 1) : 0;
    if (mode == 2 && (flags & NotNotify) != 0) {
        mode |= 4;
    }
}

MgShapesLock::MgShapesLock(MgView* view, int timeout)
{
    locker = view ? view->getLockData() : NULL;
    mode = locker && locker->lockData(Edit, timeout) ? 2 : 0;
}

MgShapesLock::~MgShapesLock()
{
    bool ended = false;
    
    if (locked()) {
        ended = (0 == locker->unlockData((mode & 2) != 0));
    }
    if (mode == 2 && ended) {
        locker->afterChanged();
    }
}

bool MgShapesLock::locked() const
{
    return mode != 0;
}

bool MgShapesLock::lockedForRead(MgView* view)
{
    return view->getLockData()->lockedForRead();
}

bool MgShapesLock::lockedForWrite(MgView* view)
{
    return view->getLockData()->lockedForWrite();
}

int MgShapesLock::getEditFlags() const
{
    return locker->getEditFlags();
}

void MgShapesLock::resetEditFlags()
{
    locker->resetEditFlags();
}

// MgDynShapeLock
//

MgDynShapeLock::MgDynShapeLock(bool forWrite, MgView* view, int timeout)
{
    locker = view->getLockData();
    mode = locker->lockDynData(forWrite, timeout) ? (forWrite ? 2 : 1) : 0;
}

MgDynShapeLock::~MgDynShapeLock()
{
    if (locked()) {
        locker->unlockDynData(mode == 2);
    }
}

bool MgDynShapeLock::locked() const
{
    return mode != 0;
}

bool MgDynShapeLock::lockedForRead(MgView* view)
{
    return view->getLockData()->lockedForReadDyn();
}

bool MgDynShapeLock::lockedForWrite(MgView* view)
{
    return view->getLockData()->lockedForWriteDyn();
}
