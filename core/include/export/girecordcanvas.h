//! \file girecordcanvas.h
//! \brief Define the canvas adapter class to record drawing: GiRecordCanvas.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_GIRECORDCANVAS_H
#define TOUCHVG_CORE_GIRECORDCANVAS_H

#include "gicanvas.h"

class MgRecordShape;
class MgShape;
class MgShapes;
class GiTransform;

//! The canvas adapter class to record drawing.
/*! \ingroup CORE_VIEW
 */
class GiRecordCanvas : public GiCanvas
{
public:
    GiRecordCanvas(MgShapes* shapes, const GiTransform* xf, int ignoreId);
    virtual ~GiRecordCanvas() { clear(); }
    
    void clear();
    
private:
    virtual bool beginShape(int type, int sid, int version, float x, float y, float w, float h);
    virtual void endShape(int type, int sid, float x, float y);
    virtual void setPen(int argb, float width, int style, float phase, float orgw);
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
    virtual bool drawHandle(float x, float y, int type, float angle);
    virtual bool drawBitmap(const char* name, float xc, float yc,
                            float w, float h, float angle);
    virtual float drawTextAt(const char* text, float x, float y, float h, int align, float angle);
    virtual float drawTextAt(GiTextWidthCallback* c, const char* text, float x, float y, float h, int align, float angle);
    
private:
    const Matrix2d d2w() const;

private:
    MgShapes*       _shapes;
    MgShape*        _shape;
    MgRecordShape*  _sp;
    const GiTransform* _xf;
    int             _ignoreId;
};

#endif // TOUCHVG_CORE_GIRECORDCANVAS_H
