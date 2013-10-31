//! \file svgcanvas.h
//! \brief 定义输出SVG文件的画布适配器类 GiSvgCanvas
// Copyright (c) 2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_SVGCANVAS_H_
#define TOUCHVG_CORE_SVGCANVAS_H_

#include "gicanvas.h"

//! 输出SVG文件的画布适配器类
/*! \ingroup CORE_STORAGE
 */
class GiSvgCanvas : public GiCanvas
{
public:
    GiSvgCanvas();
    virtual ~GiSvgCanvas();

    bool open(const char* filename, int width, int height);
    bool close();

private:
    virtual void setPen(int argb, float width, int style, float phase);
    virtual void setBrush(int argb, int style);
    virtual void clearRect(float x, float y, float w, float h);
    virtual void drawRect(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void drawLine(float x1, float y1, float x2, float y2);
    virtual void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void beginPath();
    virtual void moveTo(float x, float y);
    virtual void lineTo(float x, float y);
    virtual void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    virtual void quadTo(float cpx, float cpy, float x, float y);
    virtual void closePath();
    virtual void drawPath(bool stroke, bool fill);
    virtual void saveClip();
    virtual void restoreClip();
    virtual bool clipRect(float x, float y, float w, float h);
    virtual bool clipPath();
    virtual void drawHandle(float x, float y, int type);
    virtual void drawBitmap(const char* name, float xc, float yc, 
                            float w, float h, float angle);
    virtual float drawTextAt(const char* text, float x, float y, float h, int align);
    virtual bool beginShape(int sid, float x, float y, float w, float h);
    virtual void endShape(int sid, float x, float y);

private:
    struct Impl;
    Impl*   im;
};

#endif // TOUCHVG_CORE_SVGCANVAS_H_
