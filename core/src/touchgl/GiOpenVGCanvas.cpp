//! \file GiOpenVGCanvas.cpp
//! \brief Implement GiCanvas adapter class: GiOpenVGCanvas.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#include "GiOpenVGCanvas.h"
#include "openvg.h"
#include "vgu.h"
#include <vector>
#include <map>

static const float patDash[]      = { 4, 2, 0 };
static const float patDot[]       = { 1, 2, 0 };
static const float patDashDot[]   = { 10, 2, 2, 2, 0 };
static const float dashDotdot[]   = { 20, 2, 2, 2, 2, 2, 0 };
static const float* const LINEDASH[] = { patDash, patDot, patDashDot, dashDotdot };

class PathCache
{
    std::vector<VGPath> paths;
    unsigned            index;
    unsigned            maxCount;
public:
    PathCache() : index(0), maxCount(200000) {}
    ~PathCache() { clear(); }
    
    void clear() {
        for (unsigned i = 0; i < paths.size(); i++) {
            vgClearPath(paths[i], VG_PATH_CAPABILITY_ALL);
            vgDestroyPath(paths[i]);
        }
        paths.clear();
        index = 0;
    }
    
    VGPath pickPath(bool &created) {
        created = (index == paths.size());
        if (created) {
            paths.push_back(vgCreatePath(VG_PATH_FORMAT_STANDARD, VG_PATH_DATATYPE_F,
                                         1,0,0,0, VG_PATH_CAPABILITY_ALL));
        }
        return index < paths.size() ? paths[index++] : 0;
    }
    
    void beginPaint(bool cached) {
        maxCount = cached ? maxCount : index;
        while (paths.size() > maxCount) {
            vgClearPath(paths.back(), VG_PATH_CAPABILITY_ALL);
            vgDestroyPath(paths.back());
            paths.pop_back();
        }
        if (cached) {
            index = 0;
        }
    }
};

class PaintCache
{
    std::map<int, VGPaint> paints;
public:
    PaintCache() {}
    ~PaintCache() { clear(); }
    
    void clear() {
        for (std::map<int, VGPaint>::iterator it = paints.begin(); it != paints.end(); ++it) {
            vgDestroyPaint(it->second);
        }
        paints.clear();
    }
    
    VGPaint pick(bool &created, int argb) {
        std::map<int, VGPaint>::iterator it = paints.find(argb);
        VGPaint p = (it == paints.end()) ? vgCreatePaint() : it->second;
        
        created = p && it == paints.end();
        if (created) {
            paints[argb] = p;
        }
        return p;
    }
};

struct GiOpenVGCanvas::Impl
{
    typedef std::pair<PathCache*, int> PathsVersion;
    typedef std::map<int, PathsVersion> ShapeMap;
    ShapeMap    shapes;
    
    VGPaint		stroke;
    VGPaint		fill;
    VGbitfield  paintModes;
    bool        autosave;
    
    VGPath      curpath;
    PathCache*  paths;
    PaintCache* paints;
    bool        needBuildPath;
    
    Impl() : stroke(0), fill(0), paintModes(0), curpath(0), paths(0), paints(0) {}
    ~Impl() {
        clear();
    }
    PathCache* getPaths() {
        if (!paths) {
            paths = new PathCache();
            paths->beginPaint(autosave);
        }
        return paths;
    }
    void clear() {
        for (ShapeMap::iterator it = shapes.begin(); it != shapes.end(); ++it) {
            if (paths == it->second.first)
                paths = 0;
            delete it->second.first;
        }
        shapes.clear();
        
        if (paths) {
            delete paths;
            paths = 0;
        }
        if (paints) {
            delete paints;
            paints = 0;
        }
    }
};

GiOpenVGCanvas::GiOpenVGCanvas()
{
    im = new Impl();
}

GiOpenVGCanvas::~GiOpenVGCanvas()
{
    delete im;
}

void GiOpenVGCanvas::clear()
{
    im->clear();
}

void GiOpenVGCanvas::beginPaint(bool autosave)
{
    im->autosave = autosave;
    if (!im->paintModes) {
        setPen(0xFF000000, 1, 0, 0, 0);
    }
    if (im->paths) {
        im->paths->beginPaint(autosave);
    }
}

static VGfloat colorPart(int argb, int byteOrder)
{
    return (VGfloat)((argb >> (byteOrder * 8)) & 0xFF) / 255.f;
}

void GiOpenVGCanvas::setPen(int argb, float width, int style, float phase, float)
{
    if (argb != 0) {
        VGfloat alpha = colorPart(argb, 3);
        VGfloat fcolor[4] = { colorPart(argb, 2), colorPart(argb, 1), colorPart(argb, 0), alpha };
        
        if (alpha < 1e-3f) {
            im->paintModes &= ~VG_STROKE_PATH;
            im->stroke = 0;
        }
        else {
            bool created;
            if (!im->paints)
                im->paints = new PaintCache();
            im->stroke = im->paints->pick(created, argb);
            if (created) {
                vgSetParameterfv(im->stroke, VG_PAINT_COLOR, 4, &fcolor[0]);
            }
            vgSetPaint(im->stroke, VG_STROKE_PATH);
            im->paintModes |= VG_STROKE_PATH;
        }
    }
    if (width > 0) {
        vgSetf(VG_STROKE_LINE_WIDTH, width);
    }
    if (style > 0 && style < 5) {
        VGfloat pattern[6];
        int n = 0;
        
        for (; LINEDASH[style - 1][n] > 0.1f; n++) {
            pattern[n] = LINEDASH[style - 1][n] * (width < 1.f ? 1.f : width);
        }
        vgSetfv(VG_STROKE_DASH_PATTERN, n, pattern);
        vgSetf(VG_STROKE_DASH_PHASE, phase);
        vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_BUTT);
    }
    else if (0 == style) {
        vgSetfv(VG_STROKE_DASH_PATTERN, 0, 0);
        vgSeti(VG_STROKE_CAP_STYLE, VG_CAP_ROUND);
    }
}

void GiOpenVGCanvas::setBrush(int argb, int style)
{
    if (0 == style) {
        VGfloat alpha = colorPart(argb, 3);
        VGfloat fcolor[4] = { colorPart(argb, 2), colorPart(argb, 1), colorPart(argb, 0), alpha };
        
        if (alpha < 1e-3f) {
            im->paintModes &= ~VG_FILL_PATH;
            im->fill = 0;
        }
        else {
            bool created;
            if (!im->paints)
                im->paints = new PaintCache();
            im->fill = im->paints->pick(created, argb);
            if (created) {
                vgSetParameterfv(im->fill, VG_PAINT_COLOR, 4, &fcolor[0]);
                vgSeti(VG_FILL_RULE, VG_EVEN_ODD);
            }
            vgSetPaint(im->fill, VG_FILL_PATH);
            im->paintModes |= VG_FILL_PATH;
        }
    }
}

bool GiOpenVGCanvas::beginShape(int, int sid, int version, float, float, float, float)
{
    if (im->autosave) {
        PathCache* paths;
        Impl::ShapeMap::iterator it = im->shapes.find(sid);
        
        if (it == im->shapes.end()) {
            paths = new PathCache();
            im->shapes[sid] = Impl::PathsVersion(paths, version);
        }
        else {
            paths = it->second.first;
            if (it->second.second != version)
                paths->clear();
        }
        
        if (im->paths)
            delete im->paths;
        im->paths = paths;
    }
    
    return true;
}

void GiOpenVGCanvas::endShape(int, int, float, float)
{
    im->paths = 0;
}

void GiOpenVGCanvas::saveClip()
{
    //CGContextSaveGState(_ctx);
}

void GiOpenVGCanvas::restoreClip()
{
    //CGContextRestoreGState(_ctx);
}

void GiOpenVGCanvas::clearRect(float x, float y, float w, float h)
{
    vgClear(x, y, w, h);
}

void GiOpenVGCanvas::drawRect(float x, float y, float w, float h, bool stroke, bool fill)
{
    bool created;
    VGPath p = im->getPaths()->pickPath(created);
    if (created) {
        vguRect(p, x, y, w, h);
    }
    vgDrawPath(p, im->paintModes & (stroke ? -1 : ~VG_STROKE_PATH) & (fill ? -1 : ~VG_FILL_PATH));
}

bool GiOpenVGCanvas::clipRect(float x, float y, float w, float h)
{
    return false;
}

void GiOpenVGCanvas::drawLine(float x1, float y1, float x2, float y2)
{
    bool created;
    VGPath p = im->getPaths()->pickPath(created);
    if (created) {
        vguLine(p, x1, y1, x2, y2);
    }
    vgDrawPath(p, im->paintModes & ~VG_FILL_PATH);
}

void GiOpenVGCanvas::drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
{
    bool created;
    VGPath p = im->getPaths()->pickPath(created);
    if (created) {
        vguEllipse(p, x + w / 2, y + h / 2, w, h);
    }
    vgDrawPath(p, im->paintModes & (stroke ? -1 : ~VG_STROKE_PATH) & (fill ? -1 : ~VG_FILL_PATH));
}

void GiOpenVGCanvas::beginPath()
{
    im->curpath = im->getPaths()->pickPath(im->needBuildPath);
}

void GiOpenVGCanvas::moveTo(float x, float y)
{
    if (im->curpath && im->needBuildPath) {
        VGubyte seg = VG_MOVE_TO_ABS;
        VGfloat data[2] = { x, y };
        
        vgAppendPathData(im->curpath, 1, &seg, data);
    }
}

void GiOpenVGCanvas::lineTo(float x, float y)
{
    if (im->curpath && im->needBuildPath) {
        VGubyte seg = VG_LINE_TO_ABS;
        VGfloat data[2] = { x, y };
        
        vgAppendPathData(im->curpath, 1, &seg, data);
    }
}

void GiOpenVGCanvas::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    if (im->curpath && im->needBuildPath) {
        VGubyte seg = VG_CUBIC_TO_ABS;
        VGfloat data[6] = { c1x, c1y, c2x, c2y, x, y };
        
        vgAppendPathData(im->curpath, 1, &seg, data);
    }
}

void GiOpenVGCanvas::quadTo(float cpx, float cpy, float x, float y)
{
    if (im->curpath && im->needBuildPath) {
        VGubyte seg = VG_QUAD_TO_ABS;
        VGfloat data[4] = { cpx, cpy, x, y };
        
        vgAppendPathData(im->curpath, 1, &seg, data);
    }
}

void GiOpenVGCanvas::closePath()
{
    if (im->curpath && im->needBuildPath) {
        VGubyte seg = VG_CLOSE_PATH;
        VGfloat data = 0.0f;
        
        vgAppendPathData(im->curpath, 1, &seg, &data);
    }
}

void GiOpenVGCanvas::drawPath(bool stroke, bool fill)
{
    vgDrawPath(im->curpath, im->paintModes & (stroke ? -1 : ~VG_STROKE_PATH) & (fill ? -1 : ~VG_FILL_PATH));
}

bool GiOpenVGCanvas::clipPath()
{
    return false;
}

void GiOpenVGCanvas::drawHandle(float x, float y, int type)
{
}

void GiOpenVGCanvas::drawBitmap(const char* name, float xc, float yc, float w, float h, float angle)
{
}

float GiOpenVGCanvas::drawTextAt(const char* text, float x, float y, float h, int align)
{
    return 0;
}
