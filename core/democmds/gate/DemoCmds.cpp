// DemoCmds.cpp
#include "DemoCmds.h"
#include "cmds.h"

static inline MgView* toView(long mgView)
{
    MgView* ret;
    *(long*)&ret = mgView;
    return ret;
}

int DemoCmdsGate::registerCmds(long mgView)
{
    return DemoCmdsImpl::registerCmds(toView(mgView));
}

int DemoCmdsGate::getDimensions(long mgView, 
                                mgvector<float>& vars, mgvector<char>& types)
{
    int n = vars.count() < types.count() ? vars.count() : types.count();
    return DemoCmdsImpl::getDimensions(toView(mgView), 
        vars.address(), types.address(), n);
}
