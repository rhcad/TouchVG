// mglockrw.cpp: 实现 MgLockRW
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgshapedoc.h"

#ifdef _WIN32
#ifndef _WINDOWS_
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
void giSleep(int ms) { Sleep(ms); }
#else
#include <unistd.h>
void giSleep(int ms) { usleep(ms * 1000); }
#endif

#if defined(_MACOSX) || defined(__APPLE__) || defined(__DARWIN__)
#include <dispatch/dispatch.h>
dispatch_queue_t vg_rwlock_queue = dispatch_queue_create("touchvg.rwlock", DISPATCH_QUEUE_CONCURRENT);
#endif

MgLockRW::MgLockRW() : _editFlags(0)
{
    _counts[0] = _counts[1] = _counts[2] = 0;
}

bool MgLockRW::lockData(bool forWrite, int timeout)
{
#if defined(_MACOSX) || defined(__APPLE__) || defined(__DARWIN__)
    __block bool ret = false;
    dispatch_barrier_sync(vg_rwlock_queue, ^{
#else
    bool ret = false;
#endif
    
    if (1 == giInterlockedIncrement(_counts)) {     // first locked
        giInterlockedIncrement(_counts + (forWrite ? 2 : 1));
        ret = true;
    }
    else {
        ret = !forWrite && 0 == _counts[2];         // for read and not locked for write
        for (int i = 0; i < timeout && !ret; i += 25) {
            giSleep(25);
            ret = forWrite ? (!_counts[1] && !_counts[2]) : !_counts[2];
        }
        if (ret) {
            giInterlockedIncrement(_counts + (forWrite ? 2 : 1));
        }
        else {
            giInterlockedDecrement(_counts);
        }
    }
#if defined(_MACOSX) || defined(__APPLE__) || defined(__DARWIN__)
    });
#endif
    
    return ret;
}

long MgLockRW::unlockData(bool forWrite)
{
    giInterlockedDecrement(_counts + (forWrite ? 2 : 1));
    return giInterlockedDecrement(_counts);
}

bool MgLockRW::firstLocked() const
{
    return _counts[0] == 1;
}

bool MgLockRW::lockedForRead() const
{
    return _counts[0] > 0;
}

bool MgLockRW::lockedForWrite() const
{
    return _counts[2] > 0;
}
