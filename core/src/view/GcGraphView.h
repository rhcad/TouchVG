//! \file GcGraphView.h
//! \brief 定义主绘图视图类 GcGraphView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_MAINVIEW_H
#define TOUCHVG_CORE_MAINVIEW_H

#include "GcBaseView.h"

//! 主绘图视图类
/*! \ingroup CORE_VIEW
 */
class GcGraphView : public GcBaseView
{
public:
    GcGraphView(MgView* mgview, GiView *view);
    virtual ~GcGraphView();
    
    virtual void draw(GiGraphics& gs);
};

#endif // TOUCHVG_CORE_MAINVIEW_H
