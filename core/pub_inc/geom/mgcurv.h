//! \file mgcurv.h
//! \brief 定义曲线计算函数
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_FITCURVE_H_
#define TOUCHVG_FITCURVE_H_

#include "mgbox.h"

//! 曲线计算函数
/*! \ingroup GEOMAPI
*/
struct mgcurv {

//! 计算三次贝塞尔曲线段的参数点
/*!
    \param[in] pts 4个点的数组，为贝塞尔曲线段的控制点
    \param[in] t 要计算的参数点的参数，范围为[0, 1]
    \param[out] fitpt 计算出的参数点
    \see bezier4P, cubicSplines
*/
static void fitBezier(const Point2d* pts, float t, Point2d& fitpt);

//! 计算三次贝塞尔曲线段的参数点
/*!
    \param[in] pts 4个点的数组，为贝塞尔曲线段的控制点
    \param[in] t 要分割点的参数，范围为(0, 1)
    \param[out] pts1 分割出的第一个贝塞尔曲线段，4个点的数组
    \param[out] pts2 分割出的第二个贝塞尔曲线段，4个点的数组
*/
static void splitBezier(const Point2d* pts, float t, Point2d* pts1, Point2d* pts2);

//! 返回三次贝塞尔曲线段的长度
static float lengthOfBezier(const Point2d* pts, float tol);
    
//! 用线上四点构成三次贝塞尔曲线段
/*! 该贝塞尔曲线段的起点和终点为给定点，中间经过另外两个给定点，
    t=1/3过pt2, t=2/3过pt3。
    计算出的贝塞尔曲线段的起点为pt1、终点为pt4，中间两个控制点为ctrpt1、ctrpt2
    \param[in] pt1 线的起点
    \param[in] pt2 线上的第一个中间点
    \param[in] pt3 线上的第二个中间点
    \param[in] pt4 线的终点
    \param[out] ctrpt1 中间第一个控制点
    \param[out] ctrpt2 中间第二个控制点
    \see ellipse90ToBezier, ellipseToBezier, arcToBezier
*/
static void bezier4P(
    const Point2d& pt1, const Point2d& pt2, const Point2d& pt3, 
    const Point2d& pt4, Point2d& ctrpt1, Point2d& ctrpt2);

//! 用给定的起点和终点构造90度椭圆弧，并转换为一个三次贝塞尔曲线段
/*! 椭圆弧为从起点到终点逆时针转90度。
    计算出的贝塞尔曲线段的起点为pt1、终点为pt4，中间两个控制点为ctrpt1、ctrpt2
    \param[in] frompt 起点
    \param[in] topt 终点
    \param[out] ctrpt1 中间第一个控制点
    \param[out] ctrpt2 中间第二个控制点
    \see bezier4P, ellipseToBezier, arcToBezier
*/
static void ellipse90ToBezier(
    const Point2d& frompt, const Point2d& topt, Point2d& ctrpt1, Point2d& ctrpt2);

//! 将一个椭圆转换为4段三次贝塞尔曲线
/*! 4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，
    第一个点和最后一个点重合于+X轴上点(rx, 0)。
    \param[out] points 贝塞尔曲线的控制点，13个点
    \param[in] center 椭圆心
    \param[in] rx 半长轴的长度
    \param[in] ry 半短轴的长度
    \see bezier4P, ellipse90ToBezier, arcToBezier
*/
static void ellipseToBezier(
    Point2d points[13], const Point2d& center, float rx, float ry);

//! 将一个圆角矩形转换为4段三次贝塞尔曲线
/*! 这4段贝塞尔曲线按逆时针方向从第一象限到第四象限，每段4个点，
    第一段的点序号为[0,1,2,3]，其余段递增类推。
    \param[out] points 贝塞尔曲线的控制点，每段4个点，共16个点
    \param[in] rect 矩形外框，规范化矩形
    \param[in] rx X方向的圆角半径，非负数
    \param[in] ry Y方向的圆角半径，非负数
    \see ellipseToBezier
*/
static void roundRectToBeziers(
    Point2d points[16], const Box2d& rect, float rx, float ry);

//! 将一个椭圆弧转换为多段三次贝塞尔曲线
/*! 4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，每一段4个点，
    第一个点和最后一个点重合于+X轴上点(rx, 0)。
    \param[out] points 贝塞尔曲线的控制点，16个点
    \param[in] center 椭圆心
    \param[in] rx 半长轴的长度
    \param[in] ry 半短轴的长度，为0则取为rx
    \param[in] startAngle 起始角度，弧度，相对于+X轴，逆时针为正
    \param[in] sweepAngle 转角，弧度，相对于起始角度，逆时针为正
    \return 计算后的控制点数，点数小于4则给定参数有错误
    \see bezier4P, ellipse90ToBezier, ellipseToBezier, arc3P
*/
static int arcToBezier(
    Point2d points[16], const Point2d& center, float rx, float ry,
    float startAngle, float sweepAngle);

#ifndef SWIG
//! 给定起点、弧上一点和终点，计算圆弧参数
/*!
    \param[in] start 圆弧的起点
    \param[in] point 弧上的一点
    \param[in] end 圆弧的终点
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see arcTan, arcBulge, arcToBezier
*/
static bool arc3P(
    const Point2d& start, const Point2d& point, const Point2d& end,
    Point2d& center, float& radius,
    float* startAngle = (float*)0, float* sweepAngle = (float*)0);

//! 给定起点、终点和起点切向，计算圆弧参数
/*!
    \param[in] start 圆弧的起点
    \param[in] end 圆弧的终点
    \param[in] startTan 圆弧的起点处的切向矢量
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see arc3P, arcBulge, arcToBezier
*/
static bool arcTan(
    const Point2d& start, const Point2d& end, const Vector2d& startTan,
    Point2d& center, float& radius,
    float* startAngle = (float*)0, float* sweepAngle = (float*)0);

//! 给定弦和拱高计算圆弧参数
/*!
    \param[in] start 圆弧的起点
    \param[in] end 圆弧的终点
    \param[in] bulge 拱高，正数为逆时针方向，负数为顺时针方向
    \param[out] center 圆心
    \param[out] radius 半径
    \param[out] startAngle 填充起始角度，为NULL则忽略该参数
    \param[out] sweepAngle 填充圆弧转角，逆时针为正，为NULL则忽略该参数
    \return 是否计算成功
    \see arc3P, arcTan, arcToBezier
*/
static bool arcBulge(
    const Point2d& start, const Point2d& end, float bulge,
    Point2d& center, float& radius,
    float* startAngle = (float*)0, float* sweepAngle = (float*)0);
#endif

//! 计算两圆的交点
/*!
    \param[out] pt1 第一个交点，返回为1或2时有效
    \param[out] pt2 第二个交点，返回为2时有效
    \param[in] c1 第一个圆的圆心
    \param[in] r1 第一个圆的半径，正数
    \param[in] c2 第二个圆的圆心
    \param[in] r2 第二个圆的半径，正数
    \return 交点个数，-1:无穷个，0：没有交点，1：相切，2：两个交点相交
*/
static int insectTwoCircles(Point2d& pt1, Point2d& pt2,
                               const Point2d& c1, float r1, const Point2d& c2, float r2);

#ifndef SWIG
//! 求解三对角线方程组
/*! 三对角线方程组如下所示: \n
    　　　| b0　　　c0　　　　　　| \n
    A　=　| a0　　　b1　　　c1　　| \n
    　　　|　　..　　　..　　.. 　| \n
    　　　|　　　a[n-2]　　b[n-1] | \n
    A * (x,y) = (rx,ry)

    \param[in] n 方程组阶数，最小为2
    \param[in] a 系数矩阵中的左对角线元素数组，a[0..n-2]
    \param[in,out] b 系数矩阵中的中对角线元素数组，b[0..n-1]，会被修改
    \param[in] c 系数矩阵中的右对角线元素数组，c[0..n-2]
    \param[in,out] vs 输入方程组等号右边的已知n个矢量，输出求解出的未知矢量
    \return 是否求解成功，失败原因可能是参数错误或因系数矩阵非主角占优而出现除零
    \see gaussJordan
*/
static bool triEquations(
    int n, float *a, float *b, float *c, Vector2d *vs);

//! Gauss-Jordan法求解线性方程组
/*!
    \param[in] n 方程组阶数，最小为2
    \param[in,out] mat 系数矩阵，n维方阵，会被修改
    \param[in,out] vs 输入方程组等号右边的已知n个矢量，输出求解出的未知矢量
    \return 是否求解成功，失败原因可能是参数错误或因系数矩阵非主角占优而出现除零
    \see triEquations
*/
static bool gaussJordan(int n, float *mat, Vector2d *vs);
#endif

//! 三次参数样条曲线的端点条件
//! \see cubicSplines
typedef enum {
    cubicTan1 = 1,         //!< 起始夹持端
    cubicArm1 = 2,         //!< 起始悬臂端
    cubicTan2 = 4,         //!< 终止夹持端
    cubicArm2 = 8,         //!< 终止悬臂端
    cubicLoop = 16,        //!< 闭合, 有该值时忽略其他组合值
} SplineFlags;

//! 计算三次参数样条曲线的型值点的切矢量
/*! 三次参数样条曲线的分段曲线方程为：\n
    P[i](t) = knots[i] + knotvs[i] * t \n
    　　　+ (3*(knots[i+1] - knots[i]) - 2 * knotvs[i] - knotvs[i+1]) * t^2 \n
    　　　+ (2*(knots[i] - knots[i+1]) + knotvs[i] + knotvs[i+1]) * t^3 \n
    其中 0 ≤ t ≤ 1

    \param[in] n 型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[out] knotvs 型值点的切矢量数组，元素个数为n，由外界分配内存
    \param[in] flag 曲线边界条件，由 MgCubicSplineFlag 各种值组合而成。\n
        指定 cubicTan1 时, knotvs[0]必须指定有效的切矢量；\n
        指定 cubicTan2 时, knotvs[n-1]必须指定有效的切矢量。\n
        指定 cubicLoop 时，knots的首末型值点不必重合，计算中将首末型值点视为任意两点。
    \param[in] tension 张力系数，0≤coeff≤1, 为1时C2阶连续, 为0时成折线
    \return 是否计算成功
    \see MgCubicSplineFlag, fitCubicSpline, cubicSplinesBox
*/
static bool cubicSplines(
    int n, const Point2d* knots, Vector2d* knotvs,
    int flag = 0, float tension = 1);

//! 在三次样条曲线的一条弦上插值得到拟和点坐标
/*!
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotvs 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[in] t 分段曲线函数参数，在0到1之间
    \param[out] fitpt 拟和点坐标，第i段曲线上参数t对应的曲线坐标
    \see cubicSplines, cubicSplineToBezier
*/
static void fitCubicSpline(
    int n, const Point2d* knots, const Vector2d* knotvs,
    int i, float t, Point2d& fitpt);

//! 得到三次样条曲线的分段贝塞尔曲线段控制点
/*!
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotvs 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[out] points 贝塞尔曲线段的控制点，4个点
    \see cubicSplines, fitCubicSpline
*/
static void cubicSplineToBezier(
    int n, const Point2d* knots, const Vector2d* knotvs,
    int i, Point2d points[4]);

//! 得到三次B样条曲线的分段贝塞尔曲线段控制点
/*!
    \param[out] points 贝塞尔曲线的控制点，要预先分配(1+n*3)个点的空间
    \param n B样条曲线控制点的点数，至少为4
    \param ctlpts B样条曲线控制点坐标数组，点数为n
    \param closed 三次B样条曲线是否为闭合曲线
    \return 实际转换的贝塞尔曲线控制点的个数
*/
static int bsplinesToBeziers(
    Point2d points[/*1+n*3*/], int n, const Point2d* ctlpts, bool closed);

#ifndef SWIG
//! 计算张力样条曲线的型值点参数和弦长
/*!
    \param[in,out] n 型值点的点数，如果有重合点则输出实际点数
    \param[in,out] knots 型值点坐标数组，元素个数为n，有重合点则会删除点坐标
    \param[in] sgm 控制参数，>0，一般取1.5
    \param[in] tol 长度容差值，用于判断重合点
    \param[out] sigma 规范化张力系数，= 控制参数 / 平均弦长
    \param[out] hp 弦长数组，元素个数为n-1或n，由外界分配内存
    \param[out] knotvs 型值点的f"(x_i)/sigma^2，元素个数为n，由外界分配内存
    \return 是否计算成功
    \see fitClampedSpline
*/
static bool clampedSplines(
    int& n, Point2d* knots, float sgm, float tol, float& sigma,
    float* hp, Vector2d* knotvs);

//! 在张力样条曲线的一条弦上插值得到拟和点坐标
/*!
    \param[in] knots 型值点坐标数组
    \param[in] i 分段曲线序号，在0到(型值点的点数-1)之间
    \param[in] t 分段曲线函数参数，在0到hp[i]之间
    \param[in] sigma 规范化张力系数
    \param[in] hp 弦长数组
    \param[in] knotvs 型值点的f"(x_i)/sigma^2数组
    \param[out] fitpt 拟和点坐标，第i段曲线上参数t对应的曲线坐标
    \see clampedSplines
*/
static void fitClampedSpline(
    const Point2d* knots, int i, float t, float sigma,
    const float* hp, const Vector2d* knotvs, Point2d& fitpt);
#endif
};

#endif // TOUCHVG_FITCURVE_H_
