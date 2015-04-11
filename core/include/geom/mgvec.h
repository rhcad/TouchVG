//! \file mgvec.h
//! \brief 定义二维矢量类 Vector2d
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_VECTOR_H_
#define TOUCHVG_VECTOR_H_

#include "mgtol.h"

class Matrix2d;

//! 二维矢量类
/*!
    本类用于描述二维平面上具有方向和长度的矢量，具有x和y两个坐标分量
    \ingroup GEOM_CLASS
*/
class Vector2d
{
public:
    float   x;      //!< X坐标分量
    float   y;      //!< Y坐标分量

    //! 零矢量(0,0)
    static const Vector2d kIdentity() { return Vector2d(0, 0); }
    
    //! X轴单位矢量(1,0)
    static const Vector2d kXAxis() { return Vector2d(1, 0); }
    
    //! Y轴单位矢量(0,1)
    static const Vector2d kYAxis() { return Vector2d(0, 1); }
    
    //! 构造为零矢量
    Vector2d() : x(0), y(0) {}
    
    //! 构造为矢量(xx, yy)
    Vector2d(float xx, float yy) {
        x = xx; y = yy;
    }
    
    //! 拷贝构造函数
    Vector2d(const Vector2d& src) {
        x = src.x; y = src.y;
    }

    //! 矩阵变换, 矢量 * 矩阵
    Vector2d transform(const Matrix2d& m) const;

#ifndef SWIG
    //! 矩阵变换, 矢量 * 矩阵
    Vector2d operator*(const Matrix2d& m) const;

    //! 矩阵变换, 矢量 *= 矩阵
    Vector2d& operator*=(const Matrix2d& m);

    //! 矩阵变换, 矩阵 * 矢量
    friend Vector2d operator*(const Matrix2d& m, const Vector2d& v) {
        return v * m;
    }
    
    //! 矢量加法
    Vector2d operator+(const Vector2d& v) const {
        return Vector2d(x + v.x, y + v.y);
    }
    
    //! 矢量加法
    Vector2d& operator+=(const Vector2d& v) {
        x += v.x; y += v.y; return *this;
    }
    
    //! 矢量减法
    Vector2d operator-(const Vector2d& v) const {
        return Vector2d(x - v.x, y - v.y);
    }
    
    //! 矢量减法
    Vector2d& operator-=(const Vector2d& v) {
        x -= v.x; y -= v.y; return *this;
    }
    
    //! 返回负矢量
    Vector2d operator-() const {
        return Vector2d(-x, -y);
    }
#endif // SWIG
    
    //! 矢量加法
    Vector2d add(const Vector2d& v) const { return *this + v; }
    
    //! 矢量减法
    Vector2d subtract(const Vector2d& v) const { return *this - v; }
    
    //! 矢量反向，即将该矢量设置为(-x, -y)
    Vector2d& negate() {
        x = -x; y = -y; return *this;
    }
    
    //! 返回正交矢量, 即逆时针旋转90度，为(-y, x)
    Vector2d perpVector() const {
        return Vector2d(-y, x);
    }
    
#ifndef SWIG
    //! 标量积, 数 * 矢量
    friend Vector2d operator*(float s, const Vector2d& v) {
        return Vector2d(v.x * s, v.y * s);
    }
    
    //! 标量积, 矢量 * 数
    Vector2d operator*(float s) const {
        return Vector2d(x * s, y * s);
    }
    
    //! 标量积, 矢量 *= 数
    Vector2d& operator*=(float s) {
        x *= s; y *= s; return *this;
    }
    
    //! 标量积, 矢量 / 数
    Vector2d operator/(float s) const {
        s = 1 / s;
        return Vector2d(x * s, y * s);
    }
    
    //! 标量积, 矢量 /= 数
    Vector2d& operator/=(float s) {
        s = 1 / s;
        x *= s; y *= s; return *this;
    }
#endif // SWIG

    //! 比例放缩
    Vector2d& scaleBy(float sx, float sy) {
        x *= sx; y *= sy; return *this;
    }

    //! 比例放缩
    Vector2d& scaleBy(float s) {
        x *= s; y *= s; return *this;
    }
    
    //! 矢量点积, A·B
    float dotProduct(const Vector2d& v) const {
        return (x * v.x + y * v.y);
    }
    
    //! 矢量叉积(0,0,z)中的z, |A×B|
    float crossProduct(const Vector2d& v) const {
        return (x * v.y - y * v.x);
    }
    
#ifndef SWIG
    //! 矢量点积, A·B
    float operator/(const Vector2d& v) const {
        return dotProduct(v);
    }
    
    //! 矢量叉积(0,0,z)中的z, |A×B|
    float operator*(const Vector2d& v) const {
        return crossProduct(v);
    }
#endif // SWIG
    
    //! 矢量与X轴的夹角, [0, PI)
    float angle() const {
        float len = mgHypot(x, y);
        return len < _MGZERO ? 0 : acosf(x / len);
    }
    
    //! 矢量角度, [-PI, PI), 从X轴逆时针方向为正, 顺时针为负
    float angle2() const {
        return (mgIsZero(x) && mgIsZero(y)) ? 0 : atan2f(y, x);
    }
    
    //! 矢量夹角, [0, PI)
    float angleTo(const Vector2d& v) const {
        float len = mgHypot(x, y) * mgHypot(v.x, v.y);
        return len < _MGZERO ? 0 : acosf(dotProduct(v) / len);
    }
    
    //! 沿逆时针方向到指定矢量的转角, [-PI, PI)
    /*! 从本矢量方向逆时针旋转到另一个矢量 v 所在方向时为正，顺时针为负
    */
    float angleTo2(const Vector2d& v) const {
        float crossz = crossProduct(v);
        float dot = dotProduct(v);
        return (mgIsZero(dot) && mgIsZero(crossz)) ? 0 : atan2f(crossz, dot);
    }
    
    //! 矢量长度
    float length() const {
        return mgHypot(x, y);
    }
    
    //! 矢量长度的平方
    float lengthSquare() const {
        return mgSquare(x, y);
    }
    
    //! 返回单位矢量
    /*! 如果本矢量为零矢量，则复制本矢量
    */
    Vector2d unitVector() const {
        float len = mgHypot(x, y);
        if (len >= _MGZERO)
            return Vector2d(x / len, y / len);
        else
            return *this;
    }
    
    //! 矢量单位化
    /*! 即将本矢量的长度改成1，如果本矢量为零矢量，则不改变本矢量
        \param tol 判断的容差，用到其长度容差
        \return 如果本矢量为零矢量，则返回false，否则返回true
    */
    bool normalize(const Tol& tol = Tol::gTol()) {
        float len = mgHypot(x, y);
        bool ret = (len >= tol.equalPoint());
        if (ret)
            set(x / len, y / len);
        return ret;
    }
    
    //! 判断是否是单位矢量
    /*! 如果本矢量的长度和1相差小于长度容差，则也为单位矢量
        \param tol 判断的容差，用到其长度容差
        \return 如果本矢量为单位矢量，则返回true，否则返回false
    */
    bool isUnitVector(const Tol& tol = Tol::gTol()) const {
        return fabsf(mgHypot(x, y) - 1) < tol.equalPoint();
    }
    
    //! 判断是否是零矢量
    /*! 如果本矢量的长度小于长度容差，则为零矢量
        \param tol 判断的容差，用到其长度容差
        \return 如果本矢量为零矢量，则返回true，否则返回false
    */
    bool isZeroVector(const Tol& tol = Tol::gTol()) const {
        return mgHypot(x, y) < tol.equalPoint();
    }
    
    //! 返回某个分量是为NAN越界值
    bool isDegenerate() const { return isnan(x) || isnan(y); }
    
#ifndef SWIG
    //! 判断两个矢量是否相等
    bool operator==(const Vector2d& v) const {
        return isEqualTo(v);
    }
    
    //! 判断两个矢量是否不相等
    bool operator!=(const Vector2d& v) const {
        return !isEqualTo(v);
    }
#endif // SWIG
    
    //! 用欧拉规则判断两个矢量是否相等
    /*! 如果两个矢量相减的差矢量的长度不大于矢量容差，则相等
        \param v 比较的另一个矢量
        \param tol 判断的容差，用到其矢量容差
        \return 如果相等，则返回true，否则返回false
    */
    bool isEqualTo(const Vector2d& v, const Tol& tol = Tol::gTol()) const {
        return mgHypot(x - v.x, y - v.y) <= tol.equalVector();
    }
    
    //! 设置为矢量(xx, yy)
    Vector2d& set(float xx, float yy) {
        x = xx; y = yy; return *this;
    }
    
    //! 设置为新的矢量
    Vector2d& set(const Vector2d& v) {
        x = v.x; y = v.y; return *this;
    }
    
    //! 设置矢量的角度和长度
    /*!
        \param angle 矢量的角度，弧度
        \param len 矢量的长度
        \return 本矢量的引用
    */
    Vector2d& setAngleLength(float angle, float len) {
        return set(len * cosf(angle), len * sinf(angle));
    }
    
    //! 返回指定角度和长度的矢量
    static Vector2d angledVector(float angle, float len) {
        return Vector2d(len * cosf(angle), len * sinf(angle));
    }
    
    //! 设置矢量的长度
    Vector2d& setLength(float newLength) {
        float oldlen = length();
        float scale = 1.f;
        
        if (!mgIsZero(oldlen)) {
            scale = newLength / oldlen;
        }
        return scaleBy(scale);
    }
    
    //! 返回新长度的矢量
    Vector2d scaledVector(float newLength) const {
        float oldlen = length();
        float scale = 1.f;
        
        if (!mgIsZero(oldlen)) {
            scale = newLength / oldlen;
        }
        return Vector2d(x * scale, y * scale);
    }
    
#ifndef SWIG
    //! 得到四舍五入后的整数大小
    /*! 利用该函数可以将矢量转换为SIZE、CSize值
        \param[out] cx 四舍五入后的X坐标分量
        \param[out] cy 四舍五入后的Y坐标分量
    */
    void get(long& cx, long& cy) const {
        cx = mgRound(x); cy = mgRound(y);
    }
#endif

    //! 判断是否在指定矢量的右侧，即沿逆时针方向转到指定矢量时最近
    bool isRightOf(const Vector2d& vec) const {
        return crossProduct(vec) > 0;
    }

    //! 判断是否在指定矢量的左侧，即沿顺时针方向转到指定矢量时最近
    bool isLeftOf(const Vector2d& vec) const {
        return crossProduct(vec) < 0;
    }
    
    //! 判断两个矢量是否平行
    /*! 判断本矢量和另一个矢量是否平行，零矢量和任何矢量平行
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \return 是否平行
    */
    bool isParallelTo(const Vector2d& vec, const Tol& tol = Tol::gTol()) const;
    
    //! 判断两个矢量是否同向
    /*! 判断本矢量和另一个矢量是否同向，零矢量和任何矢量都不同向
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \return 是否同向
    */
    bool isCodirectionalTo(const Vector2d& vec, const Tol& tol = Tol::gTol()) const;
    
    //! 判断两个矢量是否反向
    /*! 判断本矢量和另一个矢量是否反向，零矢量和任何矢量都不反向
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \return 是否反向
    */
    bool isOppositeTo(const Vector2d& vec, const Tol& tol = Tol::gTol()) const;
    
    //! 判断两个矢量是否垂直
    /*! 判断本矢量和另一个矢量是否平行，零矢量和任何矢量都不垂直
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \return 是否垂直
    */
    bool isPerpendicularTo(const Vector2d& vec, const Tol& tol = Tol::gTol()) const;

#ifndef SWIG
    //! 判断两个矢量是否平行
    /*! 判断本矢量和另一个矢量是否平行，并检查零矢量。
        零矢量和任何矢量平行
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \param[out] nonzero 这两个矢量中是否没有零矢量
        \return 是否平行
    */
    bool isParallelTo(const Vector2d& vec, const Tol& tol, bool& nonzero) const;

    //! 判断两个矢量是否同向
    /*! 判断本矢量和另一个矢量是否同向，并检查零矢量。
        零矢量和任何矢量都不同向
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \param[out] nonzero 这两个矢量中是否没有零矢量
        \return 是否同向
    */
    bool isCodirectionalTo(const Vector2d& vec, const Tol& tol, bool& nonzero) const;

    //! 判断两个矢量是否反向
    /*! 判断本矢量和另一个矢量是否反向，并检查零矢量。
        零矢量和任何矢量都不反向
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \param[out] nonzero 这两个矢量中是否没有零矢量
        \return 是否反向
    */
    bool isOppositeTo(const Vector2d& vec, const Tol& tol, bool& nonzero) const;
    
    //! 判断两个矢量是否垂直
    /*! 判断本矢量和另一个矢量是否平行，并检查零矢量。
        零矢量和任何矢量都不垂直
        \param[in] vec 另一个矢量
        \param[in] tol 判断的容差，用到其矢量容差值
        \param[out] nonzero 这两个矢量中是否没有零矢量
        \return 是否垂直
    */
    bool isPerpendicularTo(const Vector2d& vec, const Tol& tol, bool& nonzero) const;
#endif // SWIG

    //! 求本矢量投影到矢量xAxis上的垂直距离
    /*! 在xAxis的逆时针方向时返回正值，顺时针则返回负值
        \param[in] xAxis 投影到的矢量
        \return 垂直投影距离，有正负
    */
    float distanceToVector(const Vector2d& xAxis) const;
    
    //! 求本矢量在矢量xAxis上的投影比例
    /*! 投影矢量 = xAxis * 投影比例 \n
        投影比例在0到1之间时表示投影矢量是xAxis内的一部分
        \param[in] xAxis 投影到的矢量
        \return 投影比例，有正负
    */
    float projectScaleToVector(const Vector2d& xAxis) const;
    
    //! 求本矢量在矢量xAxis上的投影矢量和垂直矢量
    /*! 将本矢量投影到xAxis，计算投影比例，
        正投影矢量 = xAxis * 投影比例，本矢量 = proj + perp
        \param[in] xAxis 投影到的矢量
        \param[out] proj 正投影矢量
        \param[out] perp 垂直投影矢量
        \return 投影比例，有正负
    */
    float projectResolveVector(const Vector2d& xAxis, 
        Vector2d& proj, Vector2d& perp) const;
    
    //! 矢量分解
    /*! 将本矢量在两个不共线的非零矢量上进行矢量分解, 本矢量 = u*uAxis+v*vAxis
        \param[in] uAxis u轴矢量
        \param[in] vAxis v轴矢量
        \param[out] uv 在这两轴上的矢量分量系数，(uAxis * u, vAxis * v)
        \return 是否求解成功，uAxis和vAxis共线时失败
    */
    bool resolveVector(const Vector2d& uAxis, const Vector2d& vAxis, 
        Vector2d& uv) const;
    
    //! 矢量分解
    /*! 将本矢量在两个不共线的非零矢量上进行矢量分解
        \param[in] uAxis u轴矢量
        \param[in] vAxis v轴矢量
        \return 是否求解成功，uAxis和vAxis共线时失败
    */
    bool resolveVector(const Vector2d& uAxis, const Vector2d& vAxis);
};

#endif // TOUCHVG_VECTOR_H_
