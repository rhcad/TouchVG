//! \file mgdef.h
//! \brief 定义数学几何库的常量和宏
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef __GEOMETRY_MGDEF_H_
#define __GEOMETRY_MGDEF_H_

#include <math.h>

#ifdef _MSC_VER
#pragma warning(disable: 4514)  // unreferenced inlines are common
#pragma warning(disable: 4710)  // private constructors are disallowed
#pragma warning(disable: 4251)  // using non-exported as public in exported
#endif

#ifndef isnan
#define isnan(x) ((x)!=(x))
#endif

const float _MGZERO    = 2e-6f;     //!< 定义可以视为零的极小数

// 定义常用的数学常数
const float _M_E       = 2.71828182845904523536028747135266f;      //!< e
const float _M_LOG2E   = 1.4426950408889634073599246810019f;       //!< log2(e)
const float _M_LOG10E  = 0.434294481903251827651128918916605f;     //!< log(e)
const float _M_LN2     = 0.693147180559945309417232121458177f;     //!< ln(2)
const float _M_LN10    = 2.30258509299404568401799145468436f;      //!< ln(10)
const float _M_PI      = 3.14159265358979323846264338327950288419716939937511f;  //!< 圆周率PI，180°
const float _M_PI_2    = 1.57079632679489661923132169163975144209858469968756f;  //!< PI/2，90°
const float _M_PI_4    = 0.785398163397448309615660845819875721049292349843778f; //!< PI/4，45°
const float _M_1_PI    = 0.318309886183790671537767526745028724068919291480913f; //!< 1/PI
const float _M_2_PI    = 0.636619772367581343075535053490057448137838582961826f; //!< 2/PI
const float _M_2_SQRTPI = 1.12837916709551257389615890312155f;                   //!< 2/sqrt(PI)
const float _M_SQRT2   = 1.41421356237309504880168872420969807856967187537695f;  //!< sqrt(2)
const float _M_SQRT1_2 = 0.707106781186547524400844362104849039284835937688474f; //!< sqrt(2)/2

const float _M_2PI     = 6.28318530717958647692528676655900576839433879875022f;  //!< 2PI，360°
const float _M_PI_3    = 1.04719755119659774615421446109316762806572313312504f;  //!< PI/3，60°
const float _M_PI_6    = 0.523598775598298873077107230546583814032861566562518f; //!< PI/6，30°
const float _M_D2R     = 0.0174532925199432957692369076848861271344287188854173f; //!< 度到弧度的转换常数，PI/180
const float _M_R2D     = 57.2957795130823208767981548141051703324054724665643f;  //!< 弧度到度的转换常数，180/PI
const float _M_1_SQRPI = 0.564189583547756286948079451560773f;                   //!< 1/sqrt(PI)

//const double _DBL_MIN  = 2.2250738585072014e-308;   //!< 双精度浮点数最小绝对值
//const double _DBL_MAX  = 1.7976931348623158e+308;   //!< 双精度浮点数最大绝对值
const float  _FLT_MIN  = 1.175494351e-38F;          //!< 单精度浮点数最小绝对值
const float  _FLT_MAX  = 3.402823466e+38F;          //!< 单精度浮点数最大绝对值

//! 判断一个浮点数是否可以认为是零
/*! 判断两个浮点数是否相等不能直接比较相等，应该用mgIsZero(d1-d2)判断
    \ingroup GEOMAPI
    \see _MGZERO, mgEquals
*/
template<class _T> inline bool mgIsZero(_T v)
{
    return fabs(v) < _MGZERO;
}

//! 判断两个浮点数是否相等
/*!
    \ingroup GEOMAPI
    \see mgIsZero
*/
template<class _T> inline bool mgEquals(_T v1, _T v2)
{
    return fabs(v1 - v2) < _MGZERO;
}

//! 返回两个同类型的数中较小的数的引用
//! \ingroup GEOMAPI
template<class _T> inline
const _T& mgMin(const _T& t1, const _T& t2)
{
    return (t1 < t2) ? t1 : t2;
}

//! 返回两个同类型的数中较大的数的引用
//! \ingroup GEOMAPI
template<class _T> inline
const _T& mgMax(const _T& t1, const _T& t2)
{
    return (t1 > t2) ? t1 : t2;
}

//! 交换两个同类型的数
//! \ingroup GEOMAPI
template<class _T> inline void mgSwap(_T& t1, _T& t2)
{
    _T tmp = t1; t1 = t2; t2 = tmp;
}

//! 两个同类型的数的赋值
//! \ingroup GEOMAPI
template<class _T> inline bool mgCopy(_T& t1, _T t2)
{
    bool changed = !mgEquals(t1, t2); if (changed) t1 = t2; return changed;
}

//! 求直角三角形的斜边长度
/*! 不用math.h中的hypot函数是因为在不溢出的情况下该函数执行快些
    \ingroup GEOMAPI
*/
template<class _T> inline _T mgHypot(_T x, _T y)
{
    return (_T)sqrt(x*x + y*y);
}

//! 求两个数的平方和
//! \ingroup GEOMAPI
template<class _T> inline _T mgSquare(_T x, _T y)
{
    return (x*x + y*y);
}

//! 对一个浮点数四舍五入到整数
/*! 对负数是按其绝对值进行舍入的
    \ingroup GEOMAPI
    \see mgbase::roundReal
*/
template<class _T> inline int mgRound(_T d)
{
    return d < 0 ? -(int)(-d + 0.5f) : (int)(d + 0.5f);
}

#endif // __GEOMETRY_MGDEF_H_
