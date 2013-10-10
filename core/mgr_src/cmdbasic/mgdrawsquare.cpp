// mgdrawsquare.cpp: 实现正方形绘图命令类 MgCmdDrawSquare
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawsquare.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdDrawSquare::initialize(const MgMotion* sender, MgStorage*)
{
    bool ret = _initialize(MgShapeT<MgRect>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}
