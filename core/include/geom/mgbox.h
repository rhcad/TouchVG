//! \file mgbox.h
//! \brief 定义矩形框类 Box2d
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_RECT_H_
#define TOUCHVG_RECT_H_

#include "mgpnt.h"

class Matrix2d;

//! 显示坐标矩形框结构，用于显示坐标系
typedef struct tagRECT2D
{
    float   left;   //!< 矩形左上角X坐标
    float   top;    //!< 矩形左上角Y坐标
    float   right;  //!< 矩形右下角X坐标
    float   bottom; //!< 矩形右下角Y坐标
    tagRECT2D() : left(0), top(0), right(0), bottom(0) {}
    float width() const { return right - left; }
    float height() const { return bottom - top; }
} RECT_2D;

//! 矩形框类
/*!
    \ingroup GEOM_CLASS
    本类用于描述二维平面上水平矩形框，或坐标范围
*/
class Box2d
{
public:
    float   xmin;   //!< 矩形左下角X坐标
    float   ymin;   //!< 矩形左下角Y坐标
    float   xmax;   //!< 矩形右上角X坐标
    float   ymax;   //!< 矩形右上角Y坐标

    //! 收缩到原点的空矩形框
    static const Box2d kIdentity() { return Box2d(); }

    //! 构造收缩到原点的矩形框
    Box2d() : xmin(0), ymin(0), xmax(0), ymax(0)
    {
    }

    //! 拷贝构造函数，默认不自动规范化
    Box2d(const Box2d& src, bool normal = false);
    
    //! 给定两个对角点构造，自动规范化
    Box2d(const Point2d& pnt1, const Point2d& pnt2)
    {
        set(pnt1, pnt2);
    }
    
    //! 给定对角点坐标构造，默认不自动规范化
    Box2d(float l, float t, float r, float b, bool normal = false)
    {
        xmin = l; ymin = t; xmax = r; ymax = b;
        if (normal)
            normalize();
    }

    //! 给定显示坐标矩形框构造，默认不自动规范化
    Box2d(const RECT_2D& rc, bool normal = false)
    {
        xmin = rc.left; ymin = rc.top;
        xmax = rc.right; ymax = rc.bottom;
        if (normal)
            normalize();
    }

    //! 给定对角点整数坐标构造，默认不自动规范化
    Box2d(int l, int t, int r, int b, bool normal = false)
    {
        xmin = (float)l; ymin = (float)t;
        xmax = (float)r; ymax = (float)b;
        if (normal)
            normalize();
    }
    
    //! 给定四个顶点构造
    Box2d(const Point2d& pnt1, const Point2d& pnt2,
        const Point2d& pnt3, const Point2d& pnt4)
    {
        set(pnt1, pnt2, pnt3, pnt4);
    }
    
    //! 给定多个顶点构造
    Box2d(int count, const Point2d* points)
    {
        set(count, points);
    }
    
    //! 给定中心坐标和宽高构造
    /*!
        \param center 矩形的中心坐标
        \param width 矩形宽度，为负数则不是规范化矩形
        \param height 矩形高度，为负数则不是规范化矩形
    */
    Box2d(const Point2d& center, float width, float height)
    {
        set(center, width, height);
    }
    
    //! 给定宽高构造(0, 0, w, h)
    Box2d(float width, float height)
    {
        set(0.f, 0.f, width, height);
    }
    
    //! 得到两个对角点
    void get(Point2d& p1, Point2d& p2) const
    {
        p1.set(xmin, ymin);
        p2.set(xmax, ymax);
    }

    //! 转换到显示坐标矩形框，已上下对调
    RECT_2D& get(RECT_2D& rc) const
    {
        rc.left = xmin; rc.top = ymin;
        rc.right = xmax; rc.bottom = ymax;
        return rc;
    }

#if defined(_WINDEF_) || defined(_WINDEF_H)
    //! 得到四舍五入后的矩形(CRect)，已上下对调
    /*! 如果本矩形为规范化矩形，则取出的RECT也符合Windows规范化矩形要求，
        即 CRect::NormalizeRect() 的结果。
    */
    RECT& get(RECT& rc) const
    {
        get(rc.left, rc.top, rc.right, rc.bottom);
        return rc;
    }
#endif
    
#ifndef SWIG
    //! 得到四舍五入后的矩形(RECT)，已上下对调
    /*! 如果本矩形为规范化矩形，则取出的RECT也符合Windows规范化矩形要求，
        即 CRect::NormalizeRect() 的结果。
        \param[out] l RECT.left，对应于xmin
        \param[out] t RECT.top，对应于ymin
        \param[out] r RECT.right，对应于xmax
        \param[out] b RECT.bottom，对应于ymax
    */
    void get(long& l, long& t, long& r, long& b) const
    {
        l = mgRound(xmin); t = mgRound(ymin);
        r = mgRound(xmax); b = mgRound(ymax);
    }
#endif

    //! 复制矩形，默认不自动规范化
    Box2d& set(const Box2d& src, bool normal = false);
    
    //! 设置两个对角点，自动规范化
    Box2d& set(const Point2d& p1, const Point2d& p2);
    
    //! 设置两个对角点坐标，自动规范化
    Box2d& set(float x1, float y1, float x2, float y2);
    
    //! 设置为四个顶点的包容框
    Box2d& set(const Point2d& p1, const Point2d& p2, 
        const Point2d& p3, const Point2d& p4);
    
    //! 设置为多个顶点的包容框
    Box2d& set(int count, const Point2d* points);
    
    //! 设置中心坐标和宽高
    /*!
        \param center 矩形的中心坐标
        \param width 矩形宽度，为负数则不是规范化矩形
        \param height 矩形高度，为负数则不是规范化矩形
        \return 本矩形的引用
    */
    Box2d& set(const Point2d& center, float width, float height)
    {
        if (mgIsZero(height))
            height = width;
        xmin = center.x - width * 0.5f;
        ymin = center.y - height * 0.5f;
        xmax = center.x + width * 0.5f;
        ymax = center.y + height * 0.5f;
        return *this;
    }
    
    //! 返回宽度，非负数
    float width() const
    {
        return fabsf(xmax - xmin);
    }
    
    //! 返回高度，非负数
    float height() const
    {
        return fabsf(ymax - ymin);
    }
    
    //! 返回矩形框大小，非负数
    Vector2d size() const
    {
        return Vector2d(fabsf(xmax - xmin), fabsf(ymax - ymin));
    }
    
    //! 返回中心坐标
    Point2d center() const
    {
        return Point2d((xmin + xmax) * 0.5f, (ymin + ymax) * 0.5f);
    }
    
    //! 返回左上坐标
    Point2d leftTop() const
    {
        return Point2d(xmin, ymax);
    }
    
    //! 返回右上坐标
    Point2d rightTop() const
    {
        return Point2d(xmax, ymax);
    }
    
    //! 返回左下坐标
    Point2d leftBottom() const
    {
        return Point2d(xmin, ymin);
    }
    
    //! 返回右下坐标
    Point2d rightBottom() const
    {
        return Point2d(xmax, ymin);
    }
    
    //! 规范化，使xmin<=xmax且ymin<=ymax
    Box2d& normalize()
    {
        if (xmin > xmax)
            mgSwap(xmin, xmax);
        if (ymin > ymax)
            mgSwap(ymin, ymax);
        return *this;
    }
    
    //! 交换上下坐标
    Box2d& swapTopBottom()
    {
        mgSwap(ymin, ymax);
        return *this;
    }
    
    //! 设置为空矩形框
    Box2d& empty()
    {
        xmin = ymin = xmax = ymax = 0;
        return *this;
    }
    
    //! 判断是否规范化
    bool isNormalized() const
    {
        return xmin <= xmax && ymin <= ymax;
    }
    
    //! 判断宽和高是否都为零且位于原点
    bool isNull() const
    {
        return mgIsZero(xmin) && mgIsZero(ymin)
            && mgIsZero(xmax) && mgIsZero(ymax);
    }
    
    //! 判断宽或高的绝对值是否为零
    bool isEmpty(const Tol& tol = Tol::gTol(), bool useOr = true) const
    {
        bool c1 = fabsf(xmax - xmin) < tol.equalPoint();
        bool c2 = fabsf(ymax - ymin) < tol.equalPoint();
        return useOr ? (c1 || c2) : (c1 && c2);
    }
    
    //! 判断宽或高是否接近或小于零
    bool isEmptyMinus(const Tol& tol = Tol::gTol()) const
    {
        return (xmax - xmin) < tol.equalPoint()
            || (ymax - ymin) < tol.equalPoint();
    }
    
    //! 判断一个点是否在本矩形框内，闭区间，矩形框必须规范化
    /*! 判断是在本矩形框内或位于矩形框边上。\n本矩形框必须规范化。
        \param pt 给定的测试点
        \return 如果在本矩形框内或框的边上，则返回true，否则返回false
    */
    bool contains(const Point2d& pt) const
    {
        return pt.x >= xmin && pt.y >= ymin
            && pt.x <= xmax && pt.y <= ymax;
    }
    
    //! 判断一个点是否在本矩形框内，闭区间
    /*! 将本矩形框向外扩大长度容差的距离后来判断。\n本矩形框必须规范化。
        \param pt 给定的测试点
        \param tol 判断的容差，用到其长度容差
        \return 如果在本矩形框内或框的边上，则返回true，否则返回false
    */
    bool contains(const Point2d& pt, const Tol& tol) const
    {
        return pt.x >= xmin - tol.equalPoint()
            && pt.y >= ymin - tol.equalPoint()
            && pt.x <= xmax + tol.equalPoint()
            && pt.y <= ymax + tol.equalPoint();
    }
    
    //! 判断一个矩形框是否在本矩形框内，闭区间
    /*! 判断是在本矩形框内或和本矩形框的边有重合。\n
        这两个矩形框必须规范化。
        \param box 另一个矩形框
        \return 如果在本矩形框内或有重合，则返回true，否则返回false
    */
    bool contains(const Box2d& box) const
    {
        return box.xmin >= xmin && box.ymin >= ymin
            && box.xmax <= xmax && box.ymax <= ymax;
    }
    
    //! 判断一个矩形框是否在本矩形框内，闭区间
    /*! 将本矩形框向外扩大长度容差的距离后来判断，有部分重合也返回true。\n
        这两个矩形框必须规范化。
        \param box 另一个矩形框
        \param tol 判断的容差，用到其长度容差
        \return 如果在本矩形框内或有重合，则返回true，否则返回false
    */
    bool contains(const Box2d& box, const Tol& tol) const
    {
        return box.xmin >= xmin - tol.equalPoint()
            && box.ymin >= ymin - tol.equalPoint()
            && box.xmax <= xmax + tol.equalPoint()
            && box.ymax <= ymax + tol.equalPoint();
    }
    
    //! 中心不变，扩大矩形框
    /*!
        \param d 向外侧的偏移距离
        \return 本对象的引用
    */
    Box2d& inflate(float d)
    {
        return inflate(d, d);
    }
    
    //! 中心不变，扩大矩形框
    /*!
        \param x 向外侧的X方向的偏移距离
        \param y 向外侧的Y方向的偏移距离
        \return 本对象的引用
    */
    Box2d& inflate(float x, float y)
    {
        xmin -= x; ymin -= y; xmax += x; ymax += y;
        return *this;
    }
    
    //! 中心不变，扩大矩形框
    /*!
        \param vec 向外侧的偏移，vec.x为X方向的偏移距离，vec.y为Y方向的偏移距离
        \return 本对象的引用
    */
    Box2d& inflate(const Vector2d& vec)
    {
        return inflate(vec.x, vec.y);
    }
    
    //! 中心不变，扩大矩形框
    /*!
        \param box 向外侧的偏移，
            box.xmin为左侧的偏移距离，box.ymin为底侧的偏移距离，
            box.xmax为右侧的偏移距离，box.ymax为上侧的偏移距离
        \return 本对象的引用
    */
    Box2d& inflate(const Box2d& box)
    {
        return inflate(box.xmin, box.ymin, box.xmax, box.ymax);
    }
    
    //! 中心不变，扩大矩形框
    /*!
        \param l 矩形框左侧的偏移距离
        \param b 矩形框底侧的偏移距离
        \param r 矩形框右侧的偏移距离
        \param t 矩形框上侧的偏移距离
        \return 本对象的引用
    */
    Box2d& inflate(float l, float b, float r, float t)
    {
        xmin -= l; ymin -= b; xmax += r; ymax += t;
        return *this;
    }
    
    //! 中心不变，缩小矩形框
    /*!
        \param d 向内侧的偏移距离
        \return 本对象的引用
    */
    Box2d& deflate(float d)
    {
        return deflate(d, d);
    }
    
    //! 中心不变，缩小矩形框
    /*!
        \param x 向内侧的X方向的偏移距离
        \param y 向内侧的Y方向的偏移距离
        \return 本对象的引用
    */
    Box2d& deflate(float x, float y)
    {
        xmin += x; ymin += y; xmax -= x; ymax -= y;
        return *this;
    }
    
    //! 中心不变，缩小矩形框
    /*!
        \param vec 向内侧的偏移，vec.x为X方向的偏移距离，vec.y为Y方向的偏移距离
        \return 本对象的引用
    */
    Box2d& deflate(const Vector2d& vec)
    {
        return deflate(vec.x, vec.y);
    }
    
    //! 中心不变，缩小矩形框
    /*!
        \param box 向内侧的偏移，
            box.xmin为右侧的偏移距离，box.ymin为上侧的偏移距离，
            box.xmax为左侧的偏移距离，box.ymax为底侧的偏移距离
        \return 本对象的引用
    */
    Box2d& deflate(const Box2d& box)
    {
        return deflate(box.xmin, box.ymin, box.xmax, box.ymax);
    }
    
    //! 中心不变，缩小矩形框
    /*!
        \param l 矩形框右侧的偏移距离
        \param b 矩形框上侧的偏移距离
        \param r 矩形框左侧的偏移距离
        \param t 矩形框底侧的偏移距离
        \return 本对象的引用
    */
    Box2d& deflate(float l, float b, float r, float t)
    {
        xmin += l; ymin += b; xmax -= r; ymax -= t;
        return *this;
    }
    
    //! 平移
    Box2d& offset(float x, float y)
    {
        xmin += x; ymin += y; xmax += x; ymax += y;
        return *this;
    }
    
    //! 平移
    Box2d& offset(const Vector2d& vec)
    {
        return offset(vec.x, vec.y);
    }
    
    //! 比例放缩，以原点为中心
    /*!
        \param sx X方向放缩比例
        \param sy Y方向放缩比例，为0则取为sx
        \return 本矩形的引用
    */
    Box2d& scaleBy(float sx, float sy = 0)
    {
        if (mgIsZero(sy)) sy = sx;
        xmin *= sx; xmax *= sx;
        ymin *= sy; ymax *= sy;
        return *this;
    }
    
    //! 判断是否和一个矩形框相交（两者都是规范化矩形）
    /*! 如果其中一个矩形为空矩形或非规范化矩形，则返回false。
        \param box 另一个矩形框，必须规范化
        \return 是否相交，失败时返回false
    */
    bool isIntersect(const Box2d& box) const;
    
    //! 设置为两个矩形框的交集（两者都是规范化矩形）
    /*! 设置本矩形框为给定的两个矩形框的交集，
        如果其中一个为空矩形，则设置为空矩形。
        \param r1 第一个矩形框，必须规范化，否则视为空矩形
        \param r2 第二个矩形框，必须规范化，否则视为空矩形
    */
    Box2d& intersectWith(const Box2d& r1, const Box2d& r2);
    
    //! 设置为和一个矩形框的交集（两者都是规范化矩形）
    /*! 设置本矩形框为本矩形框和另一个矩形框的交集，
        如果其中一个为空矩形，则设置为空矩形。
        \param box 另一个矩形框，必须规范化，否则视为空矩形
    */
    Box2d& intersectWith(const Box2d& box)
    {
        return intersectWith(*this, box);
    }
    
    //! 设置为两个矩形框的并集（两者都是规范化矩形）
    /*! 设置本矩形框为给定的两个矩形框的并集。\n
        如果其中一个为空矩形，则直接设置为另一个矩形框。
        \param r1 第一个矩形框，必须规范化，否则视为空矩形
        \param r2 第二个矩形框，必须规范化，否则视为空矩形
        \return 本矩形的引用，并集，规范化矩形
    */
    Box2d& unionWith(const Box2d& r1, const Box2d& r2);
    
    //! 合并一个矩形框（两者都是规范化矩形）
    /*! 设置本矩形框为本矩形框和另一个矩形框的并集。\n
        如果其中一个为空矩形，则直接设置为另一个矩形框，
        非规范化矩形将视为空矩形。
        \param box 另一个矩形框，必须规范化，否则视为空矩形
        \return 本矩形的引用，并集，规范化矩形
    */
    Box2d& unionWith(const Box2d& box)
    {
        return unionWith(*this, box);
    }
    
    //! 合并一个点
    /*! 放大本矩形框以使得给定的点包含在本矩形框内，并使放大量最小
        \param x 要包含的点的X坐标
        \param y 要包含的点的Y坐标
        \return 本矩形的引用，规范化矩形
    */
    Box2d& unionWith(float x, float y)
    {
        if (xmin > x) xmin = x;
        if (ymin > y) ymin = y;
        if (xmax < x) xmax = x;
        if (ymax < y) ymax = y;
        return *this;
    }
    
    //! 合并一个点
    /*! 放大本矩形框以使得给定的点包含在本矩形框内，并使放大量最小
        \param pt 要包含的点
        \return 本矩形的引用，规范化矩形
    */
    Box2d& unionWith(const Point2d& pt)
    {
        return unionWith(pt.x, pt.y);
    }

    //! 正向平移矩形框
    /*!
        \param box 指定四个方向偏移量的矩形框
        \return 新的矩形框
    */
    Box2d offset(const Box2d& box) const
    {
        return Box2d(xmin + box.xmin, ymin + box.ymin, 
            xmax + box.xmax, ymax + box.ymax);
    }
    
#ifndef SWIG
    //! 正向平移矩形框
    /*!
        \param box 指定四个方向偏移量的矩形框
        \return 新的矩形框
    */
    Box2d operator+(const Box2d& box) const
    {
        return Box2d(xmin + box.xmin, ymin + box.ymin, 
            xmax + box.xmax, ymax + box.ymax);
    }
    
    //! 正向平移矩形框
    /*!
        \param box 指定四个方向偏移量的矩形框
        \return 本对象的引用
    */
    Box2d& operator+=(const Box2d& box)
    {
        return set(xmin + box.xmin, ymin + box.ymin, 
            xmax + box.xmax, ymax + box.ymax);
    }
    
    //! 反向平移矩形框
    /*!
        \param box 指定四个方向偏移量的矩形框
        \return 新的矩形框
    */
    Box2d operator-(const Box2d& box) const
    {
        return Box2d(xmin - box.xmin, ymin - box.ymin, 
            xmax - box.xmax, ymax - box.ymax);
    }
    
    //! 反向平移矩形框
    /*!
        \param box 指定四个方向偏移量的矩形框
        \return 本对象的引用
    */
    Box2d& operator-=(const Box2d& box)
    {
        return set(xmin - box.xmin, ymin - box.ymin, 
            xmax - box.xmax, ymax - box.ymax);
    }
    
    //! 正向平移矩形框
    Box2d operator+(const Vector2d& vec) const
    {
        return Box2d(xmin + vec.x, ymin + vec.y, 
            xmax + vec.x, ymax + vec.y);
    }
    
    //! 正向平移矩形框
    Box2d& operator+=(const Vector2d& vec)
    {
        return set(xmin + vec.x, ymin + vec.y, 
            xmax + vec.x, ymax + vec.y);
    }
    
    //! 反向平移矩形框
    Box2d operator-(const Vector2d& vec) const
    {
        return Box2d(xmin - vec.x, ymin - vec.y, 
            xmax - vec.x, ymax - vec.y);
    }
    
    //! 反向平移矩形框
    Box2d& operator-=(const Vector2d& vec)
    {
        return set(xmin - vec.x, ymin - vec.y, 
            xmax - vec.x, ymax - vec.y);
    }
    
    //! 矩形框交集的操作符函数
    Box2d operator&(const Box2d& box) const
    {
        return Box2d().intersectWith(*this, box);
    }
    
    //! 矩形框交集的操作符函数
    Box2d& operator&=(const Box2d& box)
    {
        return intersectWith(box);
    }
    
    //! 合并矩形框的操作符函数
    Box2d operator|(const Box2d& box) const
    {
        return Box2d().unionWith(*this, box);
    }
    
    //! 合并矩形框的操作符函数
    Box2d& operator|=(const Box2d& box)
    {
        return unionWith(box);
    }
    
    //! 得到各坐标分量乘以一个数后的新矩形框
    Box2d operator*(float s) const
    {
        return Box2d(xmin * s, ymin * s, xmax * s, ymax * s);
    }
    
    //! 各坐标分量乘以一个数
    Box2d& operator*=(float s)
    {
        return set(xmin * s, ymin * s, xmax * s, ymax * s);
    }
    
    //! 得到各坐标分量除以一个数后的新矩形框
    Box2d operator/(float s) const
    {
        s = 1 / s;
        return Box2d(xmin * s, ymin * s, xmax * s, ymax * s);
    }
    
    //! 各坐标分量除以一个数
    Box2d& operator/=(float s)
    {
        s = 1 / s;
        return set(xmin * s, ymin * s, xmax * s, ymax * s);
    }
    
    //! 矩阵变换，结果为规范化矩形
    Box2d operator*(const Matrix2d& m) const;
    
    //! 矩阵变换，结果为规范化矩形
    Box2d& operator*=(const Matrix2d& m);
    
    //! 判断两个矩形框是否相等
    bool operator==(const Box2d& box) const
    {
        return mgEquals(xmin, box.xmin)
            && mgEquals(ymin, box.ymin)
            && mgEquals(xmax, box.xmax)
            && mgEquals(ymax, box.ymax);
    }
    
    //! 判断两个矩形框是否不相等
    bool operator!=(const Box2d& box) const
    {
        return !operator==(box);
    }
#endif // SWIG
    
    //! 判断两个矩形框是否相等
    /*!
        \param box 另一个矩形框
        \param tol 判断的容差，用到其长度容差
        \return 如果相等，则返回true，否则返回false
    */
    bool isEqualTo(const Box2d& box, const Tol& tol = Tol::gTol()) const
    {
        return mgHypot(xmin - box.xmin, ymin - box.ymin) <= tol.equalPoint()
            && mgHypot(xmax - box.xmax, ymax - box.ymax) <= tol.equalPoint();
    }
};

#endif // TOUCHVG_RECT_H_
