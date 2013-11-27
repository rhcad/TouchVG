//! \file gilock.h
//! \brief 定义原子锁函数 giInterlockedIncrement, giInterlockedDecrement
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_GILOCK_H_
#define TOUCHVG_GILOCK_H_

#ifndef SWIG
#if defined(_MACOSX) || defined(__APPLE__) || defined(__DARWIN__)
    #include <libkern/OSAtomic.h>
    inline long giInterlockedIncrement(volatile long *p) { return OSAtomicIncrement32((volatile int32_t *)p); }
    inline long giInterlockedDecrement(volatile long *p) { return OSAtomicDecrement32((volatile int32_t *)p); }
    typedef struct dispatch_queue_s *dispatch_queue_t;
    extern dispatch_queue_t vg_rwlock_queue;
#elif defined(__WINDOWS__) || defined(WIN32)
    #ifndef _WINDOWS_
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #endif
    #if defined(_MSC_VER) && _MSC_VER <= 1200
        inline long giInterlockedIncrement(volatile long *p) { return InterlockedIncrement((long*)p); }
        inline long giInterlockedDecrement(volatile long *p) { return InterlockedDecrement((long*)p); }
    #else
        inline long giInterlockedIncrement(volatile long *p) { return InterlockedIncrement(p); }
        inline long giInterlockedDecrement(volatile long *p) { return InterlockedDecrement(p); }
    #endif
#elif defined(__ANDROID__) || defined(__linux__)
    inline long giInterlockedIncrement(volatile long *p) { return __sync_add_and_fetch(p, 1L); }
    inline long giInterlockedDecrement(volatile long *p) { return __sync_sub_and_fetch(p, 1L); }
#else
    inline long giInterlockedIncrement(volatile long *p) { return ++(*p); }
    inline long giInterlockedDecrement(volatile long *p) { return --(*p); }
#endif
#endif // SWIG

#endif // TOUCHVG_GILOCK_H_
