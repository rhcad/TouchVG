//! \file gipath.h
//! \brief 定义矢量路径类 GiPath
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_PATH_H_
#define TOUCHVG_PATH_H_

#include "mgpnt.h"

class GiPathImpl;

//! 矢量路径节点类型
/*! \see GiPath
*/
typedef enum {
    kGiCloseFigure = 1,
    kGiLineTo = 2,
    kGiBeziersTo = 4,
    kGiMoveTo = 6,
} GiPathNode;

//! 矢量路径类
/*!
    \ingroup GRAPH_INTERFACE
    \see GiGraphics, GiPathNode
*/
class GiPath
{
public:
    //! 默认构造函数
    GiPath();

    //! 拷贝构造函数
    GiPath(const GiPath& src);

    //! 构造函数，指定节点数据
    /*!
        \param count 节点个数
        \param points 节点坐标数组，元素个数为count
        \param types 节点类型数组，元素个数为count，由 GiPathNode 值组成
    */
    GiPath(int count, const Point2d* points, const char* types);

    //! 析构函数
    virtual ~GiPath();

    //! 赋值函数
    GiPath& copy(const GiPath& src);
    
    //! 折线拐角圆角化
    /*!
        \param count 折线顶点数
        \param points 顶点坐标数组，元素个数为count
        \param radius 拐角最大圆角半径
        \param closed 是否闭合
        \return 是否生成了路径
    */
    bool genericRoundLines(int count, const Point2d* points, 
        float radius, bool closed = false);

    //! 返回节点个数
    int getCount() const;

    //! 返回节点坐标数组
    const Point2d* getPoints() const;

    //! 返回节点类型数组，由 GiPathNode 值组成
    const char* getTypes() const;
    

    //! 清除所有节点
    void clear();

    //! 对所有节点坐标进行坐标变换
    void transform(const Matrix2d& mat);

    //! 开始新的一个图元
    /*! 必须调用 moveTo 后才能调用 lineTo 等其他添加绘图指令的函数
    */
    void startFigure();

    //! 添加移动到新位置的指令节点
    /*! 必须调用 moveTo 后才能调用 lineTo 等其他添加绘图指令的函数
        \param point 新位置
        \return 是否正确添加
    */
    bool moveTo(const Point2d& point);

    //! 添加绘制直线段到新位置的指令节点
    /*!
        \param point 直线段的终点位置
        \return 是否正确添加
    */
    bool lineTo(const Point2d& point);

    //! 添加绘制折线到新位置的指令节点
    /*!
        \param count 折线点数，不含起点，至少为1
        \param points 折线顶点数组，不含起点，元素个数为count
        \return 是否正确添加
    */
    bool linesTo(int count, const Point2d* points);

    //! 添加绘制三次贝塞尔曲线到新位置的指令节点
    /*!
        \param count 曲线控制点的点数，不含起点，必须为3的倍数
        \param points 曲线控制点的位置的数组，不含起点，元素个数为count
        \param reverse 反序添加控制点
        \return 是否正确添加
    */
    bool beziersTo(int count, const Point2d* points, bool reverse = false);

    //! 添加绘制切线圆弧到新位置的指令节点
    /*! 新圆弧的起点为图元的当前位置，起始切向方向为最末一段直线段的方向。\n
        最末节点是用 lineTo 或 linesTo 添加的直线段时，本函数才具有切线作用。
        \param point 圆弧终点
        \return 是否正确添加
    */
    bool arcTo(const Point2d& point);

    //! 添加绘制三点圆弧到新位置的指令节点
    /*! 新圆弧的起点为图元的当前位置，经过中间一点到达指定终点
        \param point 圆弧中间经过的一点
        \param end 圆弧终点
        \return 是否正确添加
    */
    bool arcTo(const Point2d& point, const Point2d& end);

    //! 添加闭合当前图元的指令节点
    /*!
        \return 是否正确添加
    */
    bool closeFigure();

private:
    GiPathImpl*   m_data;
};

#endif // TOUCHVG_PATH_H_
