// mgdrawpolygon.cpp: 实现多边形绘图命令类 MgCmdDrawPolygon
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawpolygon.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdDrawPolygon::initialize(const MgMotion* sender, MgStorage*)
{
    bool ret = _initialize(MgShapeT<MgLines>::create, sender);
    if (ret) {
        ((MgBaseLines*)dynshape()->shape())->setClosed(true);
    }
    return ret;
}
