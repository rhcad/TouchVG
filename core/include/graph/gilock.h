//! \file gilock.h
//! \brief 定义原子锁函数 giAtomicIncrement, giAtomicDecrement, giAtomicCompareAndSwap
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_GILOCK_H_
#define TOUCHVG_GILOCK_H_

#ifndef SWIG
#if defined(_MACOSX) || defined(__APPLE__) || defined(__DARWIN__)
    #include <libkern/OSAtomic.h>
    inline long giAtomicIncrement(volatile long *p) { return OSAtomicIncrement32((volatile int32_t *)p); }
    inline long giAtomicDecrement(volatile long *p) { return OSAtomicDecrement32((volatile int32_t *)p); }
    inline bool giAtomicCompareAndSwap(volatile long *p, long value, long oldValue) {
        return OSAtomicCompareAndSwapLong(oldValue, value, p); }
#elif defined(__WINDOWS__) || defined(WIN32)
    #ifndef _WINDOWS_
        #define WIN32_LEAN_AND_MEAN
        #include <windows.h>
    #endif
    #if defined(_MSC_VER) && _MSC_VER <= 1200
        inline long giAtomicIncrement(volatile long *p) { return InterlockedIncrement((long*)p); }
        inline long giAtomicDecrement(volatile long *p) { return InterlockedDecrement((long*)p); }
        inline bool giAtomicCompareAndSwap(volatile long *p, long value, long oldValue) {
            return InterlockedCompareExchange((long*)p, value, oldValue) == oldValue; }
    #else
        inline long giAtomicIncrement(volatile long *p) { return InterlockedIncrement(p); }
        inline long giAtomicDecrement(volatile long *p) { return InterlockedDecrement(p); }
        inline bool giAtomicCompareAndSwap(volatile long *p, long value, long oldValue) {
            return InterlockedCompareExchange(p, value, oldValue) == oldValue; }
    #endif
#elif defined(__ANDROID__) || defined(__linux__)
    inline long giAtomicIncrement(volatile long *p) { return __sync_add_and_fetch(p, 1L); }
    inline long giAtomicDecrement(volatile long *p) { return __sync_sub_and_fetch(p, 1L); }
    inline bool giAtomicCompareAndSwap(volatile long *p, long value, long oldValue) {
        return __sync_bool_compare_and_swap(p, oldValue, value); }
#else
    inline long giAtomicIncrement(volatile long *p) { return ++(*p); }
    inline long giAtomicDecrement(volatile long *p) { return --(*p); }
    inline bool giAtomicCompareAndSwap(volatile long *p, long value, long oldValue) {
        bool b = *p == oldValue; if (b) *p = value; return oldValue; }
#endif
#endif // SWIG

#endif // TOUCHVG_GILOCK_H_
