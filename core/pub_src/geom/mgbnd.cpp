// mgbnd.cpp: 实现绑定框类BoundBox
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbnd.h"

// 扩大到包含一个点
BoundBox& BoundBox::extend(const Point2d& pnt)
{
    Vector2d vec = pnt - m_base;
    
    // 如果绑定框为点，就扩大到线段
    if (m_dir1.isZeroVector() && m_dir2.isZeroVector())
        m_dir1 = vec;
    // 如果该点和绑定框原点重合
    else if (vec.isZeroVector())
        return *this;
    // 如果绑定框两边平行或一边为零
    else if (m_dir1.isParallelTo(m_dir2))
    {
        // m_dir1取为非空边
        if (m_dir1.isZeroVector())
        {
            Vector2d v = m_dir1;
            m_dir1 = m_dir2;
            m_dir2 = v;
        }
        
        // 求两条边在m_dir1上的投影
        float proj1 = 1.f;
        float proj2 = m_dir2.projectScaleToVector(m_dir1);
        if (proj2 > 0.f)
        {
            if (proj2 > proj1)
                proj1 = proj2;
            proj2 = 0.f;
        }
        
        // 求vec在m_dir1上的投影
        Vector2d vecProj, vecPerp;
        float projv = vec.projectResolveVector(m_dir1, vecProj, vecPerp);
        
        // 扩大两条边在m_dir1上的投影以包含指定点
        if (projv > proj1)
            proj1 = projv;
        else if (projv < proj2)
            proj2 = projv;
        
        // 计算两条边
        m_dir2 = m_dir1 * proj2 + vecPerp;
        if (proj1 > 1.f)
            m_dir1 *= proj1;
    }
    else
    {
        Vector2d uv;
        vec.resolveVector(m_dir1, m_dir2, uv);
        if (uv.x > 1.f)
            m_dir1 *= uv.x;
        if (uv.y > 1.f)
            m_dir2 *= uv.y;
        if (uv.x < 0.f) {
            m_base += m_dir1 * uv.x;
            m_dir1 *= (1.f - uv.x);
        }
        if (uv.y < 0.f) {
            m_base += m_dir2 * uv.y;
            m_dir2 *= (1.f - uv.y);
        }
    }
    return *this;
}

// 向外扩大给定长度
BoundBox& BoundBox::swell(float distance)
{
    if (m_dir1.isZeroVector() && m_dir2.isZeroVector())
    {
        distance = fabsf(distance);
        m_base.x -= distance;
        m_base.y -= distance;
        m_dir1.set(2.f * distance, 0.f);
        m_dir2.set(0.f, 2.f * distance);
    }
    else if (m_dir1.isParallelTo(m_dir2))
    {
        // m_dir1取为非空边
        if (m_dir1.isZeroVector())
        {
            Vector2d v = m_dir1;
            m_dir1 = m_dir2;
            m_dir2 = v;
        }
        distance = fabsf(distance) / m_dir1.length();
        
        // 求两条边在m_dir1上的投影
        float proj1 = 1.f;
        float proj2 = m_dir2.projectScaleToVector(m_dir1);
        if (proj2 > 0.f)
        {
            if (proj2 > proj1)
                proj1 = proj2;
            proj2 = 0.f;
        }
        proj1 += distance;
        proj2 -= distance;
        
        m_base.x += proj2 * m_dir1.x + distance * m_dir1.y;
        m_base.y += proj2 * m_dir1.y - distance * m_dir1.x;
        m_dir2.x = -2.f * distance * m_dir1.y;
        m_dir2.y =  2.f * distance * m_dir1.x;
        m_dir1 *= proj1 - proj2;
    }
    else
    {
        float len1 = m_dir1.length();
        float len2 = m_dir2.length();
        if (len1 + distance < 0.f)
            distance = -len1;
        else if (len2 + distance < 0.f)
            distance = -len2;
        
        m_base -= m_dir1 * (distance / len1);
        m_base -= m_dir2 * (distance / len2);
        m_dir1 *= 1.f + 2.f * distance / len1;
        m_dir2 *= 1.f + 2.f * distance / len2;
    }
    return *this;
}

// 判断是否包含一个点
bool BoundBox::contains(const Point2d& pnt) const
{
    Vector2d uv;
    Vector2d vec (pnt - m_base);
    
    if (m_dir1.isZeroVector() && m_dir2.isZeroVector())
        return vec.isZeroVector();
    else if (m_dir1.isParallelTo(m_dir2))
    {
        uv.x = vec.projectScaleToVector(m_dir1);
        if (uv.x < -_MGZERO || uv.x > 1.f + _MGZERO)
            return false;
        uv.y = vec.projectScaleToVector(m_dir2);
        return uv.y > -_MGZERO && uv.y < 1.f + _MGZERO;
    }
    else
    {
        vec.resolveVector(m_dir1, m_dir2, uv);
        return uv.x > -_MGZERO && uv.x < 1.f + _MGZERO
            && uv.y > -_MGZERO && uv.y < 1.f + _MGZERO;
    }
}

// 判断是否和另一个绑定框不相交
bool BoundBox::isDisjoint(const BoundBox& box) const
{
    if (box.m_dir1.isZeroVector() && box.m_dir2.isZeroVector())
        return !contains(box.m_base);
    else if (box.m_dir1.isParallelTo(box.m_dir2))
    {
        return !contains(box.m_base)
            && !contains(box.m_base + box.m_dir1)
            && !contains(box.m_base + box.m_dir2);
    }
    else if (m_dir1.isZeroVector() && m_dir2.isZeroVector())
        return !box.contains(m_base);
    else if (m_dir1.isParallelTo(m_dir2))
    {
        return !box.contains(m_base)
            && !box.contains(m_base + m_dir1)
            && !box.contains(m_base + m_dir2);
    }
    else
    {
        // TODO:
    }
    return false;
}
