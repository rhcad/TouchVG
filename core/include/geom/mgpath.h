//! \file mgpath.h
//! \brief 定义矢量路径类 MgPath
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_PATH_H_
#define TOUCHVG_PATH_H_

#include "mgpnt.h"

struct MgPathImpl;
class Box2d;

//! 矢量路径节点类型
/*! \see MgPath
*/
typedef enum {
    kMgCloseFigure = 1,
    kMgLineTo = 2,
    kMgBezierTo = 4,
    kMgQuadTo = 8,
    kMgMoveTo = 6,
} MgPathNode;

//! 矢量路径类
/*!
    \ingroup GRAPH_INTERFACE
    \see GiGraphics, MgPathNode
*/
class MgPath
{
public:
    //! 默认构造函数
    MgPath();

    //! 拷贝构造函数
    MgPath(const MgPath& src);

    //! 构造函数，指定节点数据
    /*!
        \param count 节点个数
        \param points 节点坐标数组，元素个数为count
        \param types 节点类型数组，元素个数为count，由 MgPathNode 值组成；每个字符也可为 mlcqLCQ 之一，大写字符表示闭合
    */
    MgPath(int count, const Point2d* points, const char* types);
    
    //! 给定SVG的路径d串构造
    MgPath(const char* svgd);

    //! 析构函数
    virtual ~MgPath();

    //! 赋值函数
    MgPath& copy(const MgPath& src);
    
    //! 追加路径
    MgPath& append(const MgPath& src);
    
    //! 从SVG的路径d串解析
    MgPath& addSVGPath(const char* s);
    
    //! 路径反向
    MgPath& reverse();
    
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
    
    //! 返回子路径(MoveTo)的个数
    int getSubPathCount() const;
    
    //! 返回起始点
    Point2d getStartPoint() const;
    
    //! 返回起始切线矢量
    Vector2d getStartTangent() const;
    
    //! 返回最末点
    Point2d getEndPoint() const;
    
    //! 返回终止切线矢量
    Vector2d getEndTangent() const;
    
    //! 返回是否为线段
    bool isLine() const;
    
    //! 返回是否为线段、折线或多边形
    bool isLines() const;
    
    //! 返回是否每段都为曲线段
    bool isCurve() const;
    
    //! 返回是否为闭合图形
    bool isClosed() const;
    
    //! 返回路径长度
    float getLength() const;

#ifndef SWIG
    //! 返回节点坐标数组
    const Point2d* getPoints() const;

    //! 返回节点类型数组，由 MgPathNode 值组成
    const char* getTypes() const;
    
    //! 设置节点数据，types的每个字符由 MgPathNode 值组成或为 mlcqLCQ 之一
    void setPath(int count, const Point2d* points, const char* types);
    void setPath(int count, const Point2d* points, const int* types);
    
    //! 遍历路径段的回调接口
    struct MgSegmentCallback {
        virtual ~MgSegmentCallback() {}
        virtual void beginSubPath() {}
        virtual void endSubPath(bool closed) {}
        virtual bool processLine(int startIndex, int &endIndex, const Point2d& startpt, const Point2d& endpt) = 0;
        virtual bool processBezier(int startIndex, int &endIndex, const Point2d* pts) = 0;
    };
    //! 遍历路径段
    bool scanSegments(MgSegmentCallback& c) const;
#endif
    
    //! 返回节点类型，由 MgPathNode 值组成
    int getNodeType(int index) const;
    
    //! 得到节点坐标，超过点数则取余数
    Point2d getPoint(int index) const;
    
    //! 设置节点坐标
    void setPoint(int index, const Point2d& pt);

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
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool moveTo(const Point2d& point, bool rel = false);

    //! 添加绘制直线段到新位置的指令节点
    /*!
        \param point 直线段的终点位置
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool lineTo(const Point2d& point, bool rel = false);
    
    //! 添加绘制水平线段到新位置的指令节点
    bool horzTo(float x, bool rel = false);
    
    //! 添加绘制垂直线段到新位置的指令节点
    bool vertTo(float y, bool rel = false);

    //! 添加绘制折线到新位置的指令节点
    /*!
        \param count 折线点数，不含起点，至少为1
        \param points 折线顶点数组，不含起点，元素个数为count
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool linesTo(int count, const Point2d* points, bool rel = false);

    //! 添加绘制三次贝塞尔曲线到新位置的指令节点
    /*!
        \param count 曲线控制点的点数，不含起点，必须为3的倍数
        \param points 曲线控制点的位置的数组，不含起点，元素个数为count
        \param reverse 反序添加控制点
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool beziersTo(int count, const Point2d* points, bool reverse = false, bool rel = false);
    
    //! 添加绘制三次贝塞尔曲线到新位置的指令节点
    bool bezierTo(const Point2d& cp1, const Point2d& cp2, const Point2d& end, bool rel = false);
    
    //! 添加绘制光滑三次贝塞尔曲线到新位置的指令节点
    bool smoothBezierTo(const Point2d& cp2, const Point2d& end, bool rel = false);

    //! 添加绘制二次贝塞尔曲线到新位置的指令节点
    /*!
        \param count 曲线控制点的点数，不含起点，必须为2的倍数
        \param points 曲线控制点的位置的数组，不含起点，元素个数为count
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool quadsTo(int count, const Point2d* points, bool rel = false);
    
    //! 添加绘制二次贝塞尔曲线到新位置的指令节点
    bool quadTo(const Point2d& cp, const Point2d& end, bool rel = false);
    
    //! 添加绘制光滑二次贝塞尔曲线到新位置的指令节点
    bool smoothQuadTo(const Point2d& end, bool rel = false);

    //! 添加绘制切线圆弧到新位置的指令节点
    /*! 新圆弧的起点为图元的当前位置，起始切向方向为最末一段直线段的方向。\n
        最末节点是用 lineTo 或 linesTo 添加的直线段时，本函数才具有切线作用。
        \param point 圆弧终点
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool arcTo(const Point2d& point, bool rel = false);

    //! 添加绘制三点圆弧到新位置的指令节点
    /*! 新圆弧的起点为图元的当前位置，经过中间一点到达指定终点
        \param point 圆弧中间经过的一点
        \param end 圆弧终点
        \param rel 是否是相对坐标
        \return 是否正确添加
    */
    bool arcTo(const Point2d& point, const Point2d& end, bool rel = false);

    //! 添加闭合当前图元的指令节点
    /*!
        \return 是否正确添加
    */
    bool closeFigure();
    
    //! 从起始方向去掉给定距离的段，即最终路径的起点与原起点的距离为dist
    bool trimStart(const Point2d& pt, float dist);
    
    //! 求两个路径的交点
    bool crossWithPath(const MgPath& path, const Box2d& box, Point2d& ptCross) const;

private:
    MgPathImpl*   m_data;
};

#endif // TOUCHVG_PATH_H_
