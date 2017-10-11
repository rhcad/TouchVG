// mgsnapimpl.cpp: 实现命令管理器类
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcmdmgr_.h"
#include "mgbasicsps.h"
#include "mgcomposite.h"
#include "mglog.h"

//! 捕捉结果
struct SnapItem {
    Point2d pt;             //!< 捕捉到的坐标
    Point2d base;           //!< 参考线基准点、原始点
    Point2d startpt;        //!< 垂线起始点
    Point2d guildpt;        //!< 导向点，例如圆心
    float   maxdist;        //!< 最大容差
    float   dist;           //!< 捕捉距离
    int     type;           //!< 特征点类型
    int     shapeid;        //!< 捕捉到的图形
    int     handleIndex;    //!< 捕捉到控制点序号，最近点和垂足则为边号，交点则为另一图形的ID
    int     handleIndexSrc; //!< 待确定位置的源图形上的控制点序号，与handleIndex点匹配
    
    SnapItem() {}
    SnapItem(const Point2d& _pt, const Point2d& _base, float _dist, int _type = 0,
        int _shapeid = 0, int _handleIndex = -1, int _handleIndexSrc = -1)
        : pt(_pt), base(_base), startpt(Point2d::kInvalid()), guildpt(Point2d::kInvalid())
        , maxdist(_dist), dist(_dist), type(_type), shapeid(_shapeid)
        , handleIndex(_handleIndex), handleIndexSrc(_handleIndexSrc) {}
};

static inline float diffx(const Point2d& pt1, const Point2d& pt2)
{
    return fabsf(pt1.x - pt2.x);
}

static inline float diffy(const Point2d& pt1, const Point2d& pt2)
{
    return fabsf(pt1.y - pt2.y);
}

static int snapHV(const Point2d& basePt, Point2d& newPt, SnapItem arr[3])
{
    int ret = 0;
    float d;
    
    d = arr[1].dist - diffx(newPt, basePt);
    if (d > _MGZERO || (d > - _MGZERO
                        && diffy(newPt, basePt) < diffy(newPt, arr[1].base))) {
        arr[1].dist = diffx(newPt, basePt);
        arr[1].base = basePt;
        newPt.x = basePt.x;
        arr[1].pt = newPt;
        arr[1].type = kMgSnapSameX;
        ret |= 1;
    }
    d = arr[2].dist - diffy(newPt, basePt);
    if (d > _MGZERO || (d > - _MGZERO
                        && diffx(newPt, basePt) < diffx(newPt, arr[2].base))) {
        arr[2].dist = diffy(newPt, basePt);
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
    bool skip = (sp->shapec()->getFlag(kMgNoSnap) || !sp->shapec()->isVisible());
    for (int t = 0; ignoreids[t] != 0 && !skip; t++) {
        skip = (ignoreids[t] == sp->getID());           // 跳过当前图形
    }
    return skip;
}

static bool snapHandle(const MgMotion* sender, const Point2d& orgpt, int mask,
                       const MgShape* shape, int ignoreHd,
                       const MgShape* sp, SnapItem& arr0, Point2d* matchpt)
{
    bool ignored = (sp->shapec()->isKindOf(MgArc::Type())
                    || sp->shapec()->isKindOf(MgSplines::Type()));  // 除圆弧和自由曲线外
    int n = ignored ? 0 : sp->getHandleCount();
    bool dragHandle = (!shape || shape->getID() == 0    // 正画的图形:末点动
                       || (ignoreHd >= 0 && orgpt == shape->getHandlePoint(ignoreHd)) // 拖已有图形的点
                       || n == 1);                      // 点可定位
    bool handleFound = false;
    
    for (int i = 0; i < n; i++) {                       // 循环每一个控制点
        Point2d pnt(sp->getHandlePoint(i));             // 已有图形的一个控制点
        int handleType = sp->getHandleType(i);
        
        if ((mask & (1 << handleType)) == 0)
            continue;
        
        float dist = pnt.distanceTo(orgpt);             // 触点与顶点匹配
        
        if (handleType == kMgHandleMidPoint) {          // 交点优先于中点
            dist += sender->displayMmToModel(0.5f);
        }
        if (dragHandle && dist < arr0.maxdist
            && arr0.dist > dist - _MGZERO
            && handleType < kMgHandleOutside
            && !(shape && shape->getID() == 0               // 新画线段的起点已
                 && shape->getPointCount() > 1
                 && pnt == shape->getPoint(0))) {           // 与此点重合的除外
            arr0.dist = dist;
            arr0.base = orgpt;
            arr0.pt = pnt;
            arr0.type = kMgSnapPoint + handleType - kMgHandleVertex;
            arr0.shapeid = sp->getID();
            arr0.handleIndex = i;
            arr0.handleIndexSrc = dragHandle ? ignoreHd : -1;
            handleFound = true;
        }
        
        int d = matchpt ? shape->getHandleCount() - 1 : -1;
        for (; d >= 0; d--) {                           // 整体移动图形，顶点匹配
            if (d == ignoreHd || shape->shapec()->isHandleFixed(d))
                continue;
            Point2d ptd (shape->getHandlePoint(d));     // 当前图形的顶点
            
            dist = pnt.distanceTo(ptd);                 // 当前图形与其他图形顶点匹配
            if (handleType == kMgHandleMidPoint) {      // 交点优先于中点
                dist += sender->displayMmToModel(0.5f);
            }
            if (dist < arr0.maxdist && arr0.dist > dist - _MGZERO
                && handleType < kMgHandleOutside) {
                arr0.dist = dist;
                arr0.base = ptd;  // 新的移动起点为当前图形的一个顶点
                arr0.pt = pnt;    // 将从ptd移到其他图形顶点pnt
                arr0.type = kMgSnapPoint + handleType - kMgHandleVertex;
                arr0.shapeid = sp->getID();
                arr0.handleIndex = i;
                arr0.handleIndexSrc = d;
                handleFound = true;
                
                // 因为对当前图形先从startM移到pointM，然后再从pointM移到matchpt
                *matchpt = orgpt + (pnt - ptd);         // 所以最后差量为(pnt-ptd)
            }
        }
    }
    
    return handleFound;
}

static bool snapPerp(const MgMotion* sender, const Point2d& orgpt, const Tol& tol,
                     const MgShape* shape, const MgShape* sp, SnapItem& arr0,
                     bool perpOut, const Box2d& nearBox)
{
    int ret = -1;
    const MgBaseShape* s = sp->shapec();
    MgHitResult nearres;
    
    if (shape && shape->getID() == 0            // 正画的线段 与 折线类图形的边垂直
        && shape->shapec()->isKindOf(MgLine::Type()) && !s->isCurve()) {
        int n = s->getPointCount();
        Point2d perp1, perp2;
        const Point2d start(shape->getPoint(0));
        
        for (int i = 0, edges = n - (s->isClosed() ? 0 : 1); i < edges; i++) {
            Point2d pt1(s->getHandlePoint(i));
            Point2d pt2(s->getHandlePoint((i + 1) % n));
            float d2 = mglnrel::ptToBeeline2(pt1, pt2, orgpt, perp2);
            
            if (mglnrel::isColinear2(pt1, pt2, start, tol)) {   // 起点在线上
                float dist = perp2.distanceTo(start) * 2;
                if (d2 > 2 * arr0.maxdist && dist < arr0.maxdist && arr0.dist > dist
                    && (perpOut || mglnrel::isProjectBetweenLine(pt1, pt2, perp2))) {
                    arr0.startpt = start;
                    arr0.dist = dist;                           // 当前点距捕捉点
                    arr0.base = start;                          // 起点同垂足
                    arr0.pt = orgpt + (start - perp2);          // 投影到垂线
                    arr0.type = kMgSnapPerp;
                    arr0.shapeid = sp->getID();
                    arr0.handleIndex = i;
                    arr0.handleIndexSrc = -1;
                    ret = i;
                    
                    const MgShape* sp2 = sp->getParent()->hitTest(nearBox, nearres);
                    if (sp2 && sp2 != shape) {
                        pt1 = start;
                        pt2 = arr0.pt;
                        int n = MgEllipse::crossCircle(pt1, pt2, sp2->shapec());
                        if (n < 0) {
                            MgPath path1;
                            
                            path1.moveTo(pt1);
                            path1.lineTo(pt1 + (pt2 - pt1) * 2.f);
                            
                            path1.crossWithPath(sp2->shapec()->getPath(), Box2d(orgpt, 1e10f, 0), arr0.pt);
                        } else if (n > 0) {
                            arr0.pt = pt2.distanceTo(orgpt) < pt1.distanceTo(orgpt) ? pt2 : pt1;
                        }
                        arr0.type = kMgSnapPerpNear;
                        arr0.handleIndexSrc = sp2->getID();
                    }
                }
            } else if (d2 < arr0.maxdist) {                     // 终点在线附近
                mglnrel::ptToBeeline2(pt1, pt2, start, perp1);
                float dist = perp1.distanceTo(orgpt);
                if (dist < arr0.maxdist && arr0.dist > dist
                    && (perpOut || mglnrel::isProjectBetweenLine(pt1, pt2, perp1))) {
                    arr0.startpt = start;
                    arr0.dist = dist;                           // 当前点距垂足
                    arr0.base = perp1;                          // 垂足为基点
                    arr0.pt   = arr0.base;                      // 也是捕捉点
                    arr0.type = kMgSnapPerp;
                    arr0.shapeid = sp->getID();
                    arr0.handleIndex = i;
                    arr0.handleIndexSrc = sp->getID();
                    ret = i;
                }
            }
        }
    }
    
    return ret >= 0;
}

static void snapNear(const MgMotion* sender, const Point2d& orgpt,
                     const MgShape* shape, int ignoreHd, float tolNear,
                     const MgShape* sp, SnapItem& arr0,
                     Point2d* matchpt, const Point2d& ignoreStart)
{
    const int n = sp->getPointCount();
    if ((arr0.type >= kMgSnapGrid && arr0.type < kMgSnapNearPt && arr0.type != kMgSnapParallel) || n < 2) {
        return;
    }
    
    Point2d ptd;
    MgHitResult res;
    const float mind = sender->displayMmToModel(4.f);
    float minDist = arr0.type >= kMgSnapNearPt ? arr0.dist - mind : tolNear;
    int d = matchpt && shape ? shape->getHandleCount() : 0;
    
    res.disnableSnapVertex();
    for (int i = sp->shapec()->getExtent().contains(ignoreStart) ? n - 1 : 0; i >= 0; i--) {
        if (sp->getHandlePoint(i) == ignoreStart) {
            res.ignoreHandle = i;
            break;
        }
    }
    
    for (; d >= 0; d--) {       // 对需定位的图形(shape)的每个控制点和当前触点
        if (d == 0) {
            ptd = orgpt;        // 触点与边匹配
        } else {
            if (d - 1 == ignoreHd || shape->shapec()->isHandleFixed(d - 1))
                continue;
            ptd = shape->getHandlePoint(d - 1);   // 控制点与边匹配
        }
        float dist = sp->shapec()->hitTest(ptd, tolNear, res);
        
        if (minDist > dist) {
            minDist = dist;
            arr0.dist = minDist + mind;
            arr0.base = ptd;            // 新的移动起点为当前图形的一个顶点
            arr0.pt = res.nearpt;       // 将从ptd移到其他图形顶点pnt
            arr0.type = kMgSnapNearPt;
            arr0.shapeid = sp->getID(); // 最近点在此图形上
            arr0.handleIndex = res.segment;
            arr0.handleIndexSrc = d - 1;
            if (matchpt) {  // 因为对当前图形先从startM移到pointM，然后再从pointM移到matchpt
                *matchpt = orgpt + (res.nearpt - ptd);
            }
        }
    }
}

static void snapExtend(const MgMotion* sender, const Point2d& orgpt,
                     const MgShape* shape, int ignoreHd, float tolNear,
                     const MgShape* sp, SnapItem& arr0,
                     Point2d* matchpt, const Point2d& ignoreStart)
{
    const int n = sp->getPointCount();
    if ((arr0.type >= kMgSnapGrid && arr0.type < kMgSnapNearPt && arr0.type != kMgSnapParallel)
        || sp->shapec()->isCurve()
        || !shape->shapec()->isKindOf(MgLine::Type())
        || n < 2 || n > 6 || matchpt) {
        return;
    }
    
    MgHitResult res;
    const float mind = sender->displayMmToModel(4.f);
    float minDist = arr0.type >= kMgSnapNearPt ? arr0.dist - mind : tolNear;
    
    for (int i = sp->shapec()->getExtent().contains(ignoreStart) ? n - 1 : 0; i >= 0; i--) {
        if (sp->getHandlePoint(i) == ignoreStart) {
            res.ignoreHandle = i;
            break;
        }
    }
    if (res.ignoreHandle >= 0) {
        for (int i = 0, m = sp->shapec()->isClosed() ? n : n - 1; i < m; i++) {
            if ((i - res.ignoreHandle + n) % n != 1 && (res.ignoreHandle - i + n) % n != 1)
                continue;
            Point2d pt2(sp->getHandlePoint(i % n));
            float dist = mglnrel::ptToBeeline2(ignoreStart, pt2, orgpt, res.nearpt);
            
            if (minDist > dist) {
                float v = (res.nearpt - ignoreStart).projectScaleToVector(pt2 - ignoreStart);
                if (v < -_MGZERO) {
                    minDist = dist;
                    arr0.dist = minDist + mind;
                    arr0.base = orgpt;
                    arr0.pt = res.nearpt;
                    arr0.guildpt = pt2;
                    arr0.startpt = ignoreStart;
                    arr0.type = kMgSnapExtendPt;
                    arr0.shapeid = sp->getID();
                    arr0.handleIndex = (i + res.ignoreHandle) / 2;
                    arr0.handleIndexSrc = -1;
                }
            }
        }
    }
}

static bool snapTangent(const MgMotion* sender, const Point2d& orgpt, const MgShape* shape,
                        int ignoreHd, const MgShape* spTarget, SnapItem& arr0, Point2d* matchpt)
{
    const bool c1 = MgEllipse::isCircle(spTarget->shapec());
    const bool c2 = MgEllipse::isCircle(shape->shapec());
    const bool l1 = spTarget->shapec()->isKindOf(MgLine::Type());
    const bool l2 = shape->shapec()->isKindOf(MgLine::Type());
    
    if (!((c1 || c2) && (c1 || l1) && (c2 || l2))) {
        return false;
    }
    if (l1 && c2) {
        mgSwap(spTarget, shape);
    }
    
    const MgEllipse* circle = (const MgEllipse*)spTarget->shapec();
    Point2d cen(circle->getCenter());
    float r = circle->getRadiusX();
    bool ret = false;
    
    if (c2 && circle->getHandleType(ignoreHd) == kMgHandleCenter) {
        cen = orgpt;
    }
    if (c2 && ignoreHd >= 0 && circle->getHandleType(ignoreHd) != kMgHandleCenter) {
        r = cen.distanceTo(orgpt);
    }
    if (r < _MGZERO) {}
    else if (MgEllipse::isCircle(shape->shapec())) {
        
    } else {
        Point2d pt1(shape->getPoint(c2 || ignoreHd < 0 ? 0 : 1 - ignoreHd));    // 线段的不动点
        Point2d pt2(c2 || ignoreHd < 0 ? shape->getPoint(1) : orgpt);           // 线段的动点
        Point2d perp, tanpt;
        float dist = mglnrel::ptToBeeline2(pt1, pt2, cen, perp);        // 圆心到直线的距离及垂足perp
        
        if (fabsf(dist - r) < arr0.dist                                 // 圆心到直线的距离约等于半径
            && (shape->shapec()->getSubType()                           // 是射线或直线则不检查
                || mglnrel::isBetweenLine3(pt1, pt2, perp)))            // 垂足在线段上
        {
            if (c2 && ignoreHd >= 0
                && (spTarget->getID() == 0 ||                           // 正在画圆或拖动圆的象限点
                    circle->getHandleType(ignoreHd) == kMgHandleQuadrant))
            {
                arr0.pt = perp;
                arr0.base = cen.rulerPoint(perp, r, 0);
                arr0.type = kMgSnapTangent;
                arr0.dist = fabsf(dist - r);
                arr0.startpt = perp;                                    // 记下startpt为切点
                arr0.guildpt = cen.rulerPoint(perp, r, 0);              // 记下guildpt为切点圆心连线与圆的交点
                arr0.shapeid = shape->getID();                          // 捕捉到线段上
                arr0.handleIndex = -2;                                  // -2:拖动圆的象限点
                arr0.handleIndexSrc = -1;
                
                if (matchpt) {
                    *matchpt = orgpt + (arr0.pt - arr0.base);
                }
                ret = true;
            }
            else if (matchpt || ignoreHd < 0                            // 整体移动线段或圆
                     || (c2 && circle->getHandleType(ignoreHd) == kMgHandleCenter)) // 拖动圆心
            {
                tanpt = cen.rulerPoint(perp, r, 0);                     // 平移线段计算出切点
                arr0.dist = fabsf(dist - r);
                arr0.base = c2 ? tanpt : perp;                          // 若移动线段:从垂足移到切点
                arr0.pt = c2 ? perp : tanpt;                            // 若移动圆:从切点移到垂足
                arr0.type = kMgSnapTangent;
                arr0.shapeid = (c2 ? shape : spTarget)->getID();
                arr0.handleIndex = c2 ? 0 : -1;                         // handleIndex==0则移动圆
                arr0.handleIndexSrc = -1;
                
                if (matchpt) {
                    *matchpt = orgpt + (arr0.pt - arr0.base);
                }
                ret = true;
            } else if (!c2 && ignoreHd < 2) {                           // 移动线段的端点
                if (fabsf(pt1.distanceTo(cen) - r) < r * 1e-4f) {       // 线段的不动点在圆上
                    tanpt = pt1;                                        // 触点orgpt投影到切线:perp
                    dist = mglnrel::ptToBeeline2(tanpt, tanpt + (cen - pt1).perpVector(), orgpt, perp);
                } else if (mgcurv::crossTwoCircles(tanpt, pt2, (pt1 + cen)/2,
                                                   pt1.distanceTo(cen)/2, cen, r) > 0) {    // 直接为pt1到cen的圆
                    tanpt = tanpt.distanceTo(perp) < pt2.distanceTo(perp) ? tanpt : pt2;    // 与给定圆的交点
                    dist = mglnrel::ptToBeeline2(pt1, tanpt, orgpt, perp);  // 触点orgpt投影到切线:perp
                }
                if (arr0.dist > dist) {                                 // 原垂足到切点的距离足够小
                    arr0.dist = dist;
                    arr0.base = tanpt;                                  // 参考点为切点
                    arr0.pt = perp;                                     // 捕捉点为触点投影到切线的点
                    arr0.type = kMgSnapTangent;
                    arr0.shapeid = spTarget->getID();                   // 捕捉到圆上
                    arr0.handleIndex = -1;
                    arr0.handleIndexSrc = ignoreHd;
                    arr0.startpt = pt1;                                 // 记下startpt为线段的不动点
                    arr0.guildpt = cen;                                 // 记下guildpt为圆心
                    ret = true;
                }
            }
        }
    }
    
    return ret;
}

static void snapGrid(const MgMotion*, const Point2d& orgpt,
                     const MgShape* shape, int ignoreHd,
                     const MgShape* sp, SnapItem arr[3], Point2d* matchpt)
{
    if (sp->shapec()->isKindOf(MgGrid::Type())) {
        Point2d newPt (orgpt);
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
        
        int d = matchpt && shape ? shape->getHandleCount() - 1 : -1;
        for (; d >= 0; d--) {
            if (d == ignoreHd || shape->shapec()->isHandleFixed(d))
                continue;
            
            Point2d ptd (shape->getHandlePoint(d));
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
                *matchpt = orgpt + (newPt - ptd);   // 所以最后差量为(pnt-ptd)
            }
        }
    }
}

static bool snapCross(const MgMotion* sender, const Point2d& orgpt,
                      const int* ignoreids, int ignoreHd,
                      const MgShape* shape, const MgShape* sp1,
                      SnapItem& arr0, Point2d* matchpt)
{
    MgShapeIterator it(sender->view->shapes());
    Point2d ptd, ptcross, pt1, pt2;
    int d = matchpt ? shape->getHandleCount() : 0;
    int ret = 0;
    
    for (; d >= 0; d--) {       // 对需定位的图形(shape)的每个控制点和当前触点
        if (d == 0) {
            ptd = orgpt;        // 触点与交点匹配
        }
        else {
            if (d - 1 == ignoreHd || shape->shapec()->isHandleFixed(d - 1))
                continue;
            ptd = shape->getHandlePoint(d - 1);   // 控制点与交点匹配
        }
        
        Box2d snapbox(orgpt, 2 * arr0.maxdist, 0);
        
        if (sp1->getPointCount() < 2
            || !sp1->shapec()->hitTestBox(snapbox)) {
            continue;
        }
        
        MgPath path1(sp1->shapec()->getPath());
        
        while (const MgShape* sp2 = it.getNext()) {
            if (skipShape(ignoreids, sp2) || sp2 == shape || sp2 == sp1
                || sp2->getPointCount() < 2
                || !sp2->shapec()->hitTestBox(snapbox)) {
                continue;
            }
            
            int n = MgEllipse::crossCircle(pt1, pt2, sp1->shapec(), sp2->shapec(), orgpt);
            
            if (n < 0) {
                n = path1.crossWithPath(sp2->shapec()->getPath(), snapbox, ptcross) ? 1 : 0;
            } else if (n > 0) {
                ptcross = pt2.distanceTo(ptd) < pt1.distanceTo(ptd) ? pt2 : pt1;
                n = snapbox.contains(ptcross) ? 1 : 0;
            }
            
            if (n) {
                float dist = ptcross.distanceTo(ptd) - _MGZERO; // 优先于顶点
                if (dist < arr0.maxdist && arr0.dist > dist) {
                    arr0.dist = dist;
                    arr0.base = ptd;
                    arr0.pt = ptcross;
                    arr0.type = kMgSnapIntersect;
                    arr0.shapeid = sp1->getID();
                    arr0.handleIndex = sp2->getID();
                    arr0.handleIndexSrc = d - 1;
                    if (matchpt) {
                        *matchpt = orgpt + (ptcross - ptd);
                    }
                    ret = sp2->getID();
                }
            }
        }
    }
    
    return ret != 0;
}

static bool snapParallel(const MgMotion* sender, const Point2d& orgpt,
                         const int* ignoreids, int ignoreHd,
                         const MgShape* shape, const MgShape* sp1, SnapItem& arr0)
{
    if (arr0.type == kMgSnapNearPt || arr0.type == kMgSnapExtendPt
        || !shape->shapec()->isKindOf(MgLine::Type())
        || sp1->getType() != shape->getType()
        || shape->getID() != 0) {
        return false;
    }
    
    Point2d start(shape->getPoint(0));
    float angle1 = (orgpt - start).angle2();
    float angle2 = ((MgLine*)sp1->shapec())->angle();
    float diff = fabsf(mgbase::getDiffAngle(angle1, angle2));
    
    if (fabsf(diff) < _M_D2R * 3) {
        angle1 = angle2;
    } else if (fabsf(diff - _M_PI) < _M_D2R * 3) {
        angle1 = angle2 + _M_PI;
    } else {
        return false;
    }
    
    float minLen = sender->displayMmToModel(5);
    float len = orgpt.distanceTo(start);
    
    if (len < minLen || ((MgLine*)sp1->shapec())->length() < minLen) {
        return false;
    }
    
    Point2d to(start.polarPoint(angle1, len));
    float dist = orgpt.distanceTo(to);
    
    if (dist < arr0.maxdist && arr0.dist > dist) {
        arr0.dist = dist;
        arr0.base = orgpt;
        arr0.pt = to;
        arr0.type = kMgSnapParallel;
        arr0.shapeid = sp1->getID();
        arr0.handleIndex = -1;
        arr0.handleIndexSrc = 1;
        dist = -1;
    }
    
    return dist < 0;
}

static void snapShape(const MgMotion* sender, const Point2d& orgpt,
                      float minBox, const Box2d& snapbox, const Box2d& wndbox,
                      int handleMask, bool needNear, bool needExtend, float tolNear,
                      bool needPerp, bool perpOut, const Tol& tolPerp,
                      bool needTangent, bool needCross, bool needParallel, const Box2d& nearBox, bool needGrid,
                      const MgShape* spTarget, const MgShape* shape, int ignoreHd,
                      const int* ignoreids, SnapItem arr[3],
                      Point2d* matchpt, const Point2d& ignoreStart)
{
    if (skipShape(ignoreids, spTarget) || spTarget == shape) {
        return;
    }
    
    Box2d extent(spTarget->shapec()->getExtent());
    int b = 0;
    
    if (spTarget->getPointCount() > 1
        && extent.width() < minBox && extent.height() < minBox) { // 图形太小就跳过
        return;
    }
    if (extent.isIntersect(wndbox)) {
        b |= (handleMask && snapHandle(sender, orgpt, handleMask, shape, ignoreHd, spTarget, arr[0], matchpt));
        b |= (needPerp && snapPerp(sender, orgpt, tolPerp, shape, spTarget, arr[0], perpOut, nearBox));
        b |= (needCross && snapCross(sender, orgpt, ignoreids, ignoreHd, shape, spTarget, arr[0], matchpt));
        b |= (needParallel && shape && snapParallel(sender, orgpt, ignoreids, ignoreHd, shape, spTarget, arr[0]));
        b |= (needTangent && shape && snapTangent(sender, orgpt, shape, ignoreHd, spTarget, arr[0], matchpt));
        
        if (!b && needNear) {
            snapNear(sender, orgpt, shape, ignoreHd, tolNear,
                     spTarget, arr[0], matchpt, ignoreStart);
        }
        if (!b && needExtend && shape) {
            snapExtend(sender, orgpt, shape, ignoreHd, tolNear,
                       spTarget, arr[0], matchpt, ignoreStart);
        }
    }
    if (!b && needGrid && extent.isIntersect(snapbox)) {
        snapGrid(sender, orgpt, shape, ignoreHd, spTarget, arr, matchpt);
    }
}

static inline int getHandleMask(MgView* view)
{
    int handleMask = 0;
    
    if (view->getOptionBool("snapVertex", true))
        handleMask |= 1 << kMgHandleVertex;
    if (view->getOptionBool("snapCenter", true))
        handleMask |= 1 << kMgHandleCenter;
    if (view->getOptionBool("snapMidPoint", true))
        handleMask |= 1 << kMgHandleMidPoint;
    if (view->getOptionBool("snapQuadrant", false))
        handleMask |= 1 << kMgHandleQuadrant;
    
    return handleMask;
}

static void snapPoints(const MgMotion* sender, const Point2d& orgpt,
                       const MgShape* shape, int ignoreHd,
                       const int* ignoreids, SnapItem arr[3],
                       Point2d* matchpt, const Point2d& ignoreStart, bool startMustVertex)
{
    if (!sender->view->getOptionBool("snapEnabled", true)
        || (shape && ignoreHd >= 0 &&
            shape->getHandleType(ignoreHd) > kMgHandleOutside)) {
        return;
    }
    
    Box2d snapbox(orgpt, 2 * arr[0].dist, 0);       // 捕捉容差框
    GiTransform* xf = sender->view->xform();
    Box2d wndbox(xf->getWndRectM());
    MgShapeIterator it(sender->view->shapes());
    
    int handleMask = startMustVertex ? (1 << kMgHandleVertex) : getHandleMask(sender->view);
    bool needNear = !!sender->view->getOptionBool("snapNear", true);
    bool needExtend = !!sender->view->getOptionBool("snapExtend", false);
    bool needPerp = !!sender->view->getOptionBool("snapPerp", true);
    bool perpOut = !!sender->view->getOptionBool("perpOut", false);
    bool needTangent = !!sender->view->getOptionBool("snapTangent", true);
    bool needCross = !!sender->view->getOptionBool("snapCross", true);
    bool needParallel = !!sender->view->getOptionBool("snapParallel", true);
    float tolNear = sender->displayMmToModel("snapNearTol", 3.f);
    Tol tolPerp(sender->displayMmToModel(1));
    bool needGrid = !!sender->view->getOptionBool("snapGrid", true);
    Box2d nearBox(orgpt, needNear ? tolNear : 0.f, 0);
    float minBox = xf->displayToModel(2, true);
    
    if (shape) {
        wndbox.unionWith(shape->shapec()->getExtent().inflate(arr[0].dist));
    }
    while (const MgShape* spTarget = it.getNext()) {
        snapShape(sender, orgpt, minBox, snapbox, wndbox,
                  handleMask, needNear, needExtend, tolNear,
                  needPerp, perpOut, tolPerp,
                  needTangent, needCross, needParallel, nearBox, needGrid,
                  spTarget, shape, ignoreHd, ignoreids, arr, matchpt, ignoreStart);
        
        if (spTarget->shapec()->isKindOf(MgGroup::Type())
            && sender->view->getOptionBool("snapInGroup", false)) {
            MgShapeIterator it2( ((const MgGroup*)spTarget->shapec())->shapes());
            while (const MgShape* sp2 = it2.getNext()) {
                snapShape(sender, orgpt, minBox, snapbox, wndbox,
                          handleMask, needNear, false, tolNear,
                          false, false, tolPerp,
                          false, false, false, nearBox, false,
                          sp2, shape, ignoreHd, ignoreids, arr, matchpt, ignoreStart);
            }
        }
    }
}

// hotHandle: 绘新图时，起始步骤为-1，后续步骤>0；拖动一个或多个整体图形时为-1，拖动顶点时>=0
Point2d MgCmdManagerImpl::snapPoint(const MgMotion* sender, const Point2d& orgpt, const MgShape* shape,
                                    int hotHandle, int ignoreHd, const int* ignoreids)
{
    bool startMustVertex = (!shape && hotHandle == 1 && ignoreHd < 0
                            && sender->view->getOptionBool("startMustVertex", false));
    const int ignoreids_tmp[2] = { shape ? shape->getID() : 0, 0 };
    if (!ignoreids) ignoreids = ignoreids_tmp;
    
    if (!shape || hotHandle >= shape->getHandleCount()) {
        hotHandle = -1;         // 对hotHandle进行越界检查
    }
    if (!shape || ignoreHd >= shape->getHandleCount()) {
        ignoreHd = -1;          // 对ignoreHd进行越界检查
    }
    _ptSnap = orgpt;   // 默认结果为当前触点位置
    
    const float xytol = startMustVertex ? 1e5f : sender->displayMmToModel("snapPointTol", 4.f);
    const float xtol = sender->displayMmToModel("snapXTol", 1.f);
    SnapItem arr[3] = {         // 设置捕捉容差和捕捉初值
        SnapItem(_ptSnap, _ptSnap, xytol),                          // XY点捕捉
        SnapItem(_ptSnap, _ptSnap, xtol),                           // X分量捕捉，竖直线
        SnapItem(_ptSnap, _ptSnap, xtol),                           // Y分量捕捉，水平线
    };
    
    if (shape && shape->getID() == 0 && hotHandle > 0               // 绘图命令中的临时图形
        && !shape->shapec()->isCurve()                              // 是线段或折线
        && !shape->shapec()->isKindOf(MgBaseRect::Type())) {        // 不是矩形或椭圆
        Point2d pt (orgpt);
        snapHV(shape->getPoint(hotHandle - 1), pt, arr);  // 和上一个点对齐
    }
    
    Point2d pnt(-1e10f, -1e10f);                    // 当前图形的某一个顶点匹配到其他顶点pnt
    bool matchpt = (shape && shape->getID() != 0    // 拖动整个图形
                    && (hotHandle < 0
                        || (ignoreHd >= 0 && ignoreHd != hotHandle)
                        || shape->getHandleType(hotHandle) == kMgHandleCenter));
    
    snapPoints(sender, orgpt, shape, ignoreHd < 0 ? hotHandle : ignoreHd, ignoreids,
               arr, matchpt ? &pnt : NULL, _ignoreStart, startMustVertex);  // 在所有图形中捕捉
    checkResult(arr, hotHandle);
    pnt = matchpt && pnt.x > -1e8f ? pnt : _ptSnap; // 顶点匹配优先于用触点捕捉结果
    
    if (arr[0].type == kMgSnapNone) {
        int decimal = sender->view->getOptionInt("snapRoundCell", 1);
        float mm = sender->displayMmToModel(1);
        
        pnt.x = mgbase::roundReal(pnt.x / mm, decimal) * mm;
        pnt.y = mgbase::roundReal(pnt.y / mm, decimal) * mm;
    }
    
    return pnt;
}

void MgCmdManagerImpl::checkResult(SnapItem arr[3], int hotHandle)
{
    if (arr[0].type > 0) {                          // X和Y方向同时捕捉到一个点
        _ptSnap = arr[0].pt;                        // 结果点
        _snapBase[0] = arr[0].base;                 // 原始点
        _snapType[0] = arr[0].type;
        _snapShapeId = arr[0].shapeid;
        _snapHandle = arr[0].handleIndex;
        _snapHandleSrc = arr[0].handleIndexSrc;
        _startpt = arr[0].startpt;
        _guildpt = arr[0].guildpt;
        if (_snapHandleSrc < 0 && (_snapType[0] == kMgSnapNearPt ||
                                   _snapType[0] == kMgSnapExtendPt ||
                                   _snapType[0] == kMgSnapPoint)) {
            _snapHandleSrc = hotHandle;
        }
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

int MgCmdManagerImpl::getSnapOptions(MgView* view) const
{
    int bits = 0;
    
    if (view->getOptionBool("snapEnabled", true)) {
        if (view->getOptionBool("snapGrid", true))
            bits |= kMgOptionSnapGrid;
        if (view->getOptionBool("startMustVertex", false))
            bits |= kMgOptionStartMustVertex;
        if (view->getOptionBool("snapVertex", true))
            bits |= kMgOptionSnapVertex;
        if (view->getOptionBool("snapCenter", true))
            bits |= kMgOptionSnapCenter;
        if (view->getOptionBool("snapMidPoint", true))
            bits |= kMgOptionSnapMidPoint;
        if (view->getOptionBool("snapQuadrant", false))
            bits |= kMgOptionSnapQuadrant;
        if (view->getOptionBool("snapCross", true))
            bits |= kMgOptionSnapCross;
        if (view->getOptionBool("snapParallel", true))
            bits |= kMgOptionSnapParallel;
        if (view->getOptionBool("snapPerp", true))
            bits |= kMgOptionSnapPerp;
        if (view->getOptionBool("perpOut", false))
            bits |= kMgOptionSnapPerpOut;
        if (view->getOptionBool("snapTangent", true))
            bits |= kMgOptionSnapTangent;
        if (view->getOptionBool("snapNear", true))
            bits |= kMgOptionSnapNear;
        if (view->getOptionBool("snapExtend", false))
            bits |= kMgOptionSnapExtend;
    }
    return bits;
}

void MgCmdManagerImpl::setSnapOptions(MgView* view, int bits)
{
    view->setOptionBool("snapEnabled", !!bits);
    if (bits) {
        view->setOptionBool("snapGrid", !!(bits & kMgOptionSnapGrid));
        view->setOptionBool("startMustVertex", !!(bits & kMgOptionStartMustVertex));
        view->setOptionBool("snapVertex", !!(bits & kMgOptionSnapVertex));
        view->setOptionBool("snapCenter", !!(bits & kMgOptionSnapCenter));
        view->setOptionBool("snapMidPoint", !!(bits & kMgOptionSnapMidPoint));
        view->setOptionBool("snapQuadrant", !!(bits & kMgOptionSnapQuadrant));
        view->setOptionBool("snapCross", !!(bits & kMgOptionSnapCross));
        view->setOptionBool("snapParallel", !!(bits & kMgOptionSnapParallel));
        view->setOptionBool("snapPerp", !!(bits & kMgOptionSnapPerp));
        view->setOptionBool("perpOut", !!(bits & kMgOptionSnapPerpOut));
        view->setOptionBool("snapTangent", !!(bits & kMgOptionSnapTangent));
        view->setOptionBool("snapNear", !!(bits & kMgOptionSnapNear));
        view->setOptionBool("snapExtend", !!(bits & kMgOptionSnapExtend));
    }
}

int MgCmdManagerImpl::getSnappedType() const
{
    if (_snapType[0] >= kMgSnapPoint)
        return _snapType[0];
    if (_snapType[0] == kMgSnapGridX && _snapType[1] == kMgSnapGridY)
        return kMgSnapGrid;
    return 0;
}

int MgCmdManagerImpl::getSnappedPoint(Point2d& fromPt, Point2d& toPt) const
{
    fromPt = _snapBase[0];
    toPt = _ptSnap;
    return getSnappedType();
}

int MgCmdManagerImpl::getSnappedPoint(Point2d& fromPt, Point2d& toPt,
                                      Point2d& startPt, Point2d& guildPt) const
{
    fromPt = _snapBase[0];
    toPt = _ptSnap;
    startPt = _startpt;
    guildPt = _guildpt;
    return getSnappedType();
}

bool MgCmdManagerImpl::getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc) const
{
    shapeid = _snapShapeId;
    handleIndex = _snapHandle;
    handleIndexSrc = _snapHandleSrc;
    return shapeid != 0;
}

void MgCmdManagerImpl::setIgnoreStartPoint(const Point2d& pt)
{
    _ignoreStart = pt;
}

void MgCmdManagerImpl::clearSnap(const MgMotion* sender)
{
    _ignoreStart.set(_FLT_MAX, _FLT_MAX);
    if (_snapType[0] || _snapType[1]) {
        _snapType[0] = kMgSnapNone;
        _snapType[1] = kMgSnapNone;
        sender->view->redraw();
    }
}

static GiHandleTypes snapTypeToHandleType(int snapType)
{
    switch (snapType) {
        case kMgSnapPoint: return kGiHandleNode;
        case kMgSnapCenter: return kGiHandleCenter;
        case kMgSnapMidPoint: return kGiHandleMidPoint;
        case kMgSnapQuadrant: return kGiHandleQuadrant;
        case kMgSnapIntersect: return kGiHandleIntersect;
        case kMgSnapParallel: return kGiHandleParallel;
        case kMgSnapTangent: return kGiHandleTangent;
        case kMgSnapNearPt: return kGiHandleNear;
        default: return kGiHandleVertex;
    }
}

bool MgCmdManagerImpl::drawPerpMark(GiGraphics* gs, const GiContext& ctx,
                                    const Point2d& a, const Point2d& b,
                                    const Point2d& perp, const Point2d& c, float len) const
{
    float d1 = a.distanceTo(perp);
    float d2 = b.distanceTo(perp);
    Point2d markpt1(perp.rulerPoint(d1 > d2 ? a : b, len, 0));
    Point2d markpt3(perp.rulerPoint(c, len, 0));
    Point2d markpts[] = { markpt1, markpt1 + (markpt3 - perp), markpt3 };
    
    return gs->drawLines(&ctx, 3, markpts);
}

void MgCmdManagerImpl::drawPerpMark(const MgMotion* sender, GiGraphics* gs, GiContext& ctx) const
{
    const MgShape* sp = sender->view->shapes()->findShape(_snapShapeId);
    int n = sp ? sp->getPointCount() : 0;
    float r = displayMmToModel(1.2f, gs);
    
    if (n > 1 && _snapHandle >= 0) {
        if (_snapType[0] == kMgSnapPerpNear) {
            gs->drawCircle(&ctx, _ptSnap, displayMmToModel(4.f, gs));
            gs->drawHandle(_ptSnap, kGiHandleNear);
        }
        
        Point2d pt1(sp->getHandlePoint(_snapHandle));
        Point2d pt2(sp->getHandlePoint((_snapHandle + 1) % n));
        
        drawPerpMark(gs, GiContext(-2, GiColor(255, 255, 0, 200)), pt1, pt2, _snapBase[0],
                     _ptSnap == _snapBase[0] ? _startpt : _ptSnap, 2 * r);
        
        ctx.setLineWidth(0, false);
        ctx.setLineStyle(GiContext::kSolidLine);
        gs->drawBeeline(&ctx, pt1, pt2);
        gs->drawTextAt(sender->view->getOptionInt("snapTextARGB", 0), "@perppt",
                       _snapBase[0] + Vector2d(0, sender->displayMmToModel(12.f)), 3.f);
        
        ctx.setFillAlpha(64);
        if (pt1.distanceTo(_snapBase[0]) > r)
            gs->drawCircle(&ctx, pt1, r);
        if (pt2.distanceTo(_snapBase[0]) > r)
            gs->drawCircle(&ctx, pt2, r);
    }
}

bool MgCmdManagerImpl::drawSnap(const MgMotion* sender, GiGraphics* gs) const
{
    bool ret = false;
    
    if (sender->dragging() || !sender->view->useFinger()) {
        if (_snapType[0] >= kMgSnapGrid) {
            const MgShape* sp = sender->view->shapes()->findShape(_snapShapeId);
            if (sp && sp->shapec()->getFlag(kMgNotShowSnap)) {
                return ret;
            }
            
            bool small = (_snapType[0] >= kMgSnapNearPt || _snapType[0] < kMgSnapPoint);
            float r = displayMmToModel(small ? 3.f : 8.f, gs);
            GiContext ctx(-2, GiColor(0, 255, 0, 200), GiContext::kDashLine, GiColor(0, 200, 200, 32));
            
            if (_snapType[0] == kMgSnapPerp || _snapType[0] == kMgSnapPerpNear) {
                ret = gs->drawCircle(&ctx, _snapBase[0], r);
                drawPerpMark(sender, gs, ctx);
            } else if (_snapType[0] == kMgSnapTangent && _snapHandleSrc >= 0) {
                ret = gs->drawCircle(&ctx, _snapBase[0], r);
                GiContext ctxl(0, GiColor(0, 255, 0, 200), GiContext::kDashLine);
                gs->drawLine(&ctxl, _snapBase[0], _guildpt);
                drawPerpMark(gs, GiContext(-2, GiColor(255, 255, 0, 200)), _startpt, _ptSnap,
                             _snapBase[0], _guildpt, displayMmToModel(2.4f, gs));
                gs->drawHandle(_snapBase[0], kGiHandleTangent);
            } else {
                if (_snapType[0] == kMgSnapParallel) {
                    GiContext ctxaux(ctx);
                    const MgShape* sp = sender->view->shapes()->findShape(_snapShapeId);
                    if (sp) {
                        Point2d pt1(sp->getPoint(0)), pt2(sp->getPoint(1));
                        ctxaux.setLineWidth(0, false);
                        gs->drawBeeline(&ctxaux, pt1, pt2);
                        gs->drawHandle((pt1 + pt2) / 2, kGiHandleParallel);
                    }
                }
                ret = gs->drawCircle(&ctx, _ptSnap, r);
                
                GiHandleTypes handleType = snapTypeToHandleType(_snapType[0]);
                const char* names[] = { "@nodept", "@centerpt", "@midpt", "@quadpt", "@tanpt", "@crosspt", "@parallelpt" };
                if (handleType >= kGiHandleNode && handleType - kGiHandleNode < 7) {
                    gs->drawTextAt(sender->view->getOptionInt("snapTextARGB", 0),
                                   names[handleType - kGiHandleNode],
                                   _ptSnap + Vector2d(0, sender->displayMmToModel(10.f)), 3.f);
                }
                gs->drawHandle(_ptSnap, handleType);
                
                if (_snapType[0] == kMgSnapExtendPt) {
                    GiContext ctxExt(0, GiColor(0, 255, 0, 172), GiContext::kDashLine);
                    gs->drawRayline(&ctxExt, _startpt, 2 * _startpt - _guildpt.asVector());
                }
            }
        }
        else {
            GiContext ctx(0, GiColor(0, 255, 0, 200), GiContext::kDashLine, GiColor(0, 255, 0, 64));
            GiContext ctxcross(-2, GiColor(0, 255, 0, 200));
            
            if (_snapType[0] > 0) {
                if (_snapBase[0] == _ptSnap) {
                    if (_snapType[0] == kMgSnapGridX) {
                        Vector2d vec(0, displayMmToModel(12.f, gs));
                        ret = gs->drawLine(&ctxcross, _ptSnap - vec, _ptSnap + vec);
                        gs->drawCircle(&ctx, _snapBase[0], displayMmToModel(4.f, gs));
                    }
                }
                else {  // kMgSnapSameX
                    ret = gs->drawLine(&ctx, _snapBase[0], _ptSnap);
                    gs->drawCircle(&ctx, _snapBase[0], displayMmToModel(2.5f, gs));
                }
            }
            if (_snapType[1] > 0) {
                if (_snapBase[1] == _ptSnap) {
                    if (_snapType[1] == kMgSnapGridY) {
                        Vector2d vec(displayMmToModel(12.f, gs), 0);
                        ret = gs->drawLine(&ctxcross, _ptSnap - vec, _ptSnap + vec);
                        gs->drawCircle(&ctx, _snapBase[1], displayMmToModel(4.f, gs));
                    }
                }
                else {  // kMgSnapSameY
                    ret = gs->drawLine(&ctx, _snapBase[1], _ptSnap);
                    gs->drawCircle(&ctx, _snapBase[1], displayMmToModel(2.5f, gs));
                }
            }
        }
    }
    
    return ret;
}
