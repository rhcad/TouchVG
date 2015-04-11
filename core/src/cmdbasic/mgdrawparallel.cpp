// mgdrawparallel.cpp: 实现平行四边形绘图命令类 MgCmdParallel
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawparallel.h"
#include "mgbasicsps.h"

bool MgCmdParallel::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgParallel::Type(), sender, s);
}
