// gigraph.cpp: 实现图形系统类 GiGraphics
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "gigraph_.h"
#include <mglnrel.h>
#include <mgcurv.h>
#include "giplclip.h"

#ifndef SafeCall
#define SafeCall(p, f)      if (p) p->f
#endif

GiGraphics::GiGraphics(GiTransform* xform)
{
    m_impl = new GiGraphicsImpl(xform);
}

GiGraphics::~GiGraphics()
{
    delete m_impl;
}

void GiGraphics::copy(const GiGraphics& src)
{
    if (this != &src)
    {
        m_impl->maxPenWidth = src.m_impl->maxPenWidth;
        m_impl->drawColors = src.m_impl->drawColors;
    }
}

const GiTransform& GiGraphics::xf() const
{
    return *m_impl->xform;
}

GiTransform& GiGraphics::_xf()
{
    return *m_impl->xform;
}

bool GiGraphics::beginPaint(GiCanvas* canvas, const RECT_2D& clipBox)
{
    if (!canvas || m_impl->canvas || m_impl->drawRefcnt > 0) {
        return false;
    }
    
    m_impl->canvas = canvas;
    m_impl->ctxused = 0;
    giInterlockedIncrement(&m_impl->drawRefcnt);
    
    if (m_impl->lastZoomTimes != xf().getZoomTimes()) {
        m_impl->zoomChanged();
        m_impl->lastZoomTimes = xf().getZoomTimes();
    }
    
    m_impl->clipBox0 = clipBox;
    if (Box2d(clipBox).isEmpty()) {
        (xf().getWndRectW() * xf().worldToDisplay()).get(m_impl->clipBox0);
    }
    m_impl->clipBox  = m_impl->clipBox0;
    m_impl->rectDraw = m_impl->clipBox0;
    m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
    m_impl->rectDrawM = Box2d(m_impl->rectDraw) * xf().displayToModel();
    m_impl->rectDrawMaxM = xf().getWndRectM();
    m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
    m_impl->rectDrawMaxW = m_impl->rectDrawMaxM * xf().modelToWorld();
    
    return true;
}

void GiGraphics::endPaint()
{
    m_impl->canvas = NULL;
    giInterlockedDecrement(&m_impl->drawRefcnt);
}

bool GiGraphics::isDrawing() const
{
    return m_impl->drawRefcnt > 0;
}

bool GiGraphics::isPrint() const
{
    return m_impl->isPrint;
}

GiCanvas* GiGraphics::getCanvas()
{
    return m_impl->canvas;
}

Box2d GiGraphics::getClipModel() const
{
    return m_impl->rectDrawM;
}

Box2d GiGraphics::getClipWorld() const
{
    return m_impl->rectDrawW;
}

RECT_2D& GiGraphics::getClipBox(RECT_2D& rc) const
{
    rc = m_impl->clipBox;
    return rc;
}

bool GiGraphics::setClipBox(const RECT_2D& rc)
{
    if (m_impl->drawRefcnt < 1)
        return false;

    bool ret = false;
    Box2d rect;

    if (!rect.intersectWith(Box2d(rc), Box2d(m_impl->clipBox0)).isEmpty())
    {
        if (rect != Box2d(m_impl->clipBox))
        {
            rect.get(m_impl->clipBox);
            m_impl->rectDraw.set(Box2d(rc));
            m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
            m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
            m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
            SafeCall(m_impl->canvas, clipRect(m_impl->clipBox.left, m_impl->clipBox.top,
                                              m_impl->clipBox.width(),
                                              m_impl->clipBox.height()));
        }
        ret = true;
    }

    return ret;
}

bool GiGraphics::setClipWorld(const Box2d& rectWorld)
{
    bool ret = false;

    if (isDrawing() && !rectWorld.isEmpty())
    {
        Box2d box (rectWorld * xf().worldToDisplay());

        box.intersectWith(Box2d(m_impl->clipBox0));
        if (!box.isEmpty(Tol(1, 0)))
        {
            if (box != Box2d(m_impl->clipBox))
            {
                box.get(m_impl->clipBox);
                m_impl->rectDraw = box;
                m_impl->rectDraw.inflate(GiGraphicsImpl::CLIP_INFLATE);
                m_impl->rectDrawM = m_impl->rectDraw * xf().displayToModel();
                m_impl->rectDrawW = m_impl->rectDrawM * xf().modelToWorld();
                SafeCall(m_impl->canvas, clipRect(m_impl->clipBox.left, m_impl->clipBox.top,
                                                  m_impl->clipBox.width(), m_impl->clipBox.height()));
            }

            ret = true;
        }
    }

    return ret;
}

bool GiGraphics::isGrayMode() const
{
    return m_impl->drawColors > 0 && m_impl->drawColors < 8;
}

void GiGraphics::setGrayMode(bool gray)
{
    m_impl->drawColors = gray ? 2 : 0;
}

GiColor GiGraphics::getBkColor() const
{
    return m_impl->bkcolor;
}

GiColor GiGraphics::setBkColor(const GiColor& color)
{
    GiColor old(m_impl->bkcolor);
    m_impl->bkcolor = color;
    return old;
}

GiColor GiGraphics::calcPenColor(const GiColor& color) const
{
    GiColor ret = color;
    
    if (isGrayMode()) {
        unsigned char by = (unsigned char)(
            (77 * ret.r + 151 * ret.g + 28 * ret.b + 128) / 256);
        ret.set(by, by, by);
    }

    return ret;
}

float GiGraphics::calcPenWidth(float lineWidth, bool useViewScale) const
{
    float w = 1;

    if (m_impl->maxPenWidth <= 1)
        lineWidth = 0;

    if (lineWidth > 0) {        // 单位：0.01mm
        w = lineWidth / 2540.f * xf().getDpiY();
        w *= xf().getViewScale();
    }
    else if (lineWidth < 0) {   // 单位：像素
        w = -lineWidth;
        if (useViewScale)
            w *= xf().getViewScale();
    }
    w = mgMin(w, m_impl->maxPenWidth);
    w = mgMax(w, m_impl->minPenWidth);
    //if (lineWidth <= 0 && xf().getDpiY() > getScreenDpi())
    //    w = w * xf().getDpiY() / getScreenDpi();

    return w;
}

void GiGraphics::setMaxPenWidth(float pixels, float minw)
{
    if (minw < 0)
        minw = m_impl->minPenWidth;
    
    if (pixels < 0)
        pixels = m_impl->maxPenWidth;
    else if (pixels < minw)
        pixels = minw;
    else if (pixels > 200)
        pixels = 200;
    
    m_impl->maxPenWidth = pixels;
    m_impl->minPenWidth = minw;
}

static inline const Matrix2d& S2D(const GiTransform& xf, bool modelUnit)
{
    return modelUnit ? xf.modelToDisplay() : xf.worldToDisplay();
}

static inline const Box2d& DRAW_RECT(const GiGraphicsImpl* p, bool modelUnit)
{
    return modelUnit ? p->rectDrawM : p->rectDrawW;
}

static inline const Box2d& DRAW_MAXR(const GiGraphicsImpl* p, bool modelUnit)
{
    return modelUnit ? p->rectDrawMaxM : p->rectDrawMaxW;
}

bool GiGraphics::drawLine(const GiContext* ctx, 
                          const Point2d& startPt, const Point2d& endPt, 
                          bool modelUnit)
{
    if (m_impl->drawRefcnt == 0)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(Box2d(startPt, endPt)))
        return false;

    Point2d pts[2] = { startPt * S2D(xf(), modelUnit), endPt * S2D(xf(), modelUnit) };

    if (!mglnrel::clipLine(pts[0], pts[1], m_impl->rectDraw))
        return false;

    return rawLine(ctx, pts[0].x, pts[0].y, pts[1].x, pts[1].y);
}

//! 折线绘制辅助类，用于将显示与环境设置分离
class PolylineAux
{
    GiGraphics* m_gs;
    const GiContext* m_pContext;
public:
    PolylineAux(GiGraphics* gs, const GiContext* ctx)
        : m_gs(gs), m_pContext(ctx)
    {
    }
    bool draw(const Point2d* pxs, int n) const
    {
        return pxs && n > 1 && m_gs->rawLines(m_pContext, pxs, n);
    }
};

static bool DrawEdge(int count, int &i, Point2d* pts, Point2d &ptLast, 
                     const PolylineAux& aux, const Box2d& rectDraw)
{
    int n, si, ei;
    Point2d pt1, pt2;

    pt1 = ptLast;
    ptLast = pts[i+1];
    pt2 = ptLast;
    if (!mglnrel::clipLine(pt1, pt2, rectDraw))    // 该边不可见
        return false;

    si = i;                                 // 收集第一条可见边
    ei = i+1;
    pts[i] = pt1;                           // 收集起点
    if (pt2 != ptLast)                      // 如果该边终点不可见
    {
        pts[i+1] = pt2;                     // 收集交点
    }
    else                                    // 否则继续收集后续点
    {
        i++;                                // 下一边的起点可见，跳过
        for (; i < count - 1; i++)          // 依次处理后续可见边
        {
            pt1 = ptLast;
            ptLast = pts[i+1];
            pt2 = ptLast;
            if (!mglnrel::clipLine(pt1, pt2, rectDraw)) // 该边不可见
                break;
            ei++;
            if (pt2 != ptLast)              // 该边起点可见，终点不可见
            {
                pts[i+1] = pt2;             // 收集交点
                break;
            }
        }
    }

    // 显示找到的多条线段
    n = ei - si + 1;
    if (n > 1)
    {
        vector<Point2d> pxpoints;
        pxpoints.resize(n);
        Point2d* pxs = &pxpoints.front();
        n = 0;
        for (int j = si; j <= ei; j++)
        {
            // 记下第一个点，其他点如果和上一点不重合则记下，否则跳过
            if (j == si || fabsf(pt1.x - pts[j].x) > 2
                || fabsf(pt1.y - pts[j].y) > 2)
            {
                pt1 = pts[j];
                pxs[n++] = pt1;
            }
        }

        return aux.draw(pxs, n);
    }

    return false;
}

bool GiGraphics::drawLines(const GiContext* ctx, int count, 
                           const Point2d* points, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || points == NULL)
        return false;
    if (count > 0x2000)
        count = 0x2000;
    GiLock lock (&m_impl->drawRefcnt);

    int i;
    Point2d pt1, pt2, ptLast;
    vector<Point2d> pxpoints;
    vector<Point2d> pointBuf;
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        pxpoints.resize(count);
        Point2d* pxs = &pxpoints.front();
        int n = 0;
        for (i = 0; i < count; i++)
        {
            pt2 = points[i] * matD;
            if (i == 0 || fabsf(pt1.x - pt2.x) > 2 || fabsf(pt1.y - pt2.y) > 2)
            {
                pt1 = pt2;
                pxs[n++] = pt2;
            }
        }
        ret = rawLines(ctx, pxs, n);
    }
    else                                            // 部分在显示区域内
    {
        pointBuf.resize(count);
        for (i = 0; i < count; i++)                 // 转换到像素坐标
            pointBuf[i] = points[i] * matD;
        Point2d* pts = &pointBuf.front();

        ptLast = pts[0];
        PolylineAux aux(this, ctx);
        for (i = 0; i < count - 1; i++)
        {
            ret = DrawEdge(count, i, pts, ptLast, aux, m_impl->rectDraw) || ret;
        }
    }

    return ret;
}

bool GiGraphics::drawBeziers(const GiContext* ctx, int count, 
                             const Point2d* points, bool closed, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || points == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    if (count > 0x2000)
        count = 0x2000;
    count = 1 + (count - 1) / 3 * 3;

    bool ret = false;
    vector<Point2d> pxpoints;
    vector<Point2d> pointBuf;
    int i, j, n, si, ei;
    Point2d * pxs;
    Matrix2d matD(S2D(xf(), modelUnit));

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;
    
    if (closed) {
        pxpoints.resize(count);
        pxs = &pxpoints.front();
        for (i = 0; i < count; i++)
            pxs[i] = points[i] * matD;
        ret = rawBeginPath();
        if (ret) {
            rawMoveTo(pxs[0].x, pxs[0].y);
            for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
                rawBezierTo(pxs[i].x, pxs[i].y,
                    pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
            }
            rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }
    else if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        pxpoints.resize(count);
        pxs = &pxpoints.front();
        for (i = 0; i < count; i++)
            pxs[i] = points[i] * matD;
        ret = rawBeziers(ctx, pxs, count);
    }
    else
    {        
        pointBuf.resize(count);
        for (i = 0; i < count; i++)                 // 转换到像素坐标
            pointBuf[i] = points[i] * matD;
        Point2d* pts = &pointBuf.front();

        si = ei = 0;
        for (i = 3; i < count; i += 3)
        {
            for ( ; i < count
                && m_impl->rectDraw.isIntersect(Box2d(4, &pts[ei])); i += 3)
                ei = i;
            n = ei - si + 1;
            if (n > 1)
            {
                pxpoints.resize(n);
                pxs = &pxpoints.front();
                for (j=0; j<n; j++)
                    pxs[j] = pts[si + j];
                ret = rawBeziers(ctx, pxs, n);
            }
            si = ei = i;
        }
    }
    return ret;
}

bool GiGraphics::drawArc(const GiContext* ctx, 
                         const Point2d& center, float rx, float ry, 
                         float startAngle, float sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabsf(sweepAngle) < 1e-5f)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(Box2d(center, 2 * rx, 2 * ry)))
        return false;

    Point2d points[16];
    int count = mgcurv::arcToBezier(points, center,
        rx, ry, startAngle, sweepAngle);
    S2D(xf(), modelUnit).TransformPoints(count, points);

    return count > 3 && rawBeziers(ctx, points, count);
}

bool GiGraphics::drawArc3P(const GiContext* ctx, const Point2d& startpt, 
                           const Point2d& midpt, const Point2d& endpt, 
                           bool modelUnit)
{
    Point2d center;
    float r, startAngle, sweepAngle;
    
    return (mgcurv::arc3P(startpt, midpt, endpt, center, r, &startAngle, &sweepAngle)
            && drawArc(ctx, center, r, r, startAngle, sweepAngle, modelUnit));
}

static inline int findInvisibleEdge(const PolygonClip& clip)
{
    int i = 0;
    for (; i < clip.getCount() && clip.isLinked(i); i++) ;
    return i;
}

static inline int findVisibleEdge(const PolygonClip& clip, int i, int end)
{
    for (; (i - end) % clip.getCount() != 0 && !clip.isLinked(i); i++) ;
    return i;
}

static inline int findInvisibleEdge(const PolygonClip& clip, int i, int end)
{
    for (; (i - end) % clip.getCount() != 0 && clip.isLinked(i); i++) ;
    return i;
}

static bool drawPolygonEdge(const PolylineAux& aux, 
                            int count, const PolygonClip& clip, 
                            int ienter)
{
    bool ret = false;
    vector<Point2d> pxpoints;
    Point2d pt1, pt2;
    int si, ei, n, i;

    for (ei = ienter + 1; (ei - ienter) % count != 0; )
    {
        si = findVisibleEdge(clip, ei, ienter);
        ei = findInvisibleEdge(clip, si, ienter);
        n = ei - si + 1;
        if (n > 1)
        {
            pxpoints.resize(n);
            Point2d *pxs = &pxpoints.front();
            n = 0;
            for (i = si; i <= ei; i++)
            {
                pt2 = clip.getPoint(i);
                if (i == si || fabsf(pt1.x - pt2.x) > 2
                    || fabsf(pt1.y - pt2.y) > 2)
                {
                    pt1 = pt2;
                    pxs[n++] = pt1;
                }
            }

            ret = aux.draw(pxs, n) || ret;
        }
    }

    return ret;
}

bool GiGraphics::_drawPolygon(const GiContext* ctx, int count, const Point2d* points,
                              bool m2d, bool fill, bool edge, bool modelUnit)
{
    GiContext context (ctx ? *ctx : m_impl->ctx);
    if (!edge)
        context.setNullLine();
    if (!fill)
        context.setNoFillColor();
    
    if (context.isNullLine() && !context.hasFillColor())
        return false;

    vector<Point2d> pxpoints;
    Point2d pt1, pt2;
    Matrix2d matD(S2D(xf(), modelUnit));

    pxpoints.resize(count);
    Point2d *pxs = &pxpoints.front();
    int n = 0;
    for (int i = 0; i < count; i++) {
        pt2 = points[i];
        if (m2d)
            pt2 *= matD;
        if (i == 0 || count <= 4
            || fabsf(pt1.x - pt2.x) > 2
            || fabsf(pt1.y - pt2.y) > 2) {
            pt1 = pt2;
            pxs[n++] = pt1;
        }
    }

    if (n == 4 && mgEquals(pxs[0].x, pxs[3].x) && mgEquals(pxs[1].x, pxs[2].x)
        && mgEquals(pxs[0].y, pxs[1].y) && mgEquals(pxs[2].y, pxs[3].y))
    {
        Box2d rc(pxs[0].x, pxs[0].y, pxs[2].x, pxs[2].y, true);
        return rawRect(&context, rc.xmin, rc.ymin, rc.width(), rc.height());
    }

    return rawPolygon(&context, pxs, n);
}

bool GiGraphics::drawPolygon(const GiContext* ctx, int count, 
                             const Point2d* points, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || points == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    
    count = count > 0x2000 ? 0x2000 : count;
    ctx = ctx ? ctx : &(m_impl->ctx);

    bool ret = false;

    const Box2d extent (count, points);                 // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (DRAW_MAXR(m_impl, modelUnit).contains(extent))  // 全部在显示区域内
    {
        ret = _drawPolygon(ctx, count, points, true, true, true, modelUnit);
    }
    else                                                // 部分在显示区域内
    {
        PolygonClip clip (m_impl->rectDraw);
        if (!clip.clip(count, points, &S2D(xf(), modelUnit)))  // 多边形剪裁
            return false;
        count = clip.getCount();
        points = clip.getPoints();

        ret = _drawPolygon(ctx, count, points, false, true, false, modelUnit);

        int ienter = findInvisibleEdge(clip);
        if (ienter == count)
        {
            ret = _drawPolygon(ctx, count, points, false, false, true, modelUnit) || ret;
        }
        else
        {
            ret = drawPolygonEdge(PolylineAux(this, ctx), count, clip, ienter) || ret;
        }
    }

    return ret;
}

bool GiGraphics::drawEllipse(const GiContext* ctx, const Box2d& rect, bool modelUnit)
{
    return drawEllipse(ctx, rect.center(), rect.width() / 2, rect.height() / 2, modelUnit);
}

bool GiGraphics::drawEllipse(const GiContext* ctx, const Point2d& center, 
                             float rx, float ry, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    bool ret = false;
    Matrix2d matD(S2D(xf(), modelUnit));

    if (ry < _MGZERO) {
        ry = (Vector2d(rx, rx) * matD).x;
        ry = fabsf((Vector2d(ry, ry) * matD.inverse()).y);
    }

    const Box2d extent (center, rx*2.f, ry*2.f);            // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    if (mgIsZero(matD.m12) && mgIsZero(matD.m21))
    {
        Point2d cen (center * matD);
        rx *= fabsf(matD.m11);
        ry *= fabsf(matD.m22);

        ret = rawEllipse(ctx, cen.x - rx, cen.y - ry, 2 * rx, 2 * ry);
    }
    else
    {
        Point2d pxs[13];
        mgcurv::ellipseToBezier(pxs, center, rx, ry);
        matD.TransformPoints(13, pxs);

        ret = rawBeginPath();
        if (ret)
        {
            rawMoveTo(pxs[0].x, pxs[0].y);
            for (int i = 1; i + 2 < 13; i += 3) {
                rawBezierTo(pxs[i].x, pxs[i].y,
                    pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
            }
            rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawPie(const GiContext* ctx, 
                         const Point2d& center, float rx, float ry, 
                         float startAngle, float sweepAngle, 
                         bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rx < _MGZERO || fabsf(sweepAngle) < 1e-5f)
        return false;
    GiLock lock (&m_impl->drawRefcnt);

    if (ry < _MGZERO)
        ry = rx;

    const Box2d extent (center, rx*2.f, ry*2.f);            // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    Point2d pxs[16];
    int count = mgcurv::arcToBezier(pxs, center,
        rx, ry, startAngle, sweepAngle);
    if (count < 4)
        return false;
    S2D(xf(), modelUnit).TransformPoints(count, pxs);
    Point2d cen(center * S2D(xf(), modelUnit));

    bool ret = rawBeginPath();
    if (ret)
    {
        rawMoveTo(cen.x, cen.y);
        rawLineTo(pxs[0].x, pxs[0].y);
        for (int i = 1; i + 2 < count; i += 3) {
            rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        rawClosePath();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawRect(const GiContext* ctx, const Box2d& rect, 
                          bool modelUnit)
{
    if (rect.isEmpty() && ctx && m_impl->canvas) {
        rawRect(ctx, 0, 0, 0, 0);
        return false;
    }
    Point2d points[4] = {
        rect.leftBottom(), rect.rightBottom(), 
        rect.rightTop(), rect.leftTop()
    };
    return !rect.isEmpty() && drawPolygon(ctx, 4, points, modelUnit);
}

bool GiGraphics::drawRoundRect(const GiContext* ctx, 
                               const Box2d& rect, float rx, float ry, 
                               bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || rect.isEmpty())
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    bool ret = false;

    if (ry < _MGZERO)
        ry = rx;

    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(rect))  // 全部在显示区域外
        return false;

    if (rx < _MGZERO)
    {
        Point2d points[4] = {
            rect.leftBottom(), rect.rightBottom(), 
            rect.rightTop(), rect.leftTop()
        };
        return drawPolygon(ctx, 4, points);
    }
    else
    {
        Point2d pxs[16];

        mgcurv::roundRectToBeziers(pxs, rect, rx, ry);
        S2D(xf(), modelUnit).TransformPoints(16, pxs);

        ret = rawBeginPath();
        if (ret)
        {
            rawMoveTo(pxs[0].x, pxs[0].y);
            rawBezierTo(pxs[1].x, pxs[1].y, pxs[2].x, pxs[2].y, pxs[3].x, pxs[3].y);

            rawLineTo(pxs[4].x, pxs[4].y);
            rawBezierTo(pxs[5].x, pxs[5].y, pxs[6].x, pxs[6].y, pxs[7].x, pxs[7].y);

            rawLineTo(pxs[8].x, pxs[8].y);
            rawBezierTo(pxs[9].x, pxs[9].y, pxs[10].x, pxs[10].y, pxs[11].x, pxs[11].y);

            rawLineTo(pxs[12].x, pxs[12].y);
            rawBezierTo(pxs[13].x, pxs[13].y, pxs[14].x, pxs[14].y, pxs[15].x, pxs[15].y);

            rawClosePath();
            ret = rawEndPath(ctx, true);
        }
    }

    return ret;
}

bool GiGraphics::drawSplines(const GiContext* ctx, int count, 
                             const Point2d* knots, 
                             const Vector2d* knotvs, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 
        || knots == NULL || knotvs == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(1 + (0x2000 - 1) / 3));

    int i;
    Point2d pt;
    Vector2d vec;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + (count - 1) * 3);
    Point2d *pxs = &pxpoints.front();

    pt = knots[0] * matD;                       // 第一个Bezier段的起点
    vec = knotvs[0] * matD / 3.f;               // 第一个Bezier段的起始矢量
    *pxs++ = pt;                                // 产生Bezier段的起点
    for (i = 1; i < count; i++)                 // 计算每一个Bezier段
    {
        *pxs++ = (pt += vec);                   // 产生Bezier段的第二点
        pt = knots[i] * matD;                   // Bezier段的终点
        vec = knotvs[i] * matD / 3.f;           // Bezier段的终止矢量
        *pxs++ = pt - vec;                      // 产生Bezier段的第三点
        *pxs++ = pt;                            // 产生Bezier段的终点
    }

    // 绘图
    return rawBeziers(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedSplines(const GiContext* ctx, int count, 
                                   const Point2d* knots, 
                                   const Vector2d* knotvs, 
                                   bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 2 || 
        knots == NULL || knotvs == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    int i, j = 0;
    Point2d pt;
    Vector2d vec;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + count * 3);
    Point2d *pxs = &pxpoints.front();

    pt = knots[0] * matD;                       // 第一个Bezier段的起点
    vec = knotvs[0] * matD / 3.f;               // 第一个Bezier段的起始矢量
    pxs[j++] = pt;                              // 产生Bezier段的起点
    for (i = 1; i < count; i++)                 // 计算每一个Bezier段
    {
        pxs[j++] = (pt += vec);                 // 产生Bezier段的第二点
        pt = knots[i] * matD;                   // Bezier段的终点
        vec = knotvs[i] * matD / 3.f;           // Bezier段的终止矢量
        pxs[j++] = pt - vec;                    // 产生Bezier段的第三点
        pxs[j++] = pt;                          // 产生Bezier段的终点
    }
    pxs[j++] = (pt += vec);                     // 产生Bezier段的第二点
    pxs[j]   = 2 * pxs[0] - pxs[1].asVector();  // 产生Bezier段的第三点
    pxs[j+1] = pxs[0];                          // 产生Bezier段的终点

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        rawMoveTo(pxs[0].x, pxs[0].y);
        for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
            rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        rawClosePath();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawBSplines(const GiContext* ctx, int count, 
                              const Point2d* ctlpts, bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 4 || ctlpts == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>(3 + (0x2000 - 1) / 3));

    const Box2d extent (count, ctlpts);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + (count - 3) * 3);
    Point2d *pxs = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = ctlpts[0] * matD;
    pt2 = ctlpts[1] * matD;
    pt3 = ctlpts[2] * matD;
    pt4 = ctlpts[3 % count] * matD;
    (*pxs++).set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6,  (4 * pt2.y + 2 * pt3.y)   *d6);
    (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6,  (2 * pt2.y + 4 * pt3.y)   *d6);
    (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);

    // 计算其余曲线段
    for (i = 4; i < count; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[i % count] * matD;
        (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)   *d6);
        (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)   *d6);
        (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6,(pt2.y + 4 * pt3.y + pt4.y)*d6);
    }

    // 绘图
    return rawBeziers(ctx, &pxpoints.front(), getSize(pxpoints));
}

bool GiGraphics::drawClosedBSplines(const GiContext* ctx, 
                                    int count, 
                                    const Point2d* ctlpts, 
                                    bool modelUnit)
{
    if (m_impl->drawRefcnt == 0 || count < 3 || ctlpts == NULL)
        return false;
    GiLock lock (&m_impl->drawRefcnt);
    count = mgMin(count, static_cast<int>((0x2000 - 1) / 3));

    const Box2d extent (count, ctlpts);              // 模型坐标范围
    if (!DRAW_RECT(m_impl, modelUnit).isIntersect(extent))  // 全部在显示区域外
        return false;

    int i;
    Point2d pt1, pt2, pt3, pt4;
    float d6 = 1.f / 6.f;
    vector<Point2d> pxpoints;
    Matrix2d matD(S2D(xf(), modelUnit));

    // 开辟像素坐标数组
    pxpoints.resize(1 + count * 3);
    Point2d *pxs = &pxpoints.front();

    // 计算第一个曲线段
    pt1 = ctlpts[0] * matD;
    pt2 = ctlpts[1] * matD;
    pt3 = ctlpts[2] * matD;
    pt4 = ctlpts[3 % count] * matD;
    (*pxs++).set((pt1.x + 4 * pt2.x + pt3.x)*d6, (pt1.y + 4 * pt2.y + pt3.y)*d6);
    (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
    (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
    (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);

    // 计算其余曲线段
    for (i = 4; i < count + 3; i++)
    {
        pt1 = pt2;
        pt2 = pt3;
        pt3 = pt4;
        pt4 = ctlpts[i % count] * matD;
        (*pxs++).set((4 * pt2.x + 2 * pt3.x)    *d6, (4 * pt2.y + 2 * pt3.y)    *d6);
        (*pxs++).set((2 * pt2.x + 4 * pt3.x)    *d6, (2 * pt2.y + 4 * pt3.y)    *d6);
        (*pxs++).set((pt2.x + 4 * pt3.x + pt4.x)*d6, (pt2.y + 4 * pt3.y + pt4.y)*d6);
    }

    // 绘图
    bool ret = rawBeginPath();
    if (ret)
    {
        pxs = &pxpoints.front();
        rawMoveTo(pxs[0].x, pxs[0].y);
        for (i = 1; i + 2 < getSize(pxpoints); i += 3) {
            rawBezierTo(pxs[i].x, pxs[i].y,
                pxs[i+1].x, pxs[i+1].y, pxs[i+2].x, pxs[i+2].y);
        }
        rawClosePath();
        ret = rawEndPath(ctx, true);
    }

    return ret;
}

bool GiGraphics::drawPath(const GiContext* ctx, const GiPath& path, 
                          bool fill, bool modelUnit)
{
    int n = path.getCount();
    if (n == 0)
        return false;

    Matrix2d matD(S2D(xf(), modelUnit));
    const Point2d* pts = path.getPoints();
    const char* types = path.getTypes();
    Point2d a, b, c;

    rawBeginPath();

    for (int i = 0; i < n; i++) {
        switch (types[i] & ~kGiCloseFigure) {
        case kGiMoveTo:
            a = pts[i] * matD;
            rawMoveTo(a.x, a.y);
            break;

        case kGiLineTo:
            a = pts[i] * matD;
            rawLineTo(a.x, a.y);
            break;

        case kGiBeziersTo:
            if (i + 2 >= n)
                return false;
            a = pts[i] * matD;
            b = pts[i+1] * matD;
            c = pts[i+2] * matD;
            rawBezierTo(a.x, a.y, b.x, b.y, c.x, c.y);
            i += 2;
            break;

        default:
            return false;
        }
        if (types[i] & kGiCloseFigure)
            rawClosePath();
    }

    return rawEndPath(ctx, fill);
}

bool GiGraphics::setPen(const GiContext* ctx)
{
    bool changed = !(m_impl->ctxused & 1);
    
    if (m_impl->canvas) {
        if (ctx && (!mgEquals(ctx->getLineWidth(), m_impl->ctx.getLineWidth())
                    || ctx->isAutoScale() != m_impl->ctx.isAutoScale())) {
            m_impl->ctx.setLineWidth(ctx->getLineWidth(), ctx->isAutoScale());
            changed = true;
        }
        if (ctx && ctx->getLineColor() != m_impl->ctx.getLineColor()) {
            m_impl->ctx.setLineColor(ctx->getLineColor());
            changed = true;
        }
        if (ctx && ctx->getLineStyle() != m_impl->ctx.getLineStyle()) {
            m_impl->ctx.setLineStyle(ctx->getLineStyle());
            changed = true;
        }
    }
    
    ctx = &(m_impl->ctx);
    if (m_impl->canvas && changed) {
        m_impl->ctxused &= 1;
        m_impl->canvas->setPen(calcPenColor(ctx->getLineColor()).getARGB(),
                               calcPenWidth(ctx->getLineWidth(), ctx->isAutoScale()),
                               ctx->getLineStyle(), 0);
    }
    
    return !ctx->isNullLine();
}

bool GiGraphics::setBrush(const GiContext* ctx)
{
    bool changed = !(m_impl->ctxused & 2);
    
    if (m_impl->canvas) {
        if (ctx && ctx->getFillColor() != m_impl->ctx.getFillColor()) {
            m_impl->ctx.setFillColor(ctx->getFillColor());
            changed = true;
        }
    }
    
    ctx = &(m_impl->ctx);
    if (m_impl->canvas && changed) {
        m_impl->ctxused &= 2;
        m_impl->canvas->setBrush(calcPenColor(ctx->getFillColor()).getARGB(), 0);
    }
    
    return ctx->hasFillColor();
}

bool GiGraphics::rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2)
{
    if (m_impl->canvas && setPen(ctx)) {
        m_impl->canvas->drawLine(x1, y1, x2, y2);
        return true;
    }
    return false;
}

bool GiGraphics::rawLines(const GiContext* ctx, const Point2d* pxs, int count)
{
    if (m_impl->canvas && setPen(ctx) && pxs && count > 0) {
        m_impl->canvas->beginPath();
        m_impl->canvas->moveTo(pxs[0].x, pxs[0].y);
        for (int i = 1; i < count; i++) {
            m_impl->canvas->lineTo(pxs[i].x, pxs[i].y);
        }
        m_impl->canvas->drawPath(true, false);
        return true;
    }
    return false;
}

bool GiGraphics::rawBeziers(const GiContext* ctx, const Point2d* pxs, int count)
{
    if (m_impl->canvas && setPen(ctx) && pxs && count > 0) {
        m_impl->canvas->beginPath();
        m_impl->canvas->moveTo(pxs[0].x, pxs[0].y);
        for (int i = 1; i + 2 < count; i += 3) {
            m_impl->canvas->bezierTo(pxs[i].x, pxs[i].y, pxs[i+1].x, pxs[i+1].y,
                                     pxs[i+2].x, pxs[i+2].y);
        }
        m_impl->canvas->drawPath(true, false);
        return true;
    }
    return false;
}

bool GiGraphics::rawPolygon(const GiContext* ctx, const Point2d* pxs, int count)
{
    bool usePen = setPen(ctx);
    bool useBrush = setBrush(ctx);
    
    if (m_impl->canvas && (usePen || useBrush) && pxs && count > 0) {
        m_impl->canvas->beginPath();
        m_impl->canvas->moveTo(pxs[0].x, pxs[0].y);
        for (int i = 1; i < count; i++) {
            m_impl->canvas->lineTo(pxs[i].x, pxs[i].y);
        }
        m_impl->canvas->closePath();
        m_impl->canvas->drawPath(usePen, useBrush);
        return true;
    }
    return false;
}

bool GiGraphics::rawRect(const GiContext* ctx, float x, float y, float w, float h)
{
    bool usePen = setPen(ctx);
    bool useBrush = setBrush(ctx);
    
    if (m_impl->canvas && (usePen || useBrush)) {
        m_impl->canvas->drawRect(x, y, w, h, usePen, useBrush);
        return true;
    }
    return false;
}

bool GiGraphics::rawEllipse(const GiContext* ctx, float x, float y, float w, float h)
{
    bool usePen = setPen(ctx);
    bool useBrush = setBrush(ctx);
    
    if (m_impl->canvas && (usePen || useBrush)) {
        m_impl->canvas->drawEllipse(x, y, w, h, usePen, useBrush);
        return true;
    }
    return false;
}

bool GiGraphics::rawBeginPath()
{
    if (m_impl->canvas) {
        m_impl->canvas->beginPath();
    }
    return !!m_impl->canvas;
}

bool GiGraphics::rawEndPath(const GiContext* ctx, bool fill)
{
    bool usePen = setPen(ctx);
    bool useBrush = fill && setBrush(ctx);
    
    if (m_impl->canvas && (usePen || useBrush)) {
        m_impl->canvas->drawPath(usePen, useBrush);
        return true;
    }
    return false;
}

bool GiGraphics::rawMoveTo(float x, float y)
{
    if (m_impl->canvas) {
        m_impl->canvas->moveTo(x, y);
    }
    return !!m_impl->canvas;
}

bool GiGraphics::rawLineTo(float x, float y)
{
    if (m_impl->canvas) {
        m_impl->canvas->lineTo(x, y);
    }
    return !!m_impl->canvas;
}

bool GiGraphics::rawBezierTo(float c1x, float c1y, float c2x, 
                             float c2y, float x, float y)
{
    if (m_impl->canvas) {
        m_impl->canvas->bezierTo(c1x, c1y, c2x, c2y, x, y);
    }
    return !!m_impl->canvas;
}

bool GiGraphics::rawClosePath()
{
    if (m_impl->canvas) {
        m_impl->canvas->closePath();
    }
    return !!m_impl->canvas;
}

bool GiGraphics::rawText(const char* text, float x, float y, float h, int align)
{
    if (m_impl->canvas && text) {
        m_impl->canvas->drawTextAt(text, x, y, h, align);
        return true;
    }
    return false;
}

bool GiGraphics::rawImage(const char* name, float xc, float yc, 
                          float w, float h, float angle)
{
    if (m_impl->canvas && name) {
        m_impl->canvas->drawBitmap(name, xc, yc, w, h, angle);
        return true;
    }
    return false;
}

bool GiGraphics::drawHandle(const Point2d& pnt, int type, bool modelUnit)
{
    if (m_impl->canvas && type >= 0) {
        Point2d ptd(pnt * S2D(xf(), modelUnit));
        m_impl->canvas->drawHandle(ptd.x, ptd.y, type);
        return true;
    }
    return false;
}
