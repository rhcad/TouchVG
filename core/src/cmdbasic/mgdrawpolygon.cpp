// mgdrawpolygon.cpp: 实现多边形绘图命令类 MgCmdDrawPolygon
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawpolygon.h"
#include "mgbasicsps.h"

bool MgCmdDrawPolygon::initialize(const MgMotion* sender, MgStorage* s)
{
    _initialize(MgLines::Type(), sender);
    
    ((MgBaseLines*)dynshape()->shape())->setClosed(true);
    
    return _initialize(0, sender, s);
}
