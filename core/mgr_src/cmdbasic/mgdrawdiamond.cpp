// mgdrawdiamond.cpp: 实现菱形绘图命令类 MgCmdDrawDiamond
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawdiamond.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdDrawDiamond::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgDiamond>::create, sender);
}
