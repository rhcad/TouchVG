//! \file touchglview.h
//! \brief Define the core view class using OpenVG: TouchGLView.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_OPENVGVIEW_H
#define TOUCHVG_CORE_OPENVGVIEW_H

class GiCanvas;

//! The core view class using OpenVG.
/*! \ingroup GRAPH_INTERFACE
 */
class TouchGLView
{
public:
    TouchGLView(int widthPt, int heightPt, int pixelsPerPoint = 1);
    ~TouchGLView();
    
    void resize(int widthPx, int heightPx);
    int getWidthPx();
    int getHeightPx();
    void prepareToDraw();
    
    GiCanvas* beginPaint(bool pathCached) const;
    void endPaint();
    
private:
    struct Impl;
    Impl* _impl;
};

#endif // TOUCHVG_CORE_OPENVGVIEW_H
