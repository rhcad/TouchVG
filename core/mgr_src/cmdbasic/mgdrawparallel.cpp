// mgdrawparallel.cpp: 实现平行四边形绘图命令类 MgCmdParallel
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgdrawparallel.h"
#include <mgshapet.h>
#include <mgbasicsp.h>

bool MgCmdParallel::initialize(const MgMotion* sender, MgStorage*)
{
    return _initialize(MgShapeT<MgParallel>::create, sender);
}
