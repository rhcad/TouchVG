//! \file GiGdipCanvas.h
//! \brief 定义使用GDI+实现的画布适配器类 GiGdipCanvas
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_GIGDIPCANVAS_H
#define TOUCHVG_GIGDIPCANVAS_H

#include <gicanvas.h>

//! 使用GDI+实现的画布适配器类.
/*! \ingroup GRAPH_WIN
 */
class GiGdipCanvas : public GiCanvas
{
public:
    GiGdipCanvas();
    virtual ~GiGdipCanvas();
    
    //! 给定绘图设备DC，开始绘制
    bool beginPaint(HWND hwnd, HDC hdc);

    //! 给定缓冲位图的大小，开始绘制并清除背景
    bool beginPaintBuffered(int width, int height, COLORREF bkcolor = (COLORREF)-1);

    //! 结束绘制
    void endPaint();

    //! 使用背景色清除显示
    void clearWindow();

    //! 显示上次缓存的内容
    bool drawCachedBitmap(int x = 0, int y = 0, bool secondBmp = false);

    //! 显示另一个画布的上次缓存的内容
    bool drawCachedBitmap2(GiGdipCanvas& cv, bool secondBmp = false, int x = 0, int y = 0);

    //! 缓存显示的内容
    void saveCachedBitmap(bool secondBmp = false);

    //! 清除后备缓冲位图，以便重新绘图
    void clearCachedBitmap(bool clearAll = false);

    //! 返回是否有已缓存的显示内容
    bool hasCachedBitmap(bool secondBmp = false) const;

    //! 返回当前是否在缓冲位图上绘图，调用endPaint()前有效
    bool isBufferedDrawing() const;


    //! 从缓冲位图生成指定宽高上限的缩略图，该图为新画布对象的缓冲位图
    GiGdipCanvas* thumbnailImage(int maxWidth, int maxHeight);

    //! 将缓冲位图保存到文件中
    bool save(const wchar_t* filename, long jpegQuality = 100);
    
public:
    void setPen(int argb, float width, int style, float phase, float orgw);
    void setBrush(int argb, int style);
    void clearRect(float x, float y, float w, float h);
    void drawRect(float x, float y, float w, float h, bool stroke, bool fill);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
    void beginPath();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    void quadTo(float cpx, float cpy, float x, float y);
    void closePath();
    void drawPath(bool stroke, bool fill);
    void saveClip();
    void restoreClip();
    bool clipRect(float x, float y, float w, float h);
    bool clipPath();
    bool drawHandle(float x, float y, int type);
    bool drawBitmap(const char* name, float xc, float yc, float w, float h, float angle);
    float drawTextAt(const char* text, float x, float y, float h, int align);
    
private:
    bool getEncoder(const wchar_t* filename, wchar_t format[20], CLSID& clsidEncoder);
    void _beginPaint();

private:
    struct Impl;
    Impl    *_impl;
};

#endif // TOUCHVG_GIGDIPCANVAS_H
