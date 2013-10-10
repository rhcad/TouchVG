//! \file GcMagnifierView.cpp
//! \brief 实现放大镜视图类 GcMagnifierView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#include "GcMagnifierView.h"

GcMagnifierView::GcMagnifierView(MgView* mgview, GiView *view, GcGraphView* mainView)
    : GcBaseView(mgview, view), _mainView(mainView)
{
}

GcMagnifierView::~GcMagnifierView()
{
}
