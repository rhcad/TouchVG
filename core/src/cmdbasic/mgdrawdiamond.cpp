// mgdrawdiamond.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdrawdiamond.h"
#include "mgbasicsps.h"

bool MgCmdDrawDiamond::initialize(const MgMotion* sender, MgStorage* s)
{
    return _initialize(MgDiamond::Type(), sender, s);
}
