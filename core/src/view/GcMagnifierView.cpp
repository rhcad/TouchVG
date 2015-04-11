// GcMagnifierView.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "GcMagnifierView.h"

GcMagnifierView::GcMagnifierView(MgView* mgview, GiView *view, GcGraphView* mainView)
    : GcBaseView(mgview, view), _mainView(mainView)
{
}

GcMagnifierView::~GcMagnifierView()
{
}
