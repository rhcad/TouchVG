// gixform.cpp: 实现坐标系管理类 GiTransform
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "gixform.h"
#include "gilock.h"

//! GiTransform的内部数据
struct GiTransformImpl
{
    int         cxWnd;          //!< 显示窗口宽度，像素
    int         cyWnd;          //!< 显示窗口高度，像素
    float       dpiX;           //!< 显示设备每英寸的像素数X
    float       dpiY;           //!< 显示设备每英寸的像素数Y
    bool        ydown;          //!< 显示设备的+Y方向是否为向下
    Point2d     centerW;        //!< 显示窗口中心的世界坐标，默认(0,0)
    float       viewScale;      //!< 显示比例，默认100%
    Matrix2d    matM2W;         //!< 模型坐标系到世界坐标系的变换矩阵，默认单位矩阵

    Matrix2d    matW2M;         //!< 世界坐标系到模型坐标系的变换矩阵
    Matrix2d    matD2W;         //!< 显示坐标系到世界坐标系的变换矩阵
    Matrix2d    matW2D;         //!< 世界坐标系到显示坐标系的变换矩阵
    Matrix2d    matD2M;         //!< 显示坐标系到模型坐标系的变换矩阵
    Matrix2d    matM2D;         //!< 模型坐标系到显示坐标系的变换矩阵
    float       w2dx;           //!< 世界单位对应的像素数X
    float       w2dy;           //!< 世界单位对应的像素数Y

    bool        zoomEnabled;    //!< 是否允许放缩
    Point2d     tmpCenterW;     //!< 当前放缩结果，不论是否允许放缩
    float       tmpViewScale;   //!< 当前放缩结果，不论是否允许放缩
    volatile long   zoomTimes;  //!< 放缩结果改变的次数

    float       minViewScale;   //!< 最小显示比例
    float       maxViewScale;   //!< 最大显示比例
    Box2d       rectLimitsW;    //!< 显示极限的世界坐标范围

    GiTransformImpl(bool _ydown)
        : cxWnd(1), cyWnd(1), dpiX(96), dpiY(96), ydown(_ydown), viewScale(1)
        , zoomEnabled(true), tmpViewScale(1.f), zoomTimes(0)
    {
        minViewScale = 0.01f;   // 最小显示比例为1%
        maxViewScale = 5.f;     // 最大显示比例为500%
        rectLimitsW.set(Point2d::kOrigin(), 2e5f, 2e5f);

        updateTransforms();
    }

    void updateTransforms()
    {
        w2dx = viewScale * dpiX / 25.4f;
        w2dy = viewScale * dpiY / 25.4f;

        float wdy = ydown ? -w2dy : w2dy;
        float xc = cxWnd * 0.5f;
        float yc = cyWnd * 0.5f;

        matD2W.set(1.f / w2dx, 0, 0, 1.f / wdy,
            centerW.x - xc / w2dx, centerW.y - yc / wdy);
        matW2D.set(w2dx, 0, 0, wdy,
            xc - w2dx * centerW.x, yc - wdy * centerW.y);

        matD2M = matD2W * matW2M;
        matM2D = matM2W * matW2D;
    }

    void copyFrom(const GiTransformImpl* src)
    {
        cxWnd  = src->cxWnd;
        cyWnd  = src->cyWnd;
        dpiX = src->dpiX;
        dpiY = src->dpiY;
        ydown = src->ydown;
        centerW = src->centerW;
        viewScale = src->viewScale;
        matM2W = src->matM2W;
        matW2M = src->matW2M;
        matD2W = src->matD2W;
        matW2D = src->matW2D;
        matD2M = src->matD2M;
        matM2D = src->matM2D;
        w2dx = src->w2dx;
        w2dy = src->w2dy;
        minViewScale = src->minViewScale;
        maxViewScale = src->maxViewScale;
        rectLimitsW = src->rectLimitsW;
        tmpCenterW = src->tmpCenterW;
        tmpViewScale = src->tmpViewScale;
        zoomTimes = src->zoomTimes;
    }

    void zoomChanged()
    {
        giAtomicIncrement(&zoomTimes);
    }

    bool zoomNoAdjust(const Point2d& pnt, float scale, bool* changed = (bool*)0)
    {
        bool bChanged = false;

        if (pnt != centerW || !mgEquals(scale, viewScale))
        {
            tmpCenterW = pnt;
            tmpViewScale = scale;
            bChanged = true;
            if (zoomEnabled) {
                centerW = pnt;
                viewScale = scale;
                updateTransforms();
                zoomChanged();
            }
        }
        if (changed)
            *changed = bChanged;

        return bChanged;
    }

    bool zoomPanAdjust(Point2d &ptW, float dxPixel, float dyPixel) const;
};

GiTransform::GiTransform(bool ydown)
{
    m_impl = new GiTransformImpl(ydown);
}

GiTransform::GiTransform(const GiTransform& src)
{
    m_impl = new GiTransformImpl(true);
    m_impl->copyFrom(src.m_impl);
}

GiTransform::~GiTransform()
{
    delete m_impl;
}

GiTransform& GiTransform::copy(const GiTransform& src)
{
    if (this != &src)
        m_impl->copyFrom(src.m_impl);
    return *this;
}

float GiTransform::getDpiX() const { return m_impl->dpiX; }
float GiTransform::getDpiY() const { return m_impl->dpiY; }
int GiTransform::getWidth() const { return m_impl->cxWnd; }
int GiTransform::getHeight() const { return m_impl->cyWnd; }
Point2d GiTransform::getCenterW() const { return m_impl->centerW; }
float GiTransform::getViewScale() const { return m_impl->viewScale; }
float GiTransform::getWorldToDisplayX(bool useViewScale) const {
    return fabsf(useViewScale ? m_impl->w2dx : m_impl->w2dx / m_impl->viewScale); }
float GiTransform::getWorldToDisplayY(bool useViewScale) const {
    return fabsf(useViewScale ? m_impl->w2dy : m_impl->w2dy / m_impl->viewScale); }
const Matrix2d& GiTransform::modelToWorld() const {
    return m_impl->matM2W; }
const Matrix2d& GiTransform::worldToModel() const {
    return m_impl->matW2M; }
const Matrix2d& GiTransform::displayToWorld() const {
    return m_impl->matD2W; }
const Matrix2d& GiTransform::worldToDisplay() const {
    return m_impl->matW2D; }
const Matrix2d& GiTransform::displayToModel() const {
    return m_impl->matD2M; }
const Matrix2d& GiTransform::modelToDisplay() const {
    return m_impl->matM2D; }
float GiTransform::getMinViewScale() const {
    return m_impl->minViewScale; }
float GiTransform::getMaxViewScale() const {
    return m_impl->maxViewScale; }
Box2d GiTransform::getWorldLimits() const {
    return m_impl->rectLimitsW; }

float GiTransform::displayToModel(float px, bool mm) const
{
    return mm ? (Vector2d(px,px) * m_impl->matW2M).length() * _M_SQRT1_2 / m_impl->viewScale
        : (Vector2d(px,px) * m_impl->matD2M).length() * _M_SQRT1_2;
}

long GiTransform::getZoomTimes() const
{
    return m_impl->zoomTimes;
}

bool GiTransform::setWndSize(int width, int height)
{
    if ((m_impl->cxWnd != width || m_impl->cyWnd != height)
        && width > 1 && height > 1)
    {
        m_impl->cxWnd = width;
        m_impl->cyWnd = height;
        m_impl->updateTransforms();
        m_impl->zoomChanged();
        return true;
    }
    return false;
}

bool GiTransform::setModelTransform(const Matrix2d& mat)
{
    bool changed = mat.isInvertible() && m_impl->matM2W != mat;
    if (changed) {
        m_impl->matM2W = mat;
        m_impl->matW2M = m_impl->matM2W.inverse();
        m_impl->matD2M = m_impl->matD2W * m_impl->matW2M;
        m_impl->matM2D = m_impl->matM2W * m_impl->matW2D;
        m_impl->zoomChanged();
    }
    return changed;
}

void GiTransform::setResolution(float dpiX, float dpiY)
{
    if (dpiY < 0.1f)
        dpiY = dpiX;
    if (dpiX > 0.1f && dpiY > 0.1f 
        && (m_impl->dpiX != dpiX || m_impl->dpiY != dpiY))
    {
        m_impl->dpiX = dpiX;
        m_impl->dpiY = dpiY;
        m_impl->updateTransforms();
        m_impl->zoomChanged();
    }
}

Box2d GiTransform::getWndRectW() const
{
    return Box2d((float)getWidth(), (float)getHeight()) * displayToWorld();
}

Box2d GiTransform::getWndRectM() const
{
    return Box2d((float)getWidth(), (float)getHeight()) * displayToModel();
}

Box2d GiTransform::getWndRect() const
{
    return Box2d((float)getWidth(), (float)getHeight());
}

void GiTransform::setViewScaleRange(float minScale, float maxScale)
{
    if (minScale > maxScale)
        mgSwap(minScale, maxScale);

    minScale = mgMax(minScale, 1e-5f);
    minScale = mgMin(minScale, 0.5f);

    maxScale = mgMax(maxScale, 1.f);
    maxScale = mgMin(maxScale, 50.f);

    m_impl->minViewScale = minScale;
    m_impl->maxViewScale = maxScale;
}

Box2d GiTransform::setWorldLimits(const Box2d& rect)
{
    Box2d ret = m_impl->rectLimitsW;
    m_impl->rectLimitsW = rect.isEmpty() ? Box2d(Point2d::kOrigin(), 2e5f, 2e5f) : rect;
    m_impl->rectLimitsW.normalize();
    return ret;
}

bool GiTransform::enableZoom(bool enabled)
{
    bool bOld = m_impl->zoomEnabled;
    m_impl->zoomEnabled = enabled;
    return bOld;
}

float GiTransform::getZoomValue(Point2d& centerW) const
{
    centerW = m_impl->tmpCenterW;
    return m_impl->tmpViewScale;
}

bool GiTransform::zoom(Point2d centerW, float viewScale)
{
    bool changed = false;

    viewScale = mgMax(viewScale, m_impl->minViewScale);
    viewScale = mgMin(viewScale, m_impl->maxViewScale);

    Box2d rectW(m_impl->rectLimitsW);
    rectW.inflate(2);
    float halfw = m_impl->cxWnd / m_impl->w2dx * 0.5f;
    float halfh = m_impl->cyWnd / m_impl->w2dy * 0.5f;

    if (centerW.x - halfw < rectW.xmin)
        centerW.x += rectW.xmin - (centerW.x - halfw);
    if (centerW.x + halfw > rectW.xmax)
        centerW.x += rectW.xmax - (centerW.x + halfw);
    if (2 * halfw >= rectW.width())
        centerW.x = rectW.center().x;

    if (centerW.y - halfh < rectW.ymin)
        centerW.y += rectW.ymin - (centerW.y - halfh);
    if (centerW.y + halfh > rectW.ymax)
        centerW.y += rectW.ymax - (centerW.y + halfh);
    if (2 * halfh >= rectW.height())
        centerW.y = rectW.center().y;

    // 如果显示比例很小使得窗口超界，就放大显示
    if (2 * halfw > rectW.width() && 2 * halfh > rectW.height()) {
        viewScale *= mgMin(2 * halfw / rectW.width(),
            2 * halfh / rectW.height());
        if (viewScale > m_impl->maxViewScale)
            viewScale = m_impl->maxViewScale;
    }

    m_impl->zoomNoAdjust(centerW, viewScale, &changed);

    return changed;
}

static inline bool ScaleOutRange(float scale, const GiTransformImpl* impl)
{
    return scale < _MGZERO
        || scale < impl->minViewScale - _MGZERO
        || scale > impl->maxViewScale + _MGZERO;
}

static void AdjustCenterW(Point2d &ptW, float halfw, float halfh, 
                          const Box2d& rectW)
{
    if (ptW.x - halfw < rectW.xmin)
        ptW.x += rectW.xmin - (ptW.x - halfw);
    if (ptW.x + halfw > rectW.xmax)
        ptW.x += rectW.xmax - (ptW.x + halfw);
    if (2 * halfw >= rectW.width())
        ptW.x = rectW.center().x;

    if (ptW.y - halfh < rectW.ymin)
        ptW.y += rectW.ymin - (ptW.y - halfh);
    if (ptW.y + halfh > rectW.ymax)
        ptW.y += rectW.ymax - (ptW.y + halfh);
    if (2 * halfh >= rectW.height())
        ptW.y = rectW.center().y;
}

static bool AdjustCenterIn(bool adjust, const Box2d& box, const Box2d& limitsW, 
                           Point2d &ptW, float halfw, float halfh)
{
    Box2d rectW(limitsW);
    rectW.inflate(2);

    if (!limitsW.isEmpty() && !rectW.contains(box)) {
        if (adjust) {
            AdjustCenterW(ptW, halfw, halfh, rectW);
        }
        else {
            return false;
        }
    }

    return true;
}

bool GiTransform::zoomWnd(const Point2d& pt1, const Point2d& pt2, bool adjust)
{
    // 计算开窗矩形的中心和宽高
    Point2d ptCen ((pt2.x + pt1.x) * 0.5f, (pt2.y + pt1.y) * 0.5f);
    float w = fabsf(static_cast<float>(pt2.x - pt1.x));
    float h = fabsf(static_cast<float>(pt2.y - pt1.y));
    if (w < 4 || h < 4)
        return false;

    // 中心不变，扩大开窗矩形使得宽高比例和显示窗口相同
    if (h * m_impl->cxWnd < w * m_impl->cyWnd) {
        h = w * m_impl->cyWnd / m_impl->cxWnd;
    }

    // 计算放缩前矩形中心的世界坐标
    Point2d ptW (ptCen * m_impl->matD2W);

    // 计算新显示比例
    float scale = m_impl->viewScale * m_impl->cyWnd / h;
    if (!adjust && ScaleOutRange(scale, m_impl))
        return false;
    scale = mgMax(scale, m_impl->minViewScale);
    scale = mgMin(scale, m_impl->maxViewScale);

    // 计算新显示比例下的显示窗口的世界坐标范围
    float halfw = m_impl->cxWnd / (m_impl->w2dx / m_impl->viewScale * scale) * 0.5f;
    float halfh = m_impl->cyWnd / (m_impl->w2dy / m_impl->viewScale * scale) * 0.5f;
    Box2d box (ptW, 2 * halfw, 2 * halfh);

    // 检查显示窗口的新坐标范围是否在极限范围内
    if (!AdjustCenterIn(adjust, box, m_impl->rectLimitsW, ptW, halfw, halfh)) {
        return false;
    }

    // 改变显示比例和位置
    return m_impl->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Box2d& rectWorld, const RECT_2D* rcTo, bool adjust)
{
    // 如果图形范围的宽或高接近于零，就返回
    if (rectWorld.isEmpty())
        return false;

    // 计算像素到毫米的比例
    const float d2mmX = m_impl->viewScale / m_impl->w2dx;
    const float d2mmY = m_impl->viewScale / m_impl->w2dy;

    // 计算目标窗口区域(毫米)
    float w = 0, h = 0;
    Point2d ptCen;

    if (rcTo) {
        w = fabsf(static_cast<float>(rcTo->right - rcTo->left));
        h = fabsf(static_cast<float>(rcTo->bottom - rcTo->top));
        ptCen.x = (rcTo->left + rcTo->right) * 0.5f;
        ptCen.y = (rcTo->top + rcTo->bottom) * 0.5f;
    }
    if (w < 4 || h < 4) {
        w = (float)m_impl->cxWnd;
        h = (float)m_impl->cyWnd;
        ptCen.set(w * 0.5f, h * 0.5f);
        w -= 8;
        h -= 8;
    }
    if (w < 4 || h < 4)
        return false;
    w *= d2mmX;
    h *= d2mmY;
    ptCen.scaleBy(d2mmX, d2mmY);

    // 计算新显示比例 (中心不变，缩小窗口区域使得宽高比例和图形范围相同)
    float scale;
    if (h * rectWorld.width() > w * rectWorld.height()) {
        //h = w * rectWorld.height() / rectWorld.width();
        scale = w / rectWorld.width();
    }
    else {
        //w = h * rectWorld.width() / rectWorld.height();
        scale = h / rectWorld.height();
    }

    // 检查显示比例
    if (!adjust && ScaleOutRange(scale, m_impl))
        return false;
    scale = mgMax(scale, m_impl->minViewScale);
    scale = mgMin(scale, m_impl->maxViewScale);

    // 计算在新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    ptW.x = rectWorld.center().x + (m_impl->cxWnd * d2mmX * 0.5f - ptCen.x) / scale;
    ptW.y = rectWorld.center().y - (m_impl->cyWnd * d2mmY * 0.5f - ptCen.y) / scale;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    float halfw = m_impl->cxWnd * d2mmX  / scale * 0.5f;
    float halfh = m_impl->cyWnd * d2mmY  / scale * 0.5f;
    Box2d box (ptW, 2 * halfw, 2 * halfh);

    if (!AdjustCenterIn(adjust, box, m_impl->rectLimitsW, ptW, halfw, halfh)) {
        return false;
    }

    return m_impl->zoomNoAdjust(ptW, scale);
}

bool GiTransform::zoomTo(const Point2d& pntWorld, const Point2d* pxAt, bool adjust)
{
    Point2d pnt = pntWorld * m_impl->matW2D;
    return zoomPan(
        (!pxAt ? (m_impl->cxWnd * 0.5f) : pxAt->x) - pnt.x,
        (!pxAt ? (m_impl->cyWnd * 0.5f) : pxAt->y) - pnt.y, adjust);
}

bool GiTransform::zoomPan(float dxPixel, float dyPixel, bool adjust)
{
    // 计算新的显示窗口中心的世界坐标
    Vector2d vec (dxPixel, dyPixel);
    Point2d ptW (m_impl->centerW - vec * m_impl->matD2W);

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    if (m_impl->zoomPanAdjust(ptW, dxPixel, dyPixel) && !adjust)
        return false;
    if (ptW == m_impl->centerW)
        return false;

    return m_impl->zoomNoAdjust(ptW, m_impl->viewScale);
}

bool GiTransformImpl::zoomPanAdjust(Point2d &ptW, float dxPixel, float dyPixel) const
{
    bool bAdjusted = false;
    float halfw = cxWnd / w2dx * 0.5f;
    float halfh = cyWnd / w2dy * 0.5f;

    Box2d rectW(rectLimitsW);
    rectW.inflate(2);

    if (dxPixel > 0 && ptW.x - halfw < rectW.xmin) {
        bAdjusted = true;
        ptW.x += rectW.xmin - (ptW.x - halfw);
    }
    if (dxPixel < 0 && ptW.x + halfw > rectW.xmax) {
        bAdjusted = true;
        ptW.x += rectW.xmax - (ptW.x + halfw);
    }
    if (fabsf(dxPixel) > 0 && 2 * halfw >= rectW.width()) {
        bAdjusted = true;
        ptW.x = rectW.center().x;
    }
    if (dyPixel < 0 && ptW.y - halfh < rectW.ymin) {
        bAdjusted = true;
        ptW.y += rectW.ymin - (ptW.y - halfh);
    }
    if (dyPixel > 0 && ptW.y + halfh > rectW.ymax) {
        bAdjusted = true;
        ptW.y += rectW.ymax - (ptW.y + halfh);
    }
    if (fabsf(dyPixel) > 0 && 2 * halfh >= rectW.height()) {
        bAdjusted = true;
        ptW.y = rectLimitsW.center().y;
    }

    return bAdjusted;
}

bool GiTransform::zoomByFactor(float factor, const Point2d* pxAt, bool adjust)
{
    float scale = m_impl->viewScale;
    if (factor > 0)
        scale *= (1.f + fabsf(factor));
    else
        scale /= (1.f + fabsf(factor));

    if (adjust) {
        scale = mgMax(scale, m_impl->minViewScale);
        scale = mgMin(scale, m_impl->maxViewScale);
    }
    if (mgEquals(scale, m_impl->viewScale))
        return false;
    return zoomScale(scale, pxAt, adjust);
}

bool GiTransform::zoomScale(float viewScale, const Point2d* pxAt, bool adjust)
{
    // 检查显示比例
    if (!adjust && ScaleOutRange(viewScale, m_impl))
        return false;
    viewScale = mgMax(viewScale, m_impl->minViewScale);
    viewScale = mgMin(viewScale, m_impl->maxViewScale);

    // 得到放缩中心点的客户区坐标
    Point2d ptAt (m_impl->cxWnd * 0.5f,  m_impl->cyWnd * 0.5f);
    if (pxAt)
        ptAt.set(pxAt->x, pxAt->y);

    // 得到放缩中心点在放缩前的世界坐标
    Point2d ptAtW (ptAt * m_impl->matD2W);

    // 计算新显示比例下显示窗口中心的世界坐标
    Point2d ptW;
    float w2dx = m_impl->w2dx / m_impl->viewScale * viewScale;
    float w2dy = m_impl->w2dy / m_impl->viewScale * viewScale;
    ptW.x = ptAtW.x + (m_impl->cxWnd * 0.5f - ptAt.x) / w2dx;
    ptW.y = ptAtW.y - (m_impl->cyWnd * 0.5f - ptAt.y) / w2dy;

    // 检查新显示比例下显示窗口的世界坐标范围是否在极限范围内
    float halfw = m_impl->cxWnd / w2dx * 0.5f;
    float halfh = m_impl->cyWnd / w2dy * 0.5f;
    Box2d box (ptW, 2 * halfw, 2 * halfh);

    if (!AdjustCenterIn(adjust, box, m_impl->rectLimitsW, ptW, halfw, halfh)) {
        return false;
    }
    if (halfw - 3 > m_impl->rectLimitsW.width() / 2
        && halfh - 3 > m_impl->rectLimitsW.height() / 2)    // 显示比例太小
    {
        viewScale *= mgMin(2 * (halfw - 3) / m_impl->rectLimitsW.width(),
            2 * (halfh - 3) / m_impl->rectLimitsW.height());
        viewScale = mgMin(viewScale, m_impl->maxViewScale);
    }

    return m_impl->zoomNoAdjust(ptW, viewScale);
}
