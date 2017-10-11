//! \file giplclip.h
//! \brief 定义多边形剪裁类 PolygonClip
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_POLYGONCLIP_H_
#define TOUCHVG_POLYGONCLIP_H_

#include <vector>
using std::vector;

// 返回STL数组(vector)变量的元素个数
template<class T> inline static int getSize(T& arr)
{
    return static_cast<int>(arr.size());
}

//! 多边形剪裁类
class PolygonClip
{
    const Box2d     m_rect;         //!< 剪裁矩形
    vector<Point2d> m_vs1;          //!< 剪裁交点缓冲
    vector<Point2d> m_vs2;          //!< 剪裁交点缓冲
    bool            m_closed;       //!< 是否闭合
    
public:
    
    //! 构造函数
    /*!
        \param rect 剪裁矩形，必须为规范化的矩形
        \param closed 将要传入的坐标序列是多边形还是折线
    */
    PolygonClip(const Box2d& rect, bool closed = true)
        : m_rect(rect), m_closed(closed)
    {
    }
    
    //! 剪裁一个多边形
    /*!
        \param count 顶点个数
        \param points 顶点坐标数组, 个数为count
        \param mat 可指定要对给定顶点进行坐标变换的矩阵
        \return 剪裁结果中是否有轮廓边
    */
    bool clip(int count, const Point2d* points, const Matrix2d* mat = (const Matrix2d*)0)
    {
        if (count < 2 || !points)
            return false;
        
        if (mat)
        {
            m_vs1.resize(2+count/2);
            m_vs2.resize(count);
            Point2d* p = &m_vs2.front();
            for (int i=0; i < count; i++)
                p[i] = points[i] * (*mat);
            points = p;
        }
        else
        {
            m_vs1.resize(2+count/2);
            m_vs2.resize(2+count/2);
        }
        if (!clipEdge(m_vs1, count, points, LEFT) ||
            !clipEdge(m_vs2, getSize(m_vs1), &m_vs1.front(), TOP) ||
            !clipEdge(m_vs1, getSize(m_vs2), &m_vs2.front(), RIGHT) ||
            !clipEdge(m_vs2, getSize(m_vs1), &m_vs1.front(), BOTTOM))
        {
            return false;
        }

        m_vs1.clear();

        return true;
    }
    
    //! 返回剪裁结果中的顶点个数
    int getCount() const
    {
        return getSize(m_vs2);
    }
    
    //! 返回剪裁结果中的顶点数组
    const Point2d* getPoints() const
    {
        return &m_vs2.front();
    }
    
    //! 返回剪裁结果中的指定序号的顶点坐标
    /*!
        \param index 顶点的序号, >=0, 自动取到顶点个数范围内
        \return 对应的顶点坐标
    */
    const Point2d& getPoint(int index) const
    {
        return m_vs2[index % m_vs2.size()];
    }
    
    //! 判断剪裁后两相邻顶点是否构成轮廓边
    /*!
        \param index 剪裁后的边(可见或不可见的)的序号, 自动取到[0, getCount()-1]范围内, 
            为( getCount()-1 )时相当于是首尾顶点的连线
        \return 该边是否构成轮廓边, 即可见边
    */
    bool isLinked(int index) const
    {
        const Point2d& p1 = getPoint(index);
        const Point2d& p2 = getPoint(index+1);
        
        // 和剪裁边重合
        if (mgEquals(p1.x, p2.x) && (mgEquals(p1.x, m_rect.xmin) || mgEquals(p1.x, m_rect.xmax)))
            return false;
        if (mgEquals(p1.y, p2.y) && (mgEquals(p1.y, m_rect.ymin) || mgEquals(p1.y, m_rect.ymax)))
            return false;

        return true;
    }
    
private:
    
    void operator=(const PolygonClip&);

    enum kSideType { LEFT, TOP, RIGHT, BOTTOM };
    
    bool clipEdge(vector<Point2d>& arr, int count, const Point2d* points, kSideType side)
    {
        if (count < 2 || !points)
            return false;

        arr.clear();
        output(arr, points[m_closed ? count-1 : 0], points[0], side);

        for (int i=1; i < count; i++)
        {
            output(arr, points[i-1], points[i], side);
        }

        return (arr.size() >= 2);
    }
    
    void output(vector<Point2d>& arr, const Point2d& p1, const Point2d& p2, kSideType side)
    {
        switch (side)
        {
        case LEFT:      clipLeft(arr, p1, p2); break;
        case TOP:       clipTop(arr, p1, p2); break;
        case RIGHT:     clipRight(arr, p1, p2); break;
        case BOTTOM:    clipBottom(arr, p1, p2); break;
        }
    }
    
    void clipLeft(vector<Point2d>& arr, const Point2d& p1, const Point2d& p2)
    {
        float y;
        
        // 如果线段与剪裁线相交，则输出交点
        if ((p1.x < m_rect.xmin) != (p2.x < m_rect.xmin))
        {
            y = p1.y + (m_rect.xmin - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
            arr.push_back(Point2d(m_rect.xmin, y));
        }
        
        // 如果终点在可见侧，则输出终点
        if (p2.x >= m_rect.xmin)
        {
            arr.push_back(p2);
        }
    }
    
    void clipTop(vector<Point2d>& arr, const Point2d& p1, const Point2d& p2)
    {
        float x;
        
        if ((p1.y < m_rect.ymin) != (p2.y < m_rect.ymin))
        {
            x = p1.x + (m_rect.ymin - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            arr.push_back(Point2d(x, m_rect.ymin));
        }
        if (p2.y >= m_rect.ymin)
        {
            arr.push_back(p2);
        }
    }
    
    void clipRight(vector<Point2d>& arr, const Point2d& p1, const Point2d& p2)
    {
        float y;
        
        if ((p1.x > m_rect.xmax) != (p2.x > m_rect.xmax))
        {
            y = p1.y + (m_rect.xmax - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
            arr.push_back(Point2d(m_rect.xmax, y));
        }
        if (p2.x <= m_rect.xmax)
        {
            arr.push_back(p2);
        }
    }
    
    void clipBottom(vector<Point2d>& arr, const Point2d& p1, const Point2d& p2)
    {
        float x;
        
        if ((p1.y > m_rect.ymax) != (p2.y > m_rect.ymax))
        {
            x = p1.x + (m_rect.ymax - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            arr.push_back(Point2d(x, m_rect.ymax));
        }
        if (p2.y <= m_rect.ymax)
        {
            arr.push_back(p2);
        }
    }
};

#endif // TOUCHVG_POLYGONCLIP_H_
