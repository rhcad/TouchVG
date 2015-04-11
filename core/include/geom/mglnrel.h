//! \file mglnrel.h
//! \brief 定义图形位置关系函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_LINEREL_H_
#define TOUCHVG_LINEREL_H_

#include "mgbox.h"

//! 图形位置关系函数
/*! \ingroup GEOMAPI
*/
struct mglnrel {

//! 判断点pt是否在有向直线a->b的左边 (开区间)
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线的左边时返回true，否则返回false
*/
static bool isLeft(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在有向直线a->b的左边
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线的左边时返回true，否则返回false
*/
static bool isLeft2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断点pt是否在有向直线a->b的左边或线上 (闭区间)
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线的左边或线上时返回true，否则返回false
*/
static bool isLeftOn(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在有向直线a->b的左边或线上
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线的左边或线上时返回true，否则返回false
*/
static bool isLeftOn2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断点pt是否在直线a->b的线上
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \return 在直线上时返回true，否则返回false
*/
static bool isColinear(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在直线a->b的线上
/*!
    \param[in] a 直线的起点
    \param[in] b 直线的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在直线上时返回true，否则返回false
*/
static bool isColinear2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 判断两个线段ab和cd是否相交于线段内部
/*!
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \return 是否相交于线段内部，不包括端点
*/
static bool isIntersectProp(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d);

//! 判断点pt是否在线段ab上(闭区间)
/*!
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \return 在线段上时返回true，否则返回false
*/
static bool isBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否投影在线段ab上(闭区间)
static bool isProjectBetweenLine(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否投影在射线a->b上(闭区间)
static bool isProjectBetweenRayline(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 判断点pt是否在线段ab上
/*!
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[in] tol 判断的容差，用到其中的长度容差值
    \return 在线段上时返回true，否则返回false
*/
static bool isBetweenLine2(
    const Point2d& a, const Point2d& b, const Point2d& pt, const Tol& tol);

//! 已知点pt在直线ab上, 判断点pt是否在线段ab上(闭区间)
/*!
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[out] nearpt 在线段的两个端点中，到给定的点最近的端点，为NULL则忽略该参数
    \return 在线段上时返回true，否则返回false
*/
static bool isBetweenLine3(
    const Point2d& a, const Point2d& b, const Point2d& pt, Point2d* nearpt = (Point2d*)0);

//! 判断两个线段ab和cd是否相交(交点在线段闭区间内)
/*!
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \return 是否相交于线段闭区间内，包括端点
*/
static bool isIntersect(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d);

//! 计算点pt到无穷直线ab的距离
/*!
    \param[in] a 无穷直线的起点
    \param[in] b 无穷直线的终点
    \param[in] pt 给定的测试点
    \return 点到无穷直线的距离
*/
static float ptToBeeline(const Point2d& a, const Point2d& b, const Point2d& pt);

//! 计算点pt到无穷直线ab的距离
/*!
    \param[in] a 无穷直线的起点
    \param[in] b 无穷直线的终点
    \param[in] pt 给定的测试点
    \param[out] ptPerp 直线上的垂足
    \return 独立点到垂足的距离
*/
static float ptToBeeline2(
    const Point2d& a, const Point2d& b, Point2d pt, Point2d& ptPerp);

//! 计算点pt到线段ab的最近距离
/*!
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] pt 给定的测试点
    \param[out] nearpt 线段上的最近点
    \return 独立点到最近点的距离
*/
static float ptToLine(
    const Point2d& a, const Point2d& b, Point2d pt, Point2d& nearpt);

//! 求两条直线(ax+by+c=0)的交点
/*!
    \param[in] a1 第一条直线的标准方程参数A
    \param[in] b1 第一条直线的标准方程参数B
    \param[in] c1 第一条直线的标准方程参数C
    \param[in] a2 第二条直线的标准方程参数A
    \param[in] b2 第二条直线的标准方程参数B
    \param[in] c2 第二条直线的标准方程参数C
    \param[out] ptCross 交点
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
static bool crossLineAbc(
    float a1, float b1, float c1, float a2, float b2, float c2,
    Point2d& ptCross, const Tol& tolVec = Tol::gTol());

#ifndef SWIG
//! 求两条无穷直线的交点
/*!
    \param[in] a 第一条直线的起点
    \param[in] b 第一条直线的终点
    \param[in] c 第二条直线的起点
    \param[in] d 第二条直线的终点
    \param[out] ptCross 交点
    \param[out] pu 交点在第一条直线上的参数，如果为NULL则忽略该参数
    \param[out] pv 交点在第二条直线上的参数，如果为NULL则忽略该参数
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
static bool cross2Beeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d, 
    Point2d& ptCross, float* pu = (float*)0, float* pv = (float*)0, 
    const Tol& tolVec = Tol::gTol());
#endif

//! 求两条线段的交点
/*!
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \param[out] ptCross 交点
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
static bool cross2Line(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, const Tol& tolVec = Tol::gTol());

#ifndef SWIG
//! 求两条线段的交点
/*!
    \param[in] a 第一条线段的起点
    \param[in] b 第一条线段的终点
    \param[in] c 第二条线段的起点
    \param[in] d 第二条线段的终点
    \param[out] v1 交点在第一条线段上的参数，如果为NULL则忽略该参数
    \param[out] v2 交点在第二条线段上的参数，如果为NULL则忽略该参数
    \return 是否有交点
*/
static bool cross2LineV(const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
                        float* v1 = (float*)0, float* v2 = (float*)0);

//! 求线段和直线的交点
/*!
    \param[in] a 线段的起点
    \param[in] b 线段的终点
    \param[in] c 直线的起点
    \param[in] d 直线的终点
    \param[out] ptCross 交点
    \param[out] pv 交点在直线上的参数，如果为NULL则忽略该参数
    \param[in] tolVec 判断平行的容差，用到其矢量容差值
    \return 是否有交点
*/
static bool crossLineBeeline(
    const Point2d& a, const Point2d& b, const Point2d& c, const Point2d& d,
    Point2d& ptCross, float* pv = (float*)0, 
    const Tol& tolVec = Tol::gTol());
#endif

//! 用矩形剪裁线段，Sutherland-Cohen算法
/*!
    \param[in] pt1 线段的起点
    \param[in] pt2 线段的终点
    \param[in] box 剪裁矩形，必须为非空规范化矩形
    \return 剪裁后是否有处于剪裁矩形内的线段部分
*/
static bool clipLine(Point2d& pt1, Point2d& pt2, const Box2d& box);

#ifndef SWIG
//! ptInArea 的返回值枚举定义
//! \see ptInArea
typedef enum {
    kPtInArea,          //!< 在多边形内
    kPtOutArea,         //!< 在多边形外
    kPtOnEdge,          //!< 在第order边上
    kPtAtVertex,        //!< 与第order顶点重合
} PtInAreaRet;

//! 判断一点是否在一多边形范围内
/*!
    \param[in] pt 给定的测试点
    \param[in] count 多边形的顶点数
    \param[in] vertexs 多边形的顶点数组
    \param[out] order 返回 kPtAtVertex 时，输出顶点号[0, count-1]；\n
        返回 kPtOnEdge 时，输出边号[0, count-1]；\n为NULL则忽略该参数
    \param[in] tol 容差
    \param[in] closed 是否闭合，false表示折线
    \param[in] flags 允许检测的位，{1<<PtInAreaRet}
    \param[in] ignoreVertex 不捕捉的顶点序号
    \return 为枚举定义 PtInAreaRet 的各种值
*/
static int ptInArea(
    const Point2d& pt, int count, const Point2d* vertexs, 
    int& order, const Tol& tol = Tol::gTol(), bool closed = true,
    int flags = -1, int ignoreVertex = -1);

//! 判断多边形是否为凸多边形
/*!
    \param[in] count 顶点个数
    \param[in] vertexs 顶点数组
    \param[out] acw 多边形是否为逆时针方向，为NULL则忽略该参数
    \return 是否为凸多边形
*/
static bool isConvex(int count, const Point2d* vertexs, bool* acw = (bool*)0);

#endif
};

#endif // TOUCHVG_LINEREL_H_
