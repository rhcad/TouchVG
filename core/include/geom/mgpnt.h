//! \file mgpnt.h
//! \brief 定义二维点类 Point2d
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_POINT_H_
#define TOUCHVG_POINT_H_

#include "mgvec.h"

//! 二维点类
/*!
    本类用于描述二维平面上任意位置的点，具有x和y两个坐标分量
    \ingroup GEOM_CLASS
*/
class Point2d
{
public:
    float   x;      //!< X坐标分量
    float   y;      //!< Y坐标分量
    
    //! 无效点
    static const Point2d kInvalid();

    //! 原点(0,0)
    static const Point2d kOrigin() { return Point2d(); }
    
    //! 构造为原点
    Point2d() : x(0), y(0) {}
    
    //! 构造为点(x, y)
    Point2d(float xx, float yy) {
        x = xx; y = yy;
    }
    
    //! 拷贝构造函数
    Point2d(const Point2d& src) {
        x = src.x; y = src.y;
    }

    //! 矩阵变换, 点 * 矩阵
    Point2d transform(const Matrix2d& m) const;

#ifndef SWIG
    //! 矩阵变换, 点 * 矩阵
    Point2d operator*(const Matrix2d& m) const;

    //! 矩阵变换, 点 *= 矩阵
    Point2d& operator*=(const Matrix2d& m);
    
    //! 矩阵变换, 矩阵 * 点
    friend Point2d operator*(const Matrix2d& m, const Point2d& pnt) {
        return pnt * m;
    }
    
    //! 数 * 点
    friend Point2d operator*(float s, const Point2d& pnt) {
        return Point2d(pnt.x * s, pnt.y * s);
    }
    
    //! 点 * 数
    Point2d operator*(float s) const {
        return Point2d(x * s, y * s);
    }
    
    //! 点 *= 数
    Point2d& operator*=(float s) {
        x *= s; y *= s; return *this;
    }
    
    //! 点 / 数
    Point2d operator/(float s) const {
        s = 1 / s;
        return Point2d(x * s, y * s);
    }
    
    //! 点 /= 数
    Point2d& operator/=(float s) {
        s = 1 / s;
        x *= s; y *= s; return *this;
    }
#endif // SWIG

    //! 比例放缩
    Point2d& scaleBy(float sx, float sy) {
        x *= sx; y *= sy; return *this;
    }

    //! 比例放缩
    Point2d& scaleBy(float s) {
        x *= s; y *= s; return *this;
    }
    
#ifndef SWIG
    //! 点累加
    Point2d operator+(const Point2d& pnt) const {
        return Point2d(x + pnt.x, y + pnt.y);
    }
    
    //! 点累加
    Point2d& operator+=(const Point2d& pnt) {
        x += pnt.x; y += pnt.y; return *this;
    }

    //! 用矢量平移, 矢量 + 点 = 点
    friend Point2d operator+(const Vector2d& vec, const Point2d& pnt) {
        return Point2d(pnt.x + vec.x, pnt.y + vec.y);
    }
    
    //! 用矢量平移, 点 + 矢量 = 点
    Point2d operator+(const Vector2d& vec) const {
        return Point2d(x + vec.x, y + vec.y);
    }
    
    //! 用矢量平移, 点 += 矢量
    Point2d& operator+=(const Vector2d& vec) {
        x += vec.x; y += vec.y; return *this;
    }
    
    //! 用矢量平移, 点 - 矢量 = 点
    Point2d operator-(const Vector2d& vec) const {
        return Point2d(x - vec.x, y - vec.y); 
    }
    
    //! 用矢量平移, 点 -= 矢量
    Point2d& operator-=(const Vector2d& vec) {
        x -= vec.x; y -= vec.y; return *this;
    }
#endif // SWIG
    
    //! 平移
    void offset(float dx, float dy) {
        x += dx; y += dy;
    }

    //! 平移
    void offset(const Vector2d& vec) {
        x += vec.x; y += vec.y;
    }

    //! 点累加
    Point2d add(const Point2d& pnt) const {
        return *this + pnt;
    }

    //! 得到两点之间的矢量, 点 - 点 = 矢量
    Vector2d subtract(const Point2d& pnt) const {
        return *this - pnt;
    }

    //! 用矢量平移, 点 - 矢量 = 点
    Point2d subtract(const Vector2d& vec) const
    {
        return *this - vec;
    }
    
#ifndef SWIG
    //! 得到两点之间的矢量, 点 - 点 = 矢量
    Vector2d operator-(const Point2d& pnt) const {
        return Vector2d(x - pnt.x, y - pnt.y);
    }
    
    //! 返回负矢量
    Vector2d operator-() const {
        return Vector2d(-x, -y);
    }
#endif // SWIG

    //! 返回负矢量
    Vector2d negate() const {
        return Vector2d(-x, -y);
    }
    
    //! 转换为矢量
    Vector2d asVector() const {
        return Vector2d(x, y);
    }

    //! 到原点的距离
    float length() const {
        return mgHypot(x, y);
    }
    
    //! 两点距离
    float distanceTo(const Point2d& pnt) const {
        return mgHypot(x - pnt.x, y - pnt.y);
    }
    
    //! 两点距离的平方
    float distanceSquare(const Point2d& pnt) const {
        return mgSquare(x - pnt.x, y - pnt.y);
    }
    
#ifndef SWIG
    //! 判断两个点是否相等
    bool operator==(const Point2d& pnt) const {
        return isEqualTo(pnt);
    }
    
    //! 判断两个点是否不相等
    bool operator!=(const Point2d& pnt) const {
        return !isEqualTo(pnt);
    }
#endif
    
    //! 用欧拉规则判断两个点是否相等
    /*! 如果两个点的距离小于长度容差，则相等
        \param pnt 比较的另一个点
        \param tol 判断的容差，用到其长度容差
        \return 如果相等，则返回true，否则返回false
    */
    bool isEqualTo(const Point2d& pnt, const Tol& tol = Tol::gTol()) const {
        return mgHypot(x - pnt.x, y - pnt.y) <= tol.equalPoint();
    }
    
    //! 返回某个分量是为NAN越界值
    bool isDegenerate() const { return isnan(x) || isnan(y); }
    
    //! 设置为点(xx, yy)
    Point2d& set(float xx, float yy) {
        x = xx; y = yy; return *this;
    }
    
    //! 设置为新的点
    Point2d& set(const Point2d& pnt) {
        x = pnt.x; y = pnt.y; return *this;
    }
    
#ifndef SWIG
    //! 得到四舍五入后的坐标
    /*! 利用该函数可以将矢量转换为POINT、CPoint值
        \param[out] xx 四舍五入后的X坐标分量
        \param[out] yy 四舍五入后的Y坐标分量
    */
    void get(long& xx, long& yy) const {
        xx = mgRound(x); yy = mgRound(y);
    }
#endif
    
    //! 极坐标法计算坐标
    /*! 
        \param angle 极角，弧度
        \param dist 极径长度
        \return 计算出的坐标
    */
    Point2d polarPoint(float angle, float dist) const {
        return Point2d(x + dist * cosf(angle), y + dist * sinf(angle));
    }
    
    //! 移动直尺法
    /*! 相对于起始点(本点)和方向点，在垂直沿线方向(→↑)偏移指定距离计算坐标
        \param dir 方向点，本点→方向点
        \param yoff 垂直沿线方向偏移，方向点↑偏移
        \return 计算出的坐标
    */
    Point2d rulerPoint(const Point2d& dir, float yoff) const;

    //! 移动直尺法
    /*! 相对于起始点(本点)和方向点，在沿线方向和垂直沿线方向分别偏移计算坐标
        \param dir 方向点，本点→方向点
        \param xoff 在沿线方向(→)偏移
        \param yoff 垂直沿线方向(↑)偏移
        \return 计算出的坐标
    */
    Point2d rulerPoint(const Point2d& dir, float xoff, float yoff) const;
};

#endif // TOUCHVG_POINT_H_
