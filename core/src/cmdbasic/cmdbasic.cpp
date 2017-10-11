// cmdbasic.cpp: 实现基本绘图命令工厂类 MgBasicCommands
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "cmdbasic.h"
#include "mgcmderase.h"
#include "mgdrawline.h"
#include "mgdrawfreelines.h"
#include "mgdrawsplines.h"
#include "mgdrawtriang.h"
#include "mgdrawarc.h"
#include "mgdrawdiamond.h"
#include "mgdrawcircle.h"
#include "mgdrawsquare.h"
#include "mgdrawquadrangle.h"
#include "mgdrawparallel.h"
#include "mgdrawgrid.h"

void MgBasicCommands::registerCmds(MgView* view)
{
    mgRegisterCommand<MgCmdErase>(view);
    mgRegisterCommand<MgCmdDrawRect>(view);
    mgRegisterCommand<MgCmdDrawSquare>(view);
    mgRegisterCommand<MgCmdDrawEllipse>(view);
    mgRegisterCommand<MgCmdDrawCircle>(view);
    mgRegisterCommand<MgCmdDrawCircle3P>(view);
    mgRegisterCommand<MgCmdDrawDiamond>(view);
    mgRegisterCommand<MgCmdDrawLine>(view);
    mgRegisterCommand<MgCmdDrawRayLine>(view);
    mgRegisterCommand<MgCmdDrawBeeLine>(view);
    mgRegisterCommand<MgCmdDrawDot>(view);
    mgRegisterCommand<MgCmdDrawPolygon>(view);
    mgRegisterCommand<MgCmdDrawQuadrangle>(view);
    mgRegisterCommand<MgCmdDrawLines>(view);
    mgRegisterCommand<MgCmdDrawFreeLines>(view);
    mgRegisterCommand<MgCmdDrawSplines>(view);
    mgRegisterCommand<MgCmdDrawSplineMouse>(view);
    mgRegisterCommand<MgCmdDrawTriangle>(view);
    mgRegisterCommand<MgCmdParallel>(view);
    mgRegisterCommand<MgCmdDrawGrid>(view);
    mgRegisterCommand<MgCmdArc3P>(view);
    mgRegisterCommand<MgCmdArcCSE>(view);
    mgRegisterCommand<MgCmdArcTan>(view);
    mgRegisterCommand<MgCmdSector>(view);
    mgRegisterCommand<MgCmdCompass>(view);
}
