//! \file mglocal.h
//! \brief 定义本地化文字的辅助类 MgLocalized
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_LOCALSTR_H
#define TOUCHVG_CORE_LOCALSTR_H

#include <string>

struct MgView;

//! 本地化文字的辅助类
struct MgLocalized {
    static std::string getString(MgView* view, const char* name);
    static int formatString(char *buffer, int size, MgView* view, const char *format, ...);
};

#endif // TOUCHVG_CORE_LOCALSTR_H
