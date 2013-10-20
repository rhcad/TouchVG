#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "ViewAdapter.h"
#include <gimousehelper.h>

ViewAdapter::ViewAdapter() : _hwnd(NULL)
{
    _coreView = new GiCoreView();
    _coreView->createView(this);
    _helper = new GiMouseHelper(this, _coreView);
}

ViewAdapter::~ViewAdapter()
{
    delete _helper;
    _coreView->destoryView(this);
    delete _coreView;
}

void ViewAdapter::regenAll()
{
    _canvas.clearCachedBitmap();
    InvalidateRect(_hwnd, NULL, FALSE);
}

void ViewAdapter::regenAppend()
{
    regenAll();
}

void ViewAdapter::redraw()
{
    InvalidateRect(_hwnd, NULL, FALSE);
}

void ViewAdapter::onSize(int w, int h, int dpi)
{
    _coreView->setScreenDpi(dpi);
    _coreView->onSize(this, w, h);
}

void ViewAdapter::onDraw(HDC hdc)
{
    onSize(0, 0, GetDeviceCaps(hdc,LOGPIXELSY));

    if (_canvas.beginPaint(_hwnd, hdc)) {       // 开始在画布上绘制
        if (!_canvas.drawCachedBitmap()) {      // 显示上次保存的内容
            _canvas.clearWindow();              // 使用背景色清除显示
            _coreView->drawAll(this, &_canvas); // 显示所有图形
            _canvas.saveCachedBitmap();         // 缓存显示的内容
        }
        _coreView->dynDraw(this, &_canvas);     // 绘制动态图形
        _canvas.endPaint();                     // 结束绘制
    }
}

bool ViewAdapter::onLButtonDown(int x, int y, WPARAM wparam)
{
    return _helper->onLButtonDown((float)x, (float)y,
        (wparam & MK_CONTROL) != 0, (wparam & MK_SHIFT) != 0);
}

bool ViewAdapter::onLButtonUp(int x, int y)
{
    return _helper->onMouseUp((float)x, (float)y);
}

bool ViewAdapter::onMouseMove(int x, int y, WPARAM wparam)
{
    return _helper->onMouseMove((float)x, (float)y, 
        (wparam & MK_LBUTTON) != 0, (wparam & MK_RBUTTON) != 0);
}

bool ViewAdapter::onRButtonDown(int x, int y)
{
    return _helper->onRButtonDown((float)x, (float)y);
}

bool ViewAdapter::onRButtonUp(int x, int y)
{
    return _helper->onMouseUp((float)x, (float)y);
}

bool ViewAdapter::setCommand(const char* name)
{
    return _coreView->setCommand(this, name);
}
