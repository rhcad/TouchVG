#ifndef TEST_VIEW_ADAPTER_H
#define TEST_VIEW_ADAPTER_H

#include <giview.h>
#include <GiGdipCanvas.h>

class GiCoreView;
class GiMouseHelper;
class GiCanvas;

class ViewAdapter : public GiView
{
public:
    ViewAdapter();
    virtual ~ViewAdapter();

    void setWnd(HWND hwnd) { _hwnd = hwnd; }

    void onSize(int w, int h, int dpi = 0);
    void onDraw(HDC hdc);
    void drawTo(GiCanvas* canvas);
    bool onLButtonDown(int x, int y, WPARAM wparam);
    bool onLButtonUp(int x, int y);
    bool onMouseMove(int x, int y, WPARAM wparam);
    bool onRButtonDown(int x, int y);
    bool onRButtonUp(int x, int y);

    bool setCommand(const char* name);

    virtual void regenAll(bool changed);
    virtual void regenAppend(int sid, long playh);
    virtual void redraw(bool changed);
    virtual bool useFinger() { return false; }

private:
    GiMouseHelper*  _helper;
    GiCoreView*     _coreView;
    HWND            _hwnd;
    GiGdipCanvas    _canvas;
};

#endif // TEST_VIEW_ADAPTER_H
