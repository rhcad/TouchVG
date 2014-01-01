//! \file touchglview.cpp
//! \brief Implement the core view class using OpenVG: TouchGLView.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#include "touchglview.h"
#include "GiOpenVGCanvas.h"
#include "vgext.h"
#include <math.h>

struct TouchGLView::Impl
{
    GiOpenVGCanvas  canvas;
    int             width;
    int             height;
    float           scale;
};

TouchGLView::TouchGLView(int widthPt, int heightPt, int pixelsPerPoint)
{
    _impl = new Impl();
    _impl->width = widthPt * pixelsPerPoint;
    _impl->height = heightPt * pixelsPerPoint;
    _impl->scale = pixelsPerPoint;
    
    vgCreateContextMNK(_impl->width, _impl->height, VG_RENDERING_BACKEND_TYPE_OPENGLES20);
}

TouchGLView::~TouchGLView()
{
    delete _impl;
    vgDestroyContextMNK();
}

void TouchGLView::resize(int widthPx, int heightPx)
{
    _impl->width = widthPx;
    _impl->height = heightPx;
    vgResizeSurfaceMNK(widthPx, heightPx);
}

int TouchGLView::getWidthPx()
{
    return _impl->width;
}

int TouchGLView::getHeightPx()
{
    return _impl->height;
}

void TouchGLView::clear()
{
    _impl->canvas.clear();
}

GiCanvas* TouchGLView::beginPaint(bool pathCached) const
{
    _impl->canvas.beginPaint(pathCached);
    return &_impl->canvas;
}

void TouchGLView::endPaint()
{
    _impl->canvas.endPaint();
}

void TouchGLView::prepareToDraw()
{
	VGfloat clearColor[] = {1,1,1,1};
	vgSetfv(VG_CLEAR_COLOR, 4, clearColor);
	
	vgSeti(VG_MATRIX_MODE, VG_MATRIX_PATH_USER_TO_SURFACE);
	vgLoadIdentity();
    vgScale(_impl->scale, -_impl->scale);
    vgTranslate(0, _impl->height);
}
