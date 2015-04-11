//! \file gicanvas.h
//! \brief Define the canvas callback interface: GiCanvas.
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_GICANVAS_H
#define TOUCHVG_CORE_GICANVAS_H

#ifndef SWIG
//! Callback interface for receiving actual width result of drawTextAt().
struct GiTextWidthCallback {
    virtual ~GiTextWidthCallback() {}
    virtual void addRefTextWidth() {}
    virtual void releaseTextWidth() {}
    virtual void drawTextEnded(GiTextWidthCallback *c, float width) = 0;
};
#endif

//! Canvas callback interface device-dependent.
/*! Implement a derived class with a graphics library which may be device-dependent.
    The default unit of its drawing functions is the point (usually equal to the pixel).
    The default drawing attributes (may be assigned in its beginPaint function) are:
        1px black pen, solid line, round end caps, not fill.
    \ingroup GRAPH_INTERFACE
 */
class GiCanvas
{
public:
    virtual ~GiCanvas() {}
    
    enum {  //!< for setPen(..style..)
        kLineDashMask   = 0xFFF,
        kLineCapDefault = 0,
        kLineCapMask    = 0x70000,
        kLineCapButt    = 0x10000,
        kLineCapRound   = 0x20000,
        kLineCapSquare  = 0x40000,
        
        kAlignLeft      = 0,
        kAlignCenter    = 1,
        kAlignRight     = 2,
        kAlignHorz      = 0xF,
        kAlignTop       = 0,
        kAlignBottom    = 0x10,
        kAlignVCenter   = 0x20,
        kAlignVert      = 0xF0,
    };
    
    //! Set attributes of the current pen to stroke edges.
    /*!
        \param argb Stroke color, ignored if equals to zero, alpha = (argb>>24) & 0xFF,
                    red = (argb>>16) & 0xFF, green = (argb>>8) & 0xFF, blue = argb & 0xFF.
        \param width Line width in point unit, ignored if <= 0.
        \param style Line dash type (& kLineDashMask), ignored if < 0.
                    0:solid line, 1:dash line, 2:...., 3:-.-.-, 4:-..-.., 5:null line, >5:customized line.
                    Round end caps for solid line, flat end caps for the four dash types.
        \param phase Dash pattern's offset in point unit.
        \param orgw Orginal line width, >0: in 0.01mm unit, 0: 1px, <0: in point unit.
     */
    virtual void setPen(int argb, float width, int style, float phase, float orgw) = 0;
    
    //! Set attributes of the current brush to fill shapes.
    /*!
        \param argb Fill color, alpha = (argb>>24) & 0xFF.
        \param style Fill type, 0: fill with a solid color, >0: customized fill type.
     */
    virtual void setBrush(int argb, int style) = 0;
    
    
    //! Clear `rect' (that is, set the region within the rect to transparent).
    virtual void clearRect(float x, float y, float w, float h) = 0;
    
    //! Fill or stroke `rect' (w>0, h>0) with the current color.
    virtual void drawRect(float x, float y, float w, float h, bool stroke, bool fill) = 0;
    
    //! Stroke a line with the current color.
    virtual void drawLine(float x1, float y1, float x2, float y2) = 0;
    
    //! Fill or stroke a ellipse with a corner and size (w>0, h>0).
    virtual void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill) = 0;
    
    
    //! Start a new path.
    virtual void beginPath() = 0;
    
    //! Ready to add a subpath to the current path at the start point.
    virtual void moveTo(float x, float y) = 0;
    
    //! Add a line segment to the current subpath.
    virtual void lineTo(float x, float y) = 0;
    
    //! Add a cubic bezier segment to the current subpath.
    virtual void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) = 0;
    
    //! Add a quadratic bezier segment to the current subpath.
    virtual void quadTo(float cpx, float cpy, float x, float y) = 0;
    
    //! Close the current subpath of the path added by beginPath().
    virtual void closePath() = 0;
    
    //! Draw and clear the current path added by beginPath().
    virtual void drawPath(bool stroke, bool fill) = 0;
    
    //! Save the current clipping path.
    /*! Called it before clipRect() or clipPath().
        \see restoreClip
     */
    virtual void saveClip() = 0;
    
    //! Restore the current clipping path.
    virtual void restoreClip() = 0;
    
    //! Intersect the current clipping path with `rect'.
    /*! Note that this function resets the context's path to the empty path.
        \return true if successful and the result clipping path isn't empty.
        \see saveClip, restoreClip
     */
    virtual bool clipRect(float x, float y, float w, float h) = 0;
    
    //! Intersect the current clipping path with the path added by beginPath().
    /*! Note that this function resets the context's path to the empty path.
        \return true if successful and the result clipping path isn't empty.
        \see saveClip, restoreClip, beginPath
     */
    virtual bool clipPath() = 0;
    
    //! Draw a symbol (GiHandleTypes) whose center will at (x, y).
    virtual bool drawHandle(float x, float y, int type, float angle) = 0;
    
    //! Draw a image whose center will at (xc, yc).
    /*!
        \param name The image's identify which is usually a filename without path.
        \param xc The center X position of the image.
        \param yc The center Y position of the image.
        \param w Display width of the image.
        \param h Display height of the image.
        \param angle Rotate angle in radians, it's positive direction is CCW 
                in the world coordinate system.
        \return true if successful
     */
    virtual bool drawBitmap(const char* name, float xc, float yc, 
                            float w, float h, float angle) = 0;
    
    //! Draw text in one line.
    /*! Uses the current font and fill color to draw the text aligned on top.
        \param text The text to draw.
        \param x X of horizontal alignment position.
        \param y Y of horizontal alignment position.
        \param h Text height in point(or pixel) unit.
        \param align alignment type, kAlignLeft|kAlignTop.
        \param angle Rotate angle in radians, it's positive direction is CCW
                in the world coordinate system.
        \return Actual display width.
     */
    virtual float drawTextAt(const char* text, float x, float y, float h, int align, float angle) = 0;
    
#ifndef SWIG
    virtual float drawTextAt(GiTextWidthCallback* c, const char* text, float x, float y, float h, int align, float angle) {
        float w = drawTextAt(text, x, y, h, align, angle);
        if (c) c->drawTextEnded(c, w);
        return w;
    }
    
    //! Clear the cached bitmap for re-drawing on desktop PC.
    virtual void clearCachedBitmap(bool clearAll = false) {}
#endif

    //! Ready to draw a shape.
    virtual bool beginShape(int type, int sid, int version,
                            float x, float y, float w, float h) { return true; }
    
    //! Complete to draw a shape.
    virtual void endShape(int type, int sid, float x, float y) {}
};

#endif // TOUCHVG_CORE_GICANVAS_H
