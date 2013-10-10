// mgmat.cpp: 实现二维齐次变换矩阵类Matrix2d
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgmat.h"

// 构造为单位矩阵
Matrix2d::Matrix2d()
{
    m11 = 1.f; m12 = 0.f; m21 = 0.f;
    m22 = 1.f; dx = 0.f; dy = 0.f;
}

// 拷贝构造函数
Matrix2d::Matrix2d(const Matrix2d& src)
{
    m11 = src.m11; m12 = src.m12;
    m21 = src.m21; m22 = src.m22;
    dx  = src.dx;  dy  = src.dy;
}

// 给定元素构造
Matrix2d::Matrix2d(float _m11, float _m12, float _m21, float _m22, 
                   float _dx, float _dy)
{
    m11 = _m11; m12 = _m12;
    m21 = _m21; m22 = _m22;
    dx  = _dx;  dy  = _dy;
}

// 给定两坐标轴矢量和原点构造
Matrix2d::Matrix2d(const Vector2d& e0, const Vector2d& e1, const Point2d& origin)
{
    setCoordSystem(e0, e1, origin);
}

// 矩阵变换, 矢量 * 矩阵
Vector2d Vector2d::operator*(const Matrix2d& m) const
{
    return Vector2d(x * m.m11 + y * m.m21, x * m.m12 + y * m.m22);
}

// 矩阵变换, 矢量 *= 矩阵
Vector2d& Vector2d::operator*=(const Matrix2d& m)
{
    return set(x * m.m11 + y * m.m21, x * m.m12 + y * m.m22);
}

Vector2d Vector2d::transform(const Matrix2d& m) const
{
    return Vector2d(x * m.m11 + y * m.m21, x * m.m12 + y * m.m22);
}

// 矩阵变换, 点 * 矩阵
Point2d Point2d::operator*(const Matrix2d& m) const
{
    return Point2d(x * m.m11 + y * m.m21 + m.dx, 
        x * m.m12 + y * m.m22 + m.dy);
}

// 矩阵变换, 点 *= 矩阵
Point2d& Point2d::operator*=(const Matrix2d& m)
{
    return set(x * m.m11 + y * m.m21 + m.dx, 
        x * m.m12 + y * m.m22 + m.dy);
}

Point2d Point2d::transform(const Matrix2d& m) const
{
    return Point2d(x * m.m11 + y * m.m21 + m.dx, 
        x * m.m12 + y * m.m22 + m.dy);
}

// 对多个点进行矩阵变换
void Matrix2d::TransformPoints(int count, Point2d* points) const
{
    for (int i = 0; i < count; i++)
        points[i] *= (*this);
}

// 对多个矢量进行矩阵变换
void Matrix2d::TransformVectors(int count, Vector2d* vectors) const
{
    for (int i = 0; i < count; i++)
        vectors[i] *= (*this);
}

// 矩阵乘法
Matrix2d Matrix2d::operator*(const Matrix2d& mat) const
{
    Matrix2d result;
    result.setToProduct(*this, mat);
    return result;
}

// 矩阵乘法
Matrix2d& Matrix2d::operator*=(const Matrix2d& mat)
{
    return setToProduct(*this, mat);
}

// 左乘一个矩阵，leftSide * (*this)
Matrix2d& Matrix2d::preMultBy(const Matrix2d& leftSide)
{
    return setToProduct(leftSide, *this);
}

// 右乘一个矩阵，(*this) * rightSide
Matrix2d& Matrix2d::postMultBy(const Matrix2d& rightSide)
{
    return setToProduct(*this, rightSide);
}

// 设置为两个矩阵的乘积
Matrix2d& Matrix2d::setToProduct(const Matrix2d& m1, const Matrix2d& m2)
{
    return set(m1.m11 * m2.m11 + m1.m12 * m2.m21,
        m1.m11 * m2.m12 + m1.m12 * m2.m22,
        m1.m21 * m2.m11 + m1.m22 * m2.m21,
        m1.m21 * m2.m12 + m1.m22 * m2.m22,
        m1.dx  * m2.m11 + m1.dy  * m2.m21 + m2.dx,
        m1.dx  * m2.m12 + m1.dy  * m2.m22 + m2.dy);
}

// 行列式值
float Matrix2d::det() const
{
    return m11 * m22 - m12 * m21;
}

// 设置为逆矩阵
bool Matrix2d::invert()
{
    float d = m11 * m22 - m12 * m21;
    if (mgIsZero(d))
    {
        setToIdentity();
        return false;
    }
    
    d = 1.f / d;
    set(m22 * d, -m12 * d, 
        -m21 * d, m11 * d,
        (m21 * dy - m22 * dx) * d, 
        (m12 * dx - m11 * dy) * d);
    return true;
}

// 返回逆矩阵
Matrix2d Matrix2d::inverse() const
{
    Matrix2d mat (*this);
    mat.invert();
    return mat;
}

// 判断矩阵是否可逆
bool Matrix2d::isInvertible() const
{
    return fabsf(m11 * m22 - m12 * m21) > _MGZERO;
}

// 比例大小
float Matrix2d::scale() const
{
    float sx = scaleX();
    float sy = scaleY();
    return fabsf(sx - sy) < _MGZERO ? sx : mgHypot(sx, sy);
}

// X比例大小
float Matrix2d::scaleX() const
{
    return mgIsZero(m12) ? fabsf(m11) : mgHypot(m11, m12);
}

// Y比例大小
float Matrix2d::scaleY() const
{
    return mgIsZero(m21) ? fabsf(m22) : mgHypot(m21, m22);
}

float Matrix2d::angle() const
{
    return Vector2d(m11, m12).angle2();
}

// 判断两个矩阵是否相等
bool Matrix2d::operator==(const Matrix2d& mat) const
{
    return isEqualTo(mat);
}

// 判断两个矩阵是否不相等
bool Matrix2d::operator!=(const Matrix2d& mat) const
{
    return !isEqualTo(mat);
}

// 用欧拉规则判断两个矩阵是否相等，即判断矩阵的行矢量是否相等
bool Matrix2d::isEqualTo(const Matrix2d& mat, const Tol& tol) const
{
    return mgHypot(m11 - mat.m11, m12 - mat.m12) <= tol.equalVector()
        && mgHypot(m21 - mat.m21, m22 - mat.m22) <= tol.equalVector()
        && mgHypot(dx - mat.dx, dy - mat.dy) <= tol.equalVector();
}

// 判断是否为单位矩阵
bool Matrix2d::isIdentity() const
{
    return mgEquals(m11, 1.f) && mgIsZero(m12)
        && mgEquals(m22, 1.f) && mgIsZero(m21)
        && mgIsZero(dx) && mgIsZero(dy);
}

// 判断矩阵的坐标轴矢量是否分别水平和垂直
bool Matrix2d::isOrtho() const
{
    return mgIsZero(m12) && mgIsZero(m21);
}

bool Matrix2d::hasMirror(Vector2d& reflex) const
{
    Vector2d e0 (m11, m12);
    Vector2d e1 (m21, m22);
    if (e0.normalize() && e1.normalize() && e0.isPerpendicularTo(e1)) {
        if (!mgIsZero(e0.x - e1.y) || !mgIsZero(e0.y + e1.x)) {
            reflex.setAngleLength(e0.angle2() / 2.f, 1.f);
            return true;
        }
    }
    return false;
}

// 得到比例、旋转、镜像成分
bool Matrix2d::isConformal(float& scaleX, float& scaleY, float& angle, 
                           bool& isMirror, Vector2d& reflex) const
{
    Vector2d e0 (m11, m12);
    Vector2d e1 (m21, m22);
    if (!e0.isPerpendicularTo(e1))
        return false;
    
    scaleX = e0.length();
    scaleY = e1.length();
    e0 /= scaleX;
    e1 /= scaleY;
    
    if (mgIsZero(e0.x - e1.y) && mgIsZero(e0.y + e1.x))
    {
        isMirror = false;
        angle = e0.angle2();
    }
    else
    {
        isMirror = true;
        angle = e0.angle2() / 2.f;
        reflex.x = cosf(angle);
        reflex.y = sinf(angle);
        angle = 0.f;
    }
    
    return true;
}

// 设置为原点origin，坐标轴矢量为e0和e1的坐标系
Matrix2d& Matrix2d::setCoordSystem(const Vector2d& e0, const Vector2d& e1, 
                                   const Point2d& origin)
{
    m11 = e0.x;      m12 = e0.y;
    m21 = e1.x;      m22 = e1.y;
    dx  = origin.x;  dy  = origin.y;
    return *this;
}

// 得到坐标系的原点origin，坐标轴矢量e0和e1
void Matrix2d::getCoordSystem(Vector2d& e0, Vector2d& e1, Point2d& origin) const
{
    e0.set(m11, m12);
    e1.set(m21, m22);
    origin.set(dx, dy);
}

// 返回原点为origin，坐标轴矢量为e0和e1的坐标系
Matrix2d Matrix2d::coordSystem(const Vector2d& e0, const Vector2d& e1, 
                               const Point2d& origin)
{
    return Matrix2d(e0, e1, origin);
}

// 返回给定原点、比例和旋转角度的坐标系
Matrix2d Matrix2d::coordSystem(const Point2d& origin, float scaleX, 
                               float scaleY, float angle)
{
    if (mgIsZero(scaleY)) scaleY = scaleX;
    float s = sinf(angle);
    float c = cosf(angle);
    return Matrix2d(c*scaleX, s*scaleX, -s*scaleY, c*scaleY, origin.x, origin.y);
}

// 设置单位矩阵
Matrix2d& Matrix2d::setToIdentity()
{
    return set(1, 0, 0, 1, 0, 0);
}

// 设置矩阵元素
Matrix2d& Matrix2d::set(float _m11, float _m12, float _m21, float _m22,
                        float _dx, float _dy)
{
    m11 = _m11; m12 = _m12;
    m21 = _m21; m22 = _m22;
    dx  = _dx;  dy  = _dy;
    return *this;
}

// 设置为平移变换矩阵
Matrix2d& Matrix2d::setToTranslation(const Vector2d& vec)
{
    return set(1.f, 0.f, 0.f, 1.f, vec.x, vec.y);
}

// 设置为以一点为中心的旋转变换矩阵
Matrix2d& Matrix2d::setToRotation(float angle, 
                                  const Point2d& center)
{
    float c = cosf(angle);
    float s = sinf(angle);
    return set(c, s, -s, c, (1 - c) * center.x + s * center.y, 
        (1 - c) * center.y - s * center.x);
}

// 设置为以一点为中心的放缩变换矩阵
Matrix2d& Matrix2d::setToScaling(float scale, 
                                 const Point2d& center)
{
    return setToScaling(scale, scale, center);
}

// 设置为以一点为中心的放缩变换矩阵
Matrix2d& Matrix2d::setToScaling(float scaleX, float scaleY, 
                                 const Point2d& center)
{
    if (mgIsZero(scaleY)) scaleY = scaleX;
    return set(scaleX, 0, 0, scaleY,
        (1 - scaleX) * center.x, (1 - scaleY) * center.y);
}

// 设置为关于一点对称的变换矩阵
Matrix2d& Matrix2d::setToMirroring(const Point2d& pnt)
{
    return set(-1.f, 0.f, 0.f, -1.f, 2.f * pnt.x, 2.f * pnt.y);
}

// 设置为以直线{pnt,dir}为中心线的对称变换矩阵
Matrix2d& Matrix2d::setToMirroring(const Point2d& pnt, const Vector2d& dir)
{
    float d2 = dir.lengthSqrd();
    if (mgIsZero(d2))
        setToIdentity();
    else
    {
        float s2 = 2.f * dir.x * dir.y / d2;
        float c2 = (dir.x * dir.x - dir.y * dir.y) / d2;
        set(c2, s2, s2, -c2, (1 - c2) * pnt.x - s2 * pnt.y, 
            (1 + c2) * pnt.y - s2 * pnt.x);
    }
    return *this;
}

// 设置为错切变换矩阵
Matrix2d& Matrix2d::setToShearing(float sx, float sy, const Point2d& pnt)
{
    if (mgIsZero(sy)) sy = sx;
    return set(1.f, sx, sy, 1.f, -sy * pnt.y, -sx * pnt.x);
}

// 得到平移变换矩阵
Matrix2d Matrix2d::translation(const Vector2d& vec)
{
    Matrix2d mat;
    mat.setToTranslation(vec);
    return mat;
}

// 得到以一点为中心的旋转变换矩阵
Matrix2d Matrix2d::rotation(float angle, const Point2d& center)
{
    Matrix2d mat;
    mat.setToRotation(angle, center);
    return mat;
}

// 得到以一点为中心的放缩变换矩阵
Matrix2d Matrix2d::scaling(float scale, const Point2d& center)
{
    Matrix2d mat;
    mat.setToScaling(scale, scale, center);
    return mat;
}

// 得到以一点为中心的放缩变换矩阵
Matrix2d Matrix2d::scaling(float scaleX, float scaleY, const Point2d& center)
{
    Matrix2d mat;
    mat.setToScaling(scaleX, scaleY, center);
    return mat;
}

// 得到关于一点对称的变换矩阵
Matrix2d Matrix2d::mirroring(const Point2d& pnt)
{
    Matrix2d mat;
    mat.setToMirroring(pnt);
    return mat;
}

// 得到以直线{pnt,dir}为中心线的对称变换矩阵
Matrix2d Matrix2d::mirroring(const Point2d& pnt, const Vector2d& dir)
{
    Matrix2d mat;
    mat.setToMirroring(pnt, dir);
    return mat;
}

// 得到错切变换矩阵
Matrix2d Matrix2d::shearing(float sx, float sy, const Point2d& pnt)
{
    Matrix2d mat;
    mat.setToShearing(sx, sy, pnt);
    return mat;
}
