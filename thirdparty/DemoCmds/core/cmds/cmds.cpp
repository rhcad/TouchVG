// cmds.cpp
#include "cmds.h"
#include "cmdsubject.h"
#include "mgspfactory.h"
#include "HitTestCmd.h"
#include "mgshapet.h"

class DemoCmdsObserver : public CmdObserverDefault
{
public:
    DemoCmdsObserver() {}
};

static DemoCmdsObserver _observer;

int DemoCmdsImpl::registerCmds(long mgView)
{
    MgView* view = MgView::fromHandle(mgView);
    view->getCmdSubject()->registerObserver(&_observer);

    //MgShapeT<MgCube>::registerCreator(view->getShapeFactory());
    
    int n = 0;
    
    n += mgRegisterCommand<HitTestCmd>(view);
    
    return n;
}

int DemoCmdsImpl::getDimensions(long, float*, char*, int)
{
    return 0;
}
