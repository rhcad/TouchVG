//! \file mgmat.h
//! \brief 定义二维齐次变换矩阵类 Matrix2d
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MATRIX_H_
#define TOUCHVG_MATRIX_H_

#include "mgpnt.h"

//! 二维齐次变换矩阵类
/*!
    \ingroup GEOM_CLASS
    
    变换矩阵类Matrix用于二维图形的仿射变换。矩阵 M 的形式为: \n
    　　e0x　e0y　0 \n
    　　e1x　e1y　0 \n
    　　dx　　dy　1 \n
    其中矩阵 A: \n
    　　e0x　e0y \n
    　　e1x　e1y \n
    称为 M 的线性部分，其作用是对图形进行伸缩、旋转、对称和错切等变换。\n
    行向量 T (dx, dy) 称为 M 的平移部分，其作用是对图形进行平移变换。\n
    
    Point的点pnt的对应变换点为 P x M，行矢量 P = (pnt.x, pnt.y, 1) \n
    Vector的矢量vec的对应变换矢量为 V x M，M 的平移部分不参与计算。\n
    
    每个矩阵 M 对应一个坐标系，其坐标原点为(dx, dy)，坐标轴为矢量(e0x, e0y)和
    (e1x, e1y)。M 为将(0,0)(1,0)(0,1)映射到(dx, dy)(e0x, e0y)(e1x, e1y)的矩阵。
*/
class Matrix2d
{
public:
    float   m11;    //!< 矩阵元素，代表X方向线性变换分量
    float   m12;    //!< 矩阵元素，代表X方向线性变换分量
    float   m21;    //!< 矩阵元素，代表Y方向线性变换分量
    float   m22;    //!< 矩阵元素，代表Y方向线性变换分量
    float   dx;     //!< 矩阵元素，代表X方向平移量
    float   dy;     //!< 矩阵元素，代表Y方向平移量

    //! 单位矩阵
    static const Matrix2d kIdentity() { return Matrix2d(); }
    
    //! 构造为单位矩阵
    Matrix2d();
    
    //! 拷贝构造函数
    Matrix2d(const Matrix2d& src);
    
    //! 给定元素构造
    Matrix2d(float _m11, float _m12, float _m21, float _m22, float _dx, float _dy);
    
    //! 给定两坐标轴矢量和原点构造
    /*! 相对于绝对坐标系构造出新的坐标系矩阵。
        如果这两个坐标轴矢量不正交，则构造出仿射坐标系，否则为正交坐标系
        \param e0 X轴一个单位的矢量
        \param e1 Y轴一个单位的矢量
        \param origin 新坐标系的原点
    */
    Matrix2d(const Vector2d& e0, const Vector2d& e1, const Point2d& origin);
    
#ifndef SWIG
    //! 矩阵乘法
    Matrix2d operator*(const Matrix2d& mat) const;
    
    //! 矩阵乘法
    Matrix2d& operator*=(const Matrix2d& mat);
    
    //! 矩阵的数乘, 数 * 矩阵
    friend Matrix2d operator*(float s, const Matrix2d& mat) {
        return Matrix2d(mat.m11 * s, mat.m12 * s, mat.m21 * s, mat.m22 * s, mat.dx * s, mat.dy * s);
    }
    
    //! 矩阵的数乘, 矩阵 * 数
    Matrix2d operator*(float s) const {
        return Matrix2d(m11 * s, m12 * s, m21 * s, m22 * s, dx * s, dy * s);
    }
    
    //! 矩阵的数乘, 矩阵 *= 数
    Matrix2d& operator*=(float s) {
        return set(m11 * s, m12 * s, m21 * s, m22 * s, dx * s, dy * s);
    }
    
    //! 矩阵的数乘, 矩阵 / 数
    Matrix2d operator/(float s) const { return operator*(1 / s); }
    
    //! 矩阵的数乘, 矩阵 /= 数
    Matrix2d& operator/=(float s) { return operator*=(1 / s); }
#endif // SWIG
    
    //! 比例放缩
    Matrix2d& scaleBy(float sx, float sy) {
        return set(m11 * sx, m12 * sx, m21 * sy, m22 * sy, dx * sx, dy * sy);
    }
    
    //! 比例放缩、矩阵的数乘
    Matrix2d& scaleBy(float s) { return operator*=(s); }
    
    //! 左乘一个矩阵，leftSide * (*this)
    Matrix2d& preMultBy(const Matrix2d& leftSide);
    
    //! 右乘一个矩阵，(*this) * rightSide
    Matrix2d& postMultBy(const Matrix2d& rightSide);
    
    //! 设置为两个矩阵的乘积 (m1 * m2)
    Matrix2d& setToProduct(const Matrix2d& m1, const Matrix2d& m2);
    
    //! 对多个点进行矩阵变换
    /*!
        \param[in] count 点的个数
        \param[in,out] points 要变换的点的数组，元素个数为count
    */
    void transformPoints(int count, Point2d* points) const;

    //! 对多个矢量进行矩阵变换
    /*! 对矢量进行矩阵变换时，矩阵的平移分量部分不起作用
        \param[in] count 矢量的个数
        \param[in,out] vectors 要变换的矢量的数组，元素个数为count
    */
    void transformVectors(int count, Vector2d* vectors) const;
    
    //! 行列式值
    float det() const;
    
    //! 设置为逆矩阵
    /*!
        \return 是否可逆，如果不可逆则设置为单位矩阵
    */
    bool invert();
    
    //! 返回逆矩阵
    Matrix2d inverse() const;
    
    //! 判断矩阵是否可逆
    bool isInvertible() const;
    
    //! 比例大小
    float scale() const;
    
    //! X比例大小
    float scaleX() const;
    
    //! Y比例大小
    float scaleY() const;

    //! X旋转角度，弧度
    float angle() const;
    
#ifndef SWIG
    //! 判断两个矩阵是否相等
    bool operator==(const Matrix2d& mat) const;
    
    //! 判断两个矩阵是否不相等
    bool operator!=(const Matrix2d& mat) const;
#endif
    
    //! 用欧拉规则判断两个矩阵是否相等，即判断矩阵的行矢量是否相等
    bool isEqualTo(const Matrix2d& mat, const Tol& tol = Tol::gTol()) const;

    //! 判断是否为单位矩阵
    bool isIdentity() const;

    //! 判断矩阵的坐标轴矢量是否分别水平和垂直
    bool isOrtho() const;

    //! 判断矩阵中是否含有对称成分
    bool hasMirror(Vector2d& reflex) const;
    
#ifndef SWIG
    //! 得到比例、旋转、镜像成分
    /*!
        \param[out] scaleX 矩阵的X方向放缩比例
        \param[out] scaleY 矩阵的Y方向放缩比例
        \param[out] angle 矩阵的旋转角度，弧度
        \param[out] isMirror 矩阵中是否含有对称成分
        \param[out] reflex 矩阵中的对称轴方向矢量(如果有对称成分)
        \return 是否求解成功，矩阵的X轴和Y轴不垂直则失败
    */
    bool isConformal(float& scaleX, float& scaleY, float& angle,
        bool& isMirror, Vector2d& reflex) const;
#endif
    
    //! 设置为原点为origin，坐标轴矢量为e0和e1的坐标系
    Matrix2d& setCoordSystem(
        const Vector2d& e0, const Vector2d& e1, const Point2d& origin);
    
    //! 得到坐标系的原点origin，坐标轴矢量e0和e1
    void getCoordSystem(
        Vector2d& e0, Vector2d& e1, Point2d& origin) const;
    
    //! 返回原点为origin，坐标轴矢量为e0和e1的坐标系
    static Matrix2d coordSystem(
        const Vector2d& e0, const Vector2d& e1, const Point2d& origin);
    
    //! 返回给定原点、比例和旋转角度的坐标系
    /*!
        \param origin 新坐标系的原点坐标
        \param scaleX X轴的比例系数
        \param scaleY Y轴的比例系数，为0则取为scaleX
        \param angle X轴的旋转角度，弧度
        \return 新坐标系的矩阵
    */
    static Matrix2d coordSystem(const Point2d& origin, 
        float scaleX, float scaleY = 0, float angle = 0);
    
    //! 设置为单位矩阵
    Matrix2d& setToIdentity();
    
    //! 设置矩阵元素
    Matrix2d& set(float _m11, float _m12, float _m21, float _m22,
        float _dx, float _dy);
    
    //! 设置为平移变换矩阵
    /*!
        \param vec 平移矢量
        \return 改变后的本对象的引用
    */
    Matrix2d& setToTranslation(const Vector2d& vec);
    
    //! 设置为以一点为中心的旋转变换矩阵
    /*!
        \param angle 旋转角度，弧度，逆时针为正
        \param center 旋转中心点
        \return 改变后的本对象的引用
    */
    Matrix2d& setToRotation(float angle, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 设置为以一点为中心的放缩变换矩阵
    /*!
        \param scale 放缩比例
        \param center 放缩中心点
        \return 改变后的本对象的引用
    */
    Matrix2d& setToScaling(float scale, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 设置为以一点为中心的放缩变换矩阵
    /*!
        \param scaleX 在X方向上的放缩比例
        \param scaleY 在Y方向上的放缩比例，如果为0则取为scaleX
        \param center 放缩中心点
        \return 改变后的本对象的引用
    */
    Matrix2d& setToScaling(float scaleX, float scaleY, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 设置为关于一点对称的变换矩阵
    /*!
        \param pnt 对称中心点
        \return 改变后的本对象的引用
    */
    Matrix2d& setToMirroring(const Point2d& pnt = Point2d::kOrigin());
    
    //! 设置为以直线(pnt,dir)为中心线的对称变换矩阵
    /*!
        \param pnt 对称中心线上的一点
        \param dir 对称中心线的方向矢量
        \return 改变后的本对象的引用
    */
    Matrix2d& setToMirroring(const Point2d& pnt, const Vector2d& dir);
    
    //! 设置为错切变换矩阵
    /*!
        \param sx 在X方向上的错切比例
        \param sy 在Y方向上的错切比例，如果为0则取为sx
        \param pnt 错切基点，在该点上的点变换后位置不变
        \return 改变后的本对象的引用
    */
    Matrix2d& setToShearing(float sx, float sy, 
        const Point2d& pnt = Point2d::kOrigin());
    
    //! 得到平移变换矩阵
    /*!
        \param vec 平移矢量
        \return 新的变换矩阵
    */
    static Matrix2d translation(const Vector2d& vec);
    
    //! 得到以一点为中心的旋转变换矩阵
    /*!
        \param angle 旋转角度，弧度，逆时针为正
        \param center 旋转中心点
        \return 新的变换矩阵
    */
    static Matrix2d rotation(float angle, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 得到以一点为中心的放缩变换矩阵
    /*!
        \param scale 放缩比例
        \param center 放缩中心点
        \return 新的变换矩阵
    */
    static Matrix2d scaling(float scale, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 得到以一点为中心的放缩变换矩阵
    /*!
        \param scaleX 在X方向上的放缩比例
        \param scaleY 在Y方向上的放缩比例，如果为0则取为scaleX
        \param center 放缩中心点
        \return 新的变换矩阵
    */
    static Matrix2d scaling(float scaleX, float scaleY, 
        const Point2d& center = Point2d::kOrigin());
    
    //! 得到关于一点对称的变换矩阵
    /*!
        \param pnt 对称中心点
        \return 新的变换矩阵
    */
    static Matrix2d mirroring(const Point2d& pnt = Point2d::kOrigin());
    
    //! 得到以直线(pnt,dir)为中心线的对称变换矩阵
    /*!
        \param pnt 对称中心线上的一点
        \param dir 对称中心线的方向矢量
        \return 新的变换矩阵
    */
    static Matrix2d mirroring(const Point2d& pnt, const Vector2d& dir);
    
    //! 得到错切变换矩阵
    /*!
        \param sx 在X方向上的错切比例
        \param sy 在Y方向上的错切比例，如果为0则取为sx
        \param pnt 错切基点，在该点上的点变换后位置不变
        \return 新的变换矩阵
    */
    static Matrix2d shearing(float sx, float sy, 
        const Point2d& pnt = Point2d::kOrigin());
    
    //! 得到两个点同时移动后形成的变化矩阵
    static Matrix2d transformWith2P(const Point2d& from1, const Point2d& from2,
                                    const Point2d& to1, const Point2d& to2);
};

#endif // TOUCHVG_MATRIX_H_
