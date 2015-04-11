//! \file mgstrcallback.h
//! \brief 定义获取字符串的回调接口 MgStringCallback
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_STR_CALLBACK_H
#define TOUCHVG_CORE_STR_CALLBACK_H

//! 获取字符串的回调接口，可避免JNI串泄露
/*! \ingroup CORE_VIEW
    \interface MgStringCallback
 */
struct MgStringCallback {
    virtual ~MgStringCallback() {}
    virtual void onGetString(const char* text) = 0;
};

#endif // TOUCHVG_CORE_STR_CALLBACK_H
