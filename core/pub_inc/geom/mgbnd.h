//! \file mgbnd.h
//! \brief 定义绑定框类 BoundBox
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_BOUNDBOX_H_
#define TOUCHVG_BOUNDBOX_H_

#include "mgpnt.h"

//! 绑定框类，平行四边形
/*!
    \ingroup GEOM_CLASS
*/
class BoundBox
{
public:
    //! 构造收缩到原点的绑定框
    BoundBox() : m_base(Point2d::kOrigin())
        , m_dir1(Vector2d::kIdentity()), m_dir2(Vector2d::kIdentity())
    {
    }
    
    //! 构造收缩到线段的绑定框
    BoundBox(const Point2d& pnt1, const Point2d& pnt2)
        : m_base(pnt1), m_dir1(pnt2 - pnt1), m_dir2(Vector2d::kIdentity())
    {
    }
    
    //! 给定一个顶点和两个边构造平行四边形
    BoundBox(const Point2d& base,
        const Vector2d& dir1, const Vector2d& dir2)
        : m_base(base), m_dir1(dir1), m_dir2(dir2)
    {
    }
    
    //! 拷贝构造函数
    BoundBox(const BoundBox& box)
        : m_base(box.m_base), m_dir1(box.m_dir1), m_dir2(box.m_dir2)
    {
    }
    
    //! 得到绑定框坐标范围的两个对角点坐标
    void getMinMaxPoints(Point2d& pnt1, Point2d& pnt2) const
    {
        pnt1.x = mgMin(m_base.x, mgMin(m_base.x + m_dir1.x, m_base.x + m_dir2.x));
        pnt1.y = mgMin(m_base.y, mgMin(m_base.y + m_dir1.y, m_base.y + m_dir2.y));
        pnt2.x = mgMax(m_base.x, mgMax(m_base.x + m_dir1.x, m_base.x + m_dir2.x));
        pnt2.y = mgMax(m_base.y, mgMax(m_base.y + m_dir1.y, m_base.y + m_dir2.y));
    }
    
    //! 得到绑定框的一个顶点和两个边
    void get(Point2d& base, Vector2d& dir1, Vector2d& dir2) const
    {
        base = m_base;
        dir1 = m_dir1;
        dir2 = m_dir2;
    }
    
    //! 设置为收缩到线段的绑定框
    BoundBox& set(const Point2d& pnt1, const Point2d& pnt2)
    {
        m_base = pnt1;
        m_dir1 = pnt2 - pnt1;
        m_dir2 = Vector2d::kIdentity();
        return *this;
    }
    
    //! 给定一个顶点和两个边，设置为平行四边形
    BoundBox& set(const Point2d& base, 
        const Vector2d& dir1, const Vector2d& dir2)
    {
        m_base = base;
        m_dir1 = dir1;
        m_dir2 = dir2;
        return *this;
    }
    
    //! 扩大到包含一个点
    BoundBox& extend(const Point2d& pnt);
    
    //! 向外扩大给定长度
    BoundBox& swell(float distance);
    
    //! 判断是否包含一个点
    bool contains(const Point2d& pnt) const;
    
    //! 判断是否和另一个绑定框分离，即不相交
    bool isDisjoint(const BoundBox& box) const;
    
    //! 判断是否为坐标矩形
    bool isRect() const
    {
        return mgIsZero(m_dir1.y) && mgIsZero(m_dir2.x);
    }
    
    //! 将平行四边形设置为坐标矩形
    BoundBox& setToRect()
    {
        Point2d pt1, pt2;
        getMinMaxPoints(pt1, pt2);
        m_base = pt1;
        m_dir1.set(pt2.x - pt1.x, 0);
        m_dir2.set(0, pt2.y - pt1.y);
        return *this;
    }
    
private:
    Point2d     m_base;
    Vector2d    m_dir1;
    Vector2d    m_dir2;
};

#endif // TOUCHVG_BOUNDBOX_H_
