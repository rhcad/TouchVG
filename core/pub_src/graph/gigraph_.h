//! \file gigraph_.h
//! \brief 定义GiGraphics类的内部成员
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "gigraph.h"
#include "gicanvas.h"
#include "gilock.h"

//! GiGraphics的内部实现类
class GiGraphicsImpl
{
public:
    enum { CLIP_INFLATE = 10 };

    GiTransform*  xform;            //!< 坐标系管理对象
    GiCanvas*   canvas;             //!< 显示适配器
    GiContext   ctx;                //!< 当前绘图参数
    int         ctxused;            //!< 画笔和画刷的设置标志
    GiColor     bkcolor;            //!< 背景色

    float       maxPenWidth;        //!< 最大像素线宽
    float       minPenWidth;        //!< 最小像素线宽

    long        lastZoomTimes;      //!< 记下的放缩结果改变次数
    long        drawRefcnt;         //!< 绘图锁定计数
    bool        isPrint;            //!< 是否打印或打印预览
    int         drawColors;         //!< 绘图DC颜色数
    RECT_2D     clipBox0;           //!< 开始绘图时的剪裁框(LP)

    RECT_2D     clipBox;            //!< 剪裁框(LP)
    Box2d       rectDraw;           //!< 剪裁矩形，比clipBox略大
    Box2d       rectDrawM;          //!< 剪裁矩形，模型坐标
    Box2d       rectDrawW;          //!< 剪裁矩形，世界坐标
    Box2d       rectDrawMaxM;       //!< 最大剪裁矩形，模型坐标
    Box2d       rectDrawMaxW;       //!< 最大剪裁矩形，世界坐标

    GiGraphicsImpl(GiTransform* x) : xform(x), canvas(NULL)
    {
        drawRefcnt = 0;
        drawColors = 0;
        isPrint = false;
        ctxused = 0;
        bkcolor = GiColor::White();
        maxPenWidth = 100;
        minPenWidth = 1;
    }

    ~GiGraphicsImpl()
    {
    }

    void zoomChanged()
    {
        rectDrawM = rectDraw * xform->displayToModel();
        Box2d rect (0, 0, xform->getWidth(), xform->getHeight());
        rectDrawMaxM = rect * xform->displayToModel();
        rectDrawW = rectDrawM * xform->modelToWorld();
        rectDrawMaxW = rectDrawMaxM * xform->modelToWorld();
        if (canvas) {
            canvas->clearCachedBitmap(true);
        }
    }

private:
    GiGraphicsImpl();
    void operator=(const GiGraphicsImpl&);
};

//! 图形系统的绘图引用锁定辅助类
class GiLock
{
    long*  m_refcount;
public:
    GiLock(long* refcount) : m_refcount(refcount)
    {
        giInterlockedIncrement(m_refcount);
    }
    ~GiLock()
    {
        giInterlockedDecrement(m_refcount);
    }
};
