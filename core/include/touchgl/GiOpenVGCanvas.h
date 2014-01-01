//! \file GiOpenVGCanvas.h
//! \brief Define GiCanvas adapter class: GiOpenVGCanvas.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CANVASADAPTER_OPENVG_H
#define TOUCHVG_CANVASADAPTER_OPENVG_H

#include "gicanvas.h"

//! GiCanvas adapter using OpenVG.
/*! \ingroup GRAPH_INTERFACE
 */
class GiOpenVGCanvas : public GiCanvas
{
public:
    GiOpenVGCanvas();
    virtual ~GiOpenVGCanvas();
    
    void beginPaint(bool autosave);
    void endPaint() {}
    void clear();
    
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
    void drawHandle(float x, float y, int type);
    void drawBitmap(const char* name, float xc, float yc, float w, float h, float angle);
    float drawTextAt(const char* text, float x, float y, float h, int align);
    bool beginShape(int type, int sid, int version, float x, float y, float w, float h);
    void endShape(int type, int sid, float x, float y);
    
private:
    struct Impl;
    Impl*   im;
};

#endif // TOUCHVG_CANVASADAPTER_OPENVG_H
