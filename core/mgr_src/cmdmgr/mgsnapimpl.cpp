// mgsnapimpl.cpp: 实现命令管理器类
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgcmdmgr_.h"
#include <mggrid.h>

class SnapItem {
public:
    Point2d pt;             // 捕捉到的坐标
    Point2d base;           // 参考线基准点、原始点
    float   dist;           // 捕捉距离
    int     type;           // 特征点类型
    int     shapeid;        // 捕捉到的图形
    int     handleIndex;    // 捕捉到图形上的控制点序号
    int     handleIndexSrc; // 待确定位置的源图形上的控制点序号，与handleIndex点匹配
    
    SnapItem() {}
    SnapItem(const Point2d& _pt, const Point2d& _base, float _dist, int _type = 0,
        int _shapeid = 0, int _handleIndex = -1, int _handleIndexSrc = -1)
        : pt(_pt), base(_base), dist(_dist), type(_type), shapeid(_shapeid)
        , handleIndex(_handleIndex), handleIndexSrc(_handleIndexSrc) {}
};

static int snapHV(const Point2d& basePt, Point2d& newPt, SnapItem arr[3])
{
    int ret = 0;
    float diff;
    
    diff = arr[1].dist - fabsf(newPt.x - basePt.x);
    if (diff > _MGZERO || (diff > - _MGZERO
                           && fabsf(newPt.y - basePt.y) < fabsf(newPt.y - arr[1].base.y))) {
        arr[1].dist = fabsf(newPt.x - basePt.x);
        arr[1].base = basePt;
        newPt.x = basePt.x;
        arr[1].pt = newPt;
        arr[1].type = kMgSnapSameX;
        ret |= 1;
    }
    diff = arr[2].dist - fabsf(newPt.y - basePt.y);
    if (diff > _MGZERO || (diff > - _MGZERO
                     && fabsf(newPt.x - basePt.x) < fabsf(newPt.x - arr[2].base.x))) {
        arr[2].dist = fabsf(newPt.y - basePt.y);
        arr[2].base = basePt;
        newPt.y = basePt.y;
        arr[2].pt = newPt;
        arr[2].type = kMgSnapSameY;
        ret |= 2;
    }
    
    return ret;
}

static bool skipShape(const int* ignoreids, const MgShape* sp)
{
    bool skip = false;
    for (int t = 0; ignoreids[t] != 0 && !skip; t++) {
        skip = (ignoreids[t] == sp->getID());           // 跳过当前图形
    }
    return skip;
}

static bool snapHandle(const MgMotion*, const Point2d& orignPt,
                       const MgShape* shape, int ignoreHandle,
                       const MgShape* sp, SnapItem& arr0, Point2d* matchpt)
{
    int n = sp->shapec()->getHandleCount();
    bool curve = sp->shapec()->isKindOf(MgSplines::Type());
    bool dragHandle = (!shape || shape->getID() == 0 ||
                       orignPt == shape->shapec()->getHandlePoint(ignoreHandle));
    bool handleFound = false;
    
    for (int i = 0; i < n; i++) {                    // 循环每一个控制点
        if (curve && ((i > 0 && i + 1 < n) || sp->shapec()->isClosed())) {
            continue;                                   // 对于开放曲线只捕捉端点
        }
        Point2d pnt(sp->shapec()->getHandlePoint(i));   // 已有图形的一个顶点
        int handleType = sp->shapec()->getHandleType(i);
        
        float dist = pnt.distanceTo(orignPt);           // 触点与顶点匹配
        if (arr0.dist > dist && handleType < kMgHandleOutside) {
            arr0.dist = dist;
            arr0.base = orignPt;
            arr0.pt = pnt;
            arr0.type = kMgSnapPoint + handleType - kMgHandleVertext;
            arr0.shapeid = sp->getID();
            arr0.handleIndex = i;
            arr0.handleIndexSrc = dragHandle ? ignoreHandle : -1;
            handleFound = true;
        }
        
        int d = matchpt ? shape->shapec()->getHandleCount() - 1 : -1;
        for (; d >= 0; d--) {                           // 整体移动图形，顶点匹配
            if (d == ignoreHandle || shape->shapec()->isHandleFixed(d))
                continue;
            Point2d ptd (shape->shapec()->getHandlePoint(d));   // 当前图形的顶点
            
            dist = pnt.distanceTo(ptd);                 // 当前图形与其他图形顶点匹配
            if (arr0.dist > dist - _MGZERO && handleType < kMgHandleOutside) {
                arr0.dist = dist;
                arr0.base = ptd;  // 新的移动起点为当前图形的一个顶点
                arr0.pt = pnt;    // 将从ptd移到其他图形顶点pnt
                arr0.type = kMgSnapPoint + handleType - kMgHandleVertext;
                arr0.shapeid = sp->getID();
                arr0.handleIndex = i;
                arr0.handleIndexSrc = d;
                handleFound = true;
                
                // 因为对当前图形先从startM移到pointM，然后再从pointM移到matchpt
                *matchpt = orignPt + (pnt - ptd);       // 所以最后差量为(pnt-ptd)
            }
        }
    }
    
    return handleFound;
}

static void snapNear(const MgMotion* sender, const Point2d& orignPt,
                     const MgShape* shape, int ignoreHandle,
                     const MgShape* sp, SnapItem& arr0, Point2d* matchpt)
{
    Point2d nearpt, ptd;
    float dist;
    float minDist = arr0.dist;
    float tolNear = sender->displayMmToModel(0.7f);
    int d = matchpt ? shape->shapec()->getHandleCount() : 0;
    
    for (; d >= 0; d--) {
        if (d == 0) {
            ptd = orignPt;
        }
        else {
            if (d - 1 == ignoreHandle || shape->shapec()->isHandleFixed(d - 1))
                continue;
            ptd = shape->shapec()->getHandlePoint(d - 1);
        }
        dist = sp->shapec()->hitTest2(ptd, tolNear, nearpt);
        
        if (minDist > dist) {
            minDist = dist;
            arr0.base = ptd;  // 新的移动起点为当前图形的一个顶点
            arr0.pt = nearpt; // 将从ptd移到其他图形顶点pnt
            arr0.type = kMgSnapNearPt;
            arr0.shapeid = sp->getID();
            arr0.handleIndex = -1;
            arr0.handleIndexSrc = d - 1;
            if (d > 0) {    // 因为对当前图形先从startM移到pointM，然后再从pointM移到matchpt
                *matchpt = orignPt + (nearpt - ptd);
            }
        }
    }
    if (arr0.dist > minDist) {
        arr0.dist = minDist + sender->displayMmToModel(4.f);
    }
}

static void snapGrid(const MgMotion*, const Point2d& orignPt,
                     const MgShape* shape, int ignoreHandle,
                     const MgShape* sp, SnapItem arr[3], Point2d* matchpt)
{
    if (sp->shapec()->isKindOf(MgGrid::Type())) {
        Point2d newPt (orignPt);
        const MgGrid* grid = (const MgGrid*)(sp->shapec());
        
        Point2d dists(arr[1].dist, arr[2].dist);
        int type = grid->snap(newPt, dists);
        if (type & 1) {
            arr[1].base = newPt;
            arr[1].pt = newPt;
            arr[1].type = kMgSnapGridX;
            arr[1].dist = dists.x;
        }
        if (type & 2) {
            arr[2].base = newPt;
            arr[2].pt = newPt;
            arr[2].type = kMgSnapGridY;
            arr[2].dist = dists.y;
        }
        
        int d = matchpt ? shape->shapec()->getHandleCount() - 1 : -1;
        for (; d >= 0; d--) {
            if (d == ignoreHandle || shape->shapec()->isHandleFixed(d))
                continue;
            
            Point2d ptd (shape->shapec()->getHandlePoint(d));
            dists.set(mgMin(arr[0].dist, arr[1].dist), mgMin(arr[0].dist, arr[2].dist));
            
            newPt = ptd;
            type = grid->snap(newPt, dists);
            float dist = newPt.distanceTo(ptd);
            
            if ((type & 3) == 3 && arr[0].dist > dist - _MGZERO) {
                arr[0].dist = dist;
                arr[0].base = ptd;
                arr[0].pt = newPt;
                arr[0].type = kMgSnapGrid;
                arr[0].shapeid = sp->getID();
                arr[0].handleIndex = -1;
                arr[0].handleIndexSrc = d;
                
                // 因为对当前图形先从startM移到pointM，然后再从pointM移到matchpt
                *matchpt = orignPt + (newPt - ptd);     // 所以最后差量为(pnt-ptd)
            }
        }
    }
}

static void snapPoints(const MgMotion* sender, const Point2d& orignPt,
                       const MgShape* shape, int ignoreHandle,
                       const int* ignoreids, SnapItem arr[3], Point2d* matchpt)
{
    Box2d snapbox(orignPt, 2 * arr[0].dist, 0);         // 捕捉容差框
    GiTransform* xf = sender->view->xform();
    Box2d wndbox(xf->getWndRectM());
    void* it = NULL;
    
    for (const MgShape* sp = sender->view->shapes()->getFirstShape(it);
         sp; sp = sender->view->shapes()->getNextShape(it)) {
        
        if (skipShape(ignoreids, sp)) {
            continue;
        }
        Box2d extent(sp->shapec()->getExtent());
        if (extent.width() < xf->displayToModel(2, true)
            && extent.height() < xf->displayToModel(2, true)) { // 图形太小就跳过
            continue;
        }
        if (extent.isIntersect(wndbox)
            && !snapHandle(sender, orignPt, shape, ignoreHandle, sp, arr[0], matchpt)) {
            if (extent.isIntersect(snapbox)) {
                snapNear(sender, orignPt, shape, ignoreHandle, sp, arr[0], matchpt);
            }
        }
        if (extent.isIntersect(snapbox)) {
            snapGrid(sender, orignPt, shape, ignoreHandle, sp, arr, matchpt);
        }
    }
    sender->view->shapes()->freeIterator(it);
}

// hotHandle: 绘新图时，起始步骤为-1，后续步骤>0；拖动一个或多个整体图形时为-1，拖动顶点时>=0
Point2d MgCmdManagerImpl::snapPoint(const MgMotion* sender, const Point2d& orignPt, const MgShape* shape,
                                    int hotHandle, int ignoreHandle, const int* ignoreids)
{
    int ignoreids_tmp[2] = { shape ? shape->getID() : 0, 0 };
    if (!ignoreids) ignoreids = ignoreids_tmp;
    
    if (!shape || hotHandle >= shape->shapec()->getHandleCount()) {
        hotHandle = -1;         // 对hotHandle进行越界检查
    }
    _ptSnap = orignPt;   // 默认结果为当前触点位置
    
    SnapItem arr[3] = {         // 设置捕捉容差和捕捉初值
        SnapItem(_ptSnap, _ptSnap, displayMmToModel(3.f, sender)),  // XY点捕捉
        SnapItem(_ptSnap, _ptSnap, displayMmToModel(1.f, sender)),  // X分量捕捉，竖直线
        SnapItem(_ptSnap, _ptSnap, displayMmToModel(1.f, sender)),  // Y分量捕捉，水平线
    };
    
    if (shape && shape->getID() == 0 && hotHandle > 0               // 绘图命令中的临时图形
        && !shape->shapec()->isCurve()
        && !shape->shapec()->isKindOf(MgBaseRect::Type())) {        // 不是矩形或椭圆
        Point2d pt (orignPt);
        snapHV(shape->shapec()->getPoint(hotHandle - 1), pt, arr);  // 和上一个点对齐
    }
    
    Point2d pnt(-1e10f, -1e10f);                    // 当前图形的某一个顶点匹配到其他顶点pnt
    bool matchpt = (shape && shape->getID() != 0    // 拖动整个图形
                    && (hotHandle < 0 || (ignoreHandle >= 0 && ignoreHandle != hotHandle)));
    
    snapPoints(sender, orignPt, shape, ignoreHandle, ignoreids,
               arr, matchpt ? &pnt : NULL);         // 在所有图形中捕捉
    checkResult(arr);
    
    return matchpt && pnt.x > -1e8f ? pnt : _ptSnap;    // 顶点匹配优先于用触点捕捉结果
}

void MgCmdManagerImpl::checkResult(SnapItem arr[3])
{
    if (arr[0].type > 0) {                          // X和Y方向同时捕捉到一个点
        _ptSnap = arr[0].pt;                        // 结果点
        _snapBase[0] = arr[0].base;                 // 原始点
        _snapType[0] = arr[0].type;
        _snapShapeId = arr[0].shapeid;
        _snapHandle = arr[0].handleIndex;
        _snapHandleSrc = arr[0].handleIndexSrc;
    }
    else {
        _snapShapeId = 0;
        _snapHandle = -1;
        _snapHandleSrc = -1;
        
        _snapType[0] = arr[1].type;                 // 竖直方向捕捉到一个点
        if (arr[1].type > 0) {
            _ptSnap.x = arr[1].pt.x;
            _snapBase[0] = arr[1].base;
        }
        _snapType[1] = arr[2].type;                 // 水平方向捕捉到一个点
        if (arr[2].type > 0) {
            _ptSnap.y = arr[2].pt.y;
            _snapBase[1] = arr[2].base;
        }
    }
}

int MgCmdManagerImpl::getSnappedType()
{
    if (_snapType[0] >= kMgSnapPoint)
        return _snapType[0];
    return (_snapType[0] == kMgSnapGridX && _snapType[1] == kMgSnapGridY) ? kMgSnapPoint : 0;
}

int MgCmdManagerImpl::getSnappedPoint(Point2d& fromPt, Point2d& toPt)
{
    fromPt = _snapBase[0];
    toPt = _ptSnap;
    return getSnappedType();
}

bool MgCmdManagerImpl::getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc)
{
    shapeid = _snapShapeId;
    handleIndex = _snapHandle;
    handleIndexSrc = _snapHandleSrc;
    return shapeid != 0;
}

void MgCmdManagerImpl::clearSnap()
{
    _snapType[0] = 0;
    _snapType[1] = 0;
}

bool MgCmdManagerImpl::drawSnap(const MgMotion* sender, GiGraphics* gs)
{
    bool ret = false;
    
    if (sender->dragging() || !sender->view->useFinger()) {
        if (_snapType[0] >= kMgSnapPoint) {
            bool isnear = (_snapType[0] >= kMgSnapNearPt);
            GiContext ctx(-2, GiColor(0, 255, 0, 200), kGiLineDash, GiColor(0, 255, 0, 64));
            ret = gs->drawEllipse(&ctx, _ptSnap, displayMmToModel(isnear ? 3.f : 6.f, gs));
            gs->drawHandle(_ptSnap, 0);
        }
        else {
            GiContext ctx(0, GiColor(0, 255, 0, 200), kGiLineDash, GiColor(0, 255, 0, 64));
            GiContext ctxcross(-2, GiColor(0, 255, 0, 200));
            
            if (_snapType[0] > 0) {
                if (_snapBase[0] == _ptSnap) {
                    if (_snapType[0] == kMgSnapGridX) {
                        Vector2d vec(0, displayMmToModel(15.f, gs));
                        ret = gs->drawLine(&ctxcross, _ptSnap - vec, _ptSnap + vec);
                        gs->drawEllipse(&ctx, _snapBase[0], displayMmToModel(4.f, gs));
                    }
                }
                else {  // kMgSnapSameX
                    ret = gs->drawLine(&ctx, _snapBase[0], _ptSnap);
                    gs->drawEllipse(&ctx, _snapBase[0], displayMmToModel(2.5f, gs));
                }
            }
            if (_snapType[1] > 0) {
                if (_snapBase[1] == _ptSnap) {
                    if (_snapType[1] == kMgSnapGridY) {
                        Vector2d vec(displayMmToModel(15.f, gs), 0);
                        ret = gs->drawLine(&ctxcross, _ptSnap - vec, _ptSnap + vec);
                        gs->drawEllipse(&ctx, _snapBase[1], displayMmToModel(4.f, gs));
                    }
                }
                else {  // kMgSnapSameY
                    ret = gs->drawLine(&ctx, _snapBase[1], _ptSnap);
                    gs->drawEllipse(&ctx, _snapBase[1], displayMmToModel(2.5f, gs));
                }
            }
        }
    }
    
    return ret;
}
