//! \file GcMagnifierView.h
//! \brief 定义放大镜视图类 GcMagnifierView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_MAGNIFIERVIEW_H
#define TOUCHVG_CORE_MAGNIFIERVIEW_H

#include "GcGraphView.h"

//! 放大镜视图类
/*! \ingroup CORE_VIEW
 */
class GcMagnifierView : public GcBaseView
{
public:
    GcMagnifierView(MgView* mgview, GiView *view, GcGraphView* mainView);
    virtual ~GcMagnifierView();
    
    GcGraphView* getMainView() const { return _mainView; }
    
private:
    GcGraphView*     _mainView;
};

#endif // TOUCHVG_CORE_MAGNIFIERVIEW_H
