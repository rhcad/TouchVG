// mgdrawcircle.cpp: 实现圆绘图命令类 MgCmdDrawCircle
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawcircle.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdDrawCircle::initialize(const MgMotion* sender, MgStorage*)
{
    bool ret = _initialize(MgShapeT<MgEllipse>::create, sender);
    
    MgBaseRect* rect = (MgBaseRect*)dynshape()->shape();
    rect->setSquare(true);
    
    return ret;
}
