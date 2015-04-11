// svgcanvas.cpp
// Copyright (c) 2013-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "svgcanvas.h"
#include "simple_svg.hpp"

using namespace svg;

struct GiSvgCanvas::Impl
{
    Document    *doc;
    Stroke      *pen;
    Fill        *brush;
    std::stringstream d;

    Impl() : doc(NULL), pen(NULL), brush(NULL) {}
    Stroke getPen(bool stroke = true) { return stroke && pen ? *pen : Stroke(); }
    Fill getBrush(bool fill) { return fill && brush ? *brush : Fill(); }
};

GiSvgCanvas::GiSvgCanvas()
{
    im = new Impl();
}

GiSvgCanvas::~GiSvgCanvas()
{
    close();
    delete im;
}

bool GiSvgCanvas::open(const char* filename, int width, int height)
{
    if (im->doc || !filename || width < 1 || height < 1) {
        return false;
    }

    Dimensions dimensions((double)width, (double)height);
    im->doc = new Document(filename, Layout(dimensions, Layout::TopLeft));

    return true;
}

bool GiSvgCanvas::close()
{
    bool ret = false;

    if (im->doc) {
        delete im->pen;
        im->pen = NULL;

        delete im->brush;
        im->brush = NULL;

        ret = im->doc->save();
        delete im->doc;
        im->doc = NULL;
    }
    
    return ret;
}

bool GiSvgCanvas::beginShape(int, int sid, int, float, float, float, float)
{
    std::stringstream ss;
    ss << "s" << sid;
    im->doc->pushGroup(ss.str());
    return true;
}

void GiSvgCanvas::endShape(int, int, float, float)
{
    im->doc->popGroup();
}

static const float patDash[]      = { 4, 2, 0 };
static const float patDot[]       = { 1, 2, 0 };
static const float patDashDot[]   = { 10, 2, 2, 2, 0 };
static const float dashDotdot[]   = { 20, 2, 2, 2, 2, 2, 0 };
static const float* const lpats[] = { NULL, patDash, patDot, patDashDot, dashDotdot };

void GiSvgCanvas::setPen(int argb, float width, int style, float phase, float)
{
    if (width > 0 && style >= 0) {
        int a = (argb>>24) & 0xFF;
        int r = (argb>>16) & 0xFF;
        int g = (argb>>8) & 0xFF;
        int b = argb & 0xFF;
        int linecap = style & kLineCapMask;

        delete im->pen;
        im->pen = new Stroke(width, Color(r, g, b, a));
        
        style = style & kLineDashMask;
        if (style > 0 && style < 5) {
            std::stringstream dasharray;
            
            for (int i = 0; lpats[style][i] > 0.1f; i++) {
                if (i > 0) dasharray << ", ";
                dasharray << (lpats[style][i] * (width < 1.f ? 1.f : width));
            }
            im->pen->dasharray = dasharray.str();
            im->pen->dashoffset = phase;
        }
        if (linecap & kLineCapButt)
            im->pen->linecap = "butt";
        else if (linecap & kLineCapRound)
            im->pen->linecap = "round";
        else if (linecap & kLineCapSquare)
            im->pen->linecap = "square";
        else
            im->pen->linecap = (style > 0 && style < 5) ? "butt" : "round";
    }
}

void GiSvgCanvas::setBrush(int argb, int style)
{
    if (style == 0) {
        int a = (argb>>24) & 0xFF;
        int r = (argb>>16) & 0xFF;
        int g = (argb>>8) & 0xFF;
        int b = argb & 0xFF;

        delete im->brush;
        im->brush = new Fill(Color(r, g, b, a));
    }
}

void GiSvgCanvas::clearRect(float, float, float, float)
{
}

void GiSvgCanvas::drawRect(float x, float y, float w, float h, bool stroke, bool fill)
{
    *im->doc << Rectangle(Point(x, y), w, h,
        im->getBrush(fill), im->getPen(stroke));
}

void GiSvgCanvas::drawLine(float x1, float y1, float x2, float y2)
{
    *im->doc << Line(Point(x1, y1), Point(x2, y2), im->getPen());
}

void GiSvgCanvas::drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
{
    *im->doc << Elipse(Point(x + w / 2, y + h / 2), w, h,
        im->getBrush(fill), im->getPen(stroke));
}

void GiSvgCanvas::beginPath()
{
    im->d.str("");  // clear() not clear buffer!
}

void GiSvgCanvas::moveTo(float x, float y)
{
    im->d << "M" << x << " " << y;
}

void GiSvgCanvas::lineTo(float x, float y)
{
    im->d << "L" << x << " " << y;
}

void GiSvgCanvas::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    im->d << "C" << c1x << " " << c1y << " "
        << c2x << " " << c2y << " " << x << " " << y;
}

void GiSvgCanvas::quadTo(float cpx, float cpy, float x, float y)
{
    im->d << "Q" << cpx << " " << cpy << " " << x << " " << y;
}

void GiSvgCanvas::closePath()
{
    im->d << "Z";
}

void GiSvgCanvas::drawPath(bool stroke, bool fill)
{
    if (stroke || fill) {
        *im->doc << Path(im->d.str(), im->getBrush(fill), im->getPen(stroke));
    }
}

void GiSvgCanvas::saveClip()
{
}

void GiSvgCanvas::restoreClip()
{
}

bool GiSvgCanvas::clipRect(float, float, float, float)
{
    return false;
}

bool GiSvgCanvas::clipPath()
{
    return false;
}

bool GiSvgCanvas::drawHandle(float, float, int, float)
{
    return false;
}

bool GiSvgCanvas::drawBitmap(const char*, float, float, float, float, float)
{
    return false;
}

float GiSvgCanvas::drawTextAt(const char* text, float x, float y, float, int, float)
{
    *im->doc << Text(Point(x, y), text, im->getBrush(true));
    return 0;
}
