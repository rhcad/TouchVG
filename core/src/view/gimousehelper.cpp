// gimousehelper.cpp
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#include "gimousehelper.h"
#include <math.h>

GiMouseHelper::GiMouseHelper(GiView* view, GiCoreView* coreView)
    : _view(view), _coreView(coreView), _startX(0), _startY(0)
    , _moved(false), _ldown(false), _rdown(false)
{
}

bool GiMouseHelper::onLButtonDown(float x, float y, bool, bool)
{
    _startX = x;
    _startY = y;
    _moved = false;
    _ldown = true;
    _rdown = false;
    return _coreView->onGesture(_view, kGiGesturePan, kGiGesturePossible, x, y);
}

bool GiMouseHelper::onMouseUp(float x, float y)
{
    bool ret = false;

    if (_ldown) {
        ret = _coreView->onGesture(_view, 
            _moved ? kGiGesturePan : kGiGestureTap, 
            kGiGestureEnded, x, y);
    }
    else if (_rdown) {
        ret = _coreView->onGesture(_view, kGiGesturePress,
            kGiGestureBegan, x, y);
    }

    return ret;
}

#ifndef _MSC_VER
static inline float _hypotf(float x, float y) { return hypotf(x, y); }
#endif

bool GiMouseHelper::onMouseMove(float x, float y, bool ldown, bool)
{
    bool ret = false;

    if (ldown && _ldown) {
        if (!_moved && _hypotf(x - _startX, y - _startY) > 5.f) {
            _moved = _coreView->onGesture(_view, kGiGesturePan, 
                kGiGestureBegan, _startX, _startY);
        }
        if (_moved) {
            ret = _coreView->onGesture(_view, kGiGesturePan, 
                kGiGestureMoved, x, y);
        }
    }

    return ret;
}

bool GiMouseHelper::onLButtonDblClk(float x, float y)
{
    return _ldown && _coreView->onGesture(_view, kGiGestureDblTap, kGiGestureEnded, x, y);
}

bool GiMouseHelper::onRButtonDown(float x, float y)
{
    _startX = x;
    _startY = y;
    _moved = false;
    _rdown = true;
    _ldown = false;
    return false;
}

bool GiMouseHelper::onKeyDown(int)
{
    return false;
}
