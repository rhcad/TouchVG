//! \file gimousehelper.h
//! \brief 定义鼠标转手势的辅助类 GiMouseHelper
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_MOUSEHELPER_H
#define TOUCHVG_CORE_MOUSEHELPER_H

#include "gicoreview.h"

//! 鼠标转手势的辅助类
/*! \ingroup CORE_VIEW
 */
class GiMouseHelper
{
public:
    GiMouseHelper(GiView* view, GiCoreView* coreView);

    bool onLButtonDown(float x, float y, bool ctrl, bool shift);
    bool onRButtonDown(float x, float y);
    bool onMouseUp(float x, float y);
    bool onMouseMove(float x, float y, bool ldown, bool rdown);
    bool onLButtonDblClk(float x, float y);
    bool onKeyDown(int code);

private:
    GiView*     _view;
    GiCoreView* _coreView;
    float       _startX;
    float       _startY;
    bool        _moved;
    bool        _ldown;
    bool        _rdown;
};

#endif // TOUCHVG_CORE_MOUSEHELPER_H
