//! \file mgsnap.h
//! \brief 定义图形特征点捕捉器接口 MgSnap
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGSNAP_H_
#define TOUCHVG_MGSNAP_H_

struct MgView;
class MgMotion;
class Point2d;
class GiGraphics;
class MgShape;

//! 特征点类型
typedef enum {
    kMgSnapNone,        //!< 无
    kMgSnapSameX,       //!< X方向参考点
    kMgSnapSameY,       //!< Y方向参考点
    kMgSnapGridX,       //!< X方向网格线
    kMgSnapGridY,       //!< Y方向网格线
    kMgSnapGrid,        //!< 网格点
    kMgSnapPoint,       //!< 顶点
    kMgSnapCenter,      //!< 圆心
    kMgSnapMidPoint,    //!< 中点
    kMgSnapQuadrant,    //!< 象限点
    kMgSnapOutPoint,    //!< 线外点
    kMgSnapTangent,     //!< 切点
    kMgSnapIntersect,   //!< 交点
    kMgSnapParallel,    //!< 平行
    kMgSnapPerp,        //!< 垂足
    kMgSnapPerpNear,    //!< 一端为垂足，另一端为线上最近点
    kMgSnapNearPt,      //!< 线上最近点
    kMgSnapExtendPt,    //!< 延长线上的点
} MgSnapType;

//! 捕捉选项位
typedef enum {
    kMgOptionSnapGrid           = 1 << 1,
    kMgOptionStartMustVertex    = 1 << 2,
    kMgOptionSnapVertex         = 1 << 3,
    kMgOptionSnapCenter         = 1 << 4,
    kMgOptionSnapMidPoint       = 1 << 5,
    kMgOptionSnapQuadrant       = 1 << 6,
    kMgOptionSnapCross          = 1 << 7,
    kMgOptionSnapPerp           = 1 << 8,
    kMgOptionSnapPerpOut        = 1 << 9,
    kMgOptionSnapTangent        = 1 << 10,
    kMgOptionSnapNear           = 1 << 11,
    kMgOptionSnapExtend         = 1 << 12,
    kMgOptionSnapParallel       = 1 << 13,
} MgSnapOptions;

//! 图形特征点捕捉器接口
/*! \ingroup CORE_COMMAND
    \interface MgSnap
*/
struct MgSnap {
    virtual ~MgSnap() {}
    
    //! 清除捕捉结果
    virtual void clearSnap(const MgMotion* sender) = 0;
    
    //! 显示捕捉提示线
    virtual bool drawSnap(const MgMotion* sender, GiGraphics* gs) const = 0;
    
    //! 显示垂足标志(90度折线), perp为perp-c与a-b相交的垂足
    virtual bool drawPerpMark(GiGraphics* gs, const GiContext& ctx,
                              const Point2d& a, const Point2d& b,
                              const Point2d& perp, const Point2d& c, float len) const = 0;
    
    //! 返回捕捉选项，由 MgSnapOptions 按位组成
    virtual int getSnapOptions(MgView* view) const = 0;
    
    //! 设置捕捉选项，由 MgSnapOptions 按位组成
    virtual void setSnapOptions(MgView* view, int bits) = 0;
    
    //! 返回捕捉到的特征点类型, >=kMgSnapGrid
    virtual int getSnappedType() const = 0;
    
    //! 得到捕捉到的特征点坐标和原始参考坐标、捕捉坐标
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt) const = 0;
    
    //! 得到捕捉到的特征点坐标和原始参考坐标、捕捉坐标、导向点
    virtual int getSnappedPoint(Point2d& fromPt, Point2d& toPt,
                                Point2d& startPt, Point2d& guildPt) const = 0;
    
    //! 为当前捕捉设置上一线段的坐标，以避免与上一点重合
    virtual void setIgnoreStartPoint(const Point2d& pt) = 0;
    
    //! 根据当前点捕捉新的坐标
    virtual Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt) {
        return snapPoint(sender, orignPt, MgShape::Null(), -1);
    }
    
#ifndef SWIG
    //! 根据当前点捕捉新的坐标
    virtual Point2d snapPoint(const MgMotion* sender, const Point2d& orignPt, const MgShape* shape,
                              int hotHandle, int ignoreHandle = -1, const int* ignoreids = (const int*)0) = 0;
    
    //! 得到捕捉到的图形、控制点序号、源图形上匹配的控制点序号
    virtual bool getSnappedHandle(int& shapeid, int& handleIndex, int& handleIndexSrc) const = 0;
#endif
};

#endif // TOUCHVG_MGSNAP_H_
