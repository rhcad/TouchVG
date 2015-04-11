//! \file mgcurv.h
//! \brief 定义曲线计算函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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

//! 计算三次贝塞尔曲线段的切线点
/*!
    \param[in] pts 4个点的数组，为贝塞尔曲线段的控制点
    \param[in] t 要计算的参数点的参数，范围为[0, 1]
    \param[out] outpt 参数点处的切线终点
    \see fitBezier
*/
static void bezierTanget(const Point2d* pts, float t, Point2d& outpt);

//! 分割一个三次贝塞尔曲线段
/*!
    \param[in] pts 4个点的数组，为贝塞尔曲线段的控制点
    \param[in] t 要分割点的参数，范围为(0, 1)
    \param[out] pts1 传入4个点的数组，填充分割出的第一个贝塞尔曲线段
    \param[out] pts2 传入4个点的数组，填充分割出的第二个贝塞尔曲线段
*/
static void splitBezier(const Point2d* pts, float t, Point2d* pts1, Point2d* pts2);

//! 返回一个三次贝塞尔曲线段是否为直线段
static bool bezierIsStraight(const Point2d* pts);

//! 返回一个三次贝塞尔曲线段的长度
static float lengthOfBezier(const Point2d* pts);

//! 返回三次贝塞尔曲线段从起点开始的指定弧长处的点参数，范围为[0,1]
static float bezierPointLengthFromStart(const Point2d* pts, float len);

#ifndef SWIG
//! 计算曲线起点附近的外部点到三次贝塞尔曲线段相距指定距离的曲线点和参数
static bool bezierPointDistantFromPoint(const Point2d* pts, float dist, const Point2d& pt,
                                        Point2d &result, float &tRes);

//! 计算曲线段与给定的直线段ab相交处的参数
static bool bezierIntersectionWithLine(const Point2d* pts, const Point2d& a, const Point2d& b, float &tIntersect);
#endif

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
static void bezier4P(const Point2d& pt1, const Point2d& pt2, const Point2d& pt3,
                     const Point2d& pt4, Point2d& ctrpt1, Point2d& ctrpt2);

//! 对数据点光滑拟合为三次贝塞尔曲线
/*!
    \param[in] knotCount 缓冲区knots和knotvs的元素个数
    \param[out] knots 贝塞尔曲线的顶点，元素个数为knotCount
    \param[out] knotvs 顶点处的切向矢量，可为NULL
    \param[in] count 数据点pts的个数
    \param[in] pts 数据点
    \param[in] tol 拟合曲线与数据点的最大允许距离
    \return knots和knotvs的结果元素个数，不超过knotCount
*/
static int fitCurve(int knotCount, Point2d* knots, Vector2d* knotvs,
                    int count, const Point2d* pts, float tol);

#ifndef SWIG
typedef Point2d (*PtCallback)(void* data, int i);
static int fitCurve2(int knotCount, Point2d* knots, int count, PtCallback pts, void* data, float tol);

typedef void (*FitCubicCallback)(void* data, const Point2d curve[4]);
static void fitCurve3(FitCubicCallback fc, void* data, const Point2d *pts, int n, float tol);
static void fitCurve4(FitCubicCallback fc, void* data, PtCallback pts, void* data2, int n, float tol);
#endif

//! 二次贝塞尔曲线段转为三次贝塞尔曲线段
static void quadBezierToCubic(const Point2d quad[3], Point2d cubic[4]);

//! 用给定的起点和终点构造90度椭圆弧，并转换为一个三次贝塞尔曲线段
/*! 椭圆弧为从起点到终点逆时针转90度。
    计算出的贝塞尔曲线段的起点为pt1、终点为pt4，中间两个控制点为ctrpt1、ctrpt2
    \param[in] frompt 起点
    \param[in] topt 终点
    \param[out] ctrpt1 中间第一个控制点
    \param[out] ctrpt2 中间第二个控制点
    \see bezier4P, ellipseToBezier, arcToBezier
*/
static void ellipse90ToBezier(const Point2d& frompt, const Point2d& topt,
                              Point2d& ctrpt1, Point2d& ctrpt2);

//! 将一个椭圆转换为4段三次贝塞尔曲线
/*! 4段三次贝塞尔曲线是按逆时针方向从第一象限到第四象限连接，
    第一个点和最后一个点重合于+X轴上点(rx, 0)。
    \param[out] points 贝塞尔曲线的控制点，13个点
    \param[in] center 椭圆心
    \param[in] rx 半长轴的长度
    \param[in] ry 半短轴的长度
    \see bezier4P, ellipse90ToBezier, arcToBezier
*/
static void ellipseToBezier(Point2d points[13], const Point2d& center, float rx, float ry);

//! 将一个圆角矩形转换为4段三次贝塞尔曲线
/*! 这4段贝塞尔曲线按逆时针方向从第一象限到第四象限，每段4个点，
    第一段的点序号为[0,1,2,3]，其余段递增类推。
    \param[out] points 贝塞尔曲线的控制点，每段4个点，共16个点
    \param[in] rect 矩形外框，规范化矩形
    \param[in] rx X方向的圆角半径，非负数
    \param[in] ry Y方向的圆角半径，非负数
    \see ellipseToBezier
*/
static void roundRectToBeziers(Point2d points[16], const Box2d& rect, float rx, float ry);

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
static int arcToBezier(Point2d points[16], const Point2d& center,
                       float rx, float ry, float startAngle, float sweepAngle);

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
static bool arc3P(const Point2d& start, const Point2d& point, const Point2d& end,
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
static bool arcTan(const Point2d& start, const Point2d& end, const Vector2d& startTan,
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
static bool arcBulge(const Point2d& start, const Point2d& end, float bulge,
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
static int crossTwoCircles(Point2d& pt1, Point2d& pt2,
                           const Point2d& c1, float r1, const Point2d& c2, float r2);

//! 计算无穷直线(ab)与圆(cr)的交点，返回交点数
static int crossLineCircle(Point2d& pt1, Point2d& pt2,
                           const Point2d& a, const Point2d& b, const Point2d& c, float r, bool ray = false);

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
static bool triEquations(int n, float *a, float *b, float *c, Vector2d *vs);

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

//! 计算三次参数样条曲线(Hermite)的型值点的切矢量
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
static bool cubicSplines(int n, const Point2d* knots, Vector2d* knotvs,
                         int flag = 0, float tension = 1);

//! 在三次样条曲线(Hermite)的一条弦上插值得到拟和点坐标
/*!
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotvs 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[in] t 分段曲线函数参数，在0到1之间
    \param[out] fitpt 拟和点坐标，第i段曲线上参数t对应的曲线坐标
    \see cubicSplines, cubicSplineToBezier
*/
static void fitCubicSpline(int n, const Point2d* knots, const Vector2d* knotvs,
                           int i, float t, Point2d& fitpt);

//! 得到三次样条曲线的分段贝塞尔曲线段控制点
/*!
    \param[in] n 三次样条曲线的型值点的点数
    \param[in] knots 型值点坐标数组，元素个数为n
    \param[in] knotvs 型值点的切矢量数组，元素个数为n
    \param[in] i 分段曲线序号，在0到(n-2)之间，如果曲线是闭合条件，则可取到(n-1)
    \param[out] points 贝塞尔曲线段的控制点，4个点
    \param[in] hermite 是否为Hermite曲线，不是则切矢加型值点直接形成Bezier段的控制点
    \see cubicSplines, fitCubicSpline
*/
static void cubicSplineToBezier(int n, const Point2d* knots, const Vector2d* knotvs,
                                int i, Point2d points[4], bool hermite = true);

//! 得到三次B样条曲线的分段贝塞尔曲线段控制点
/*!
    \param[out] points 贝塞尔曲线的控制点，要预先分配(1+n*3)个点的空间
    \param n B样条曲线控制点的点数，至少为4
    \param ctlpts B样条曲线控制点坐标数组，点数为n
    \param closed 三次B样条曲线是否为闭合曲线
    \return 实际转换的贝塞尔曲线控制点的个数
*/
static int bsplinesToBeziers(Point2d points[/*1+n*3*/], int n, const Point2d* ctlpts, bool closed);
};

#endif // TOUCHVG_FITCURVE_H_
