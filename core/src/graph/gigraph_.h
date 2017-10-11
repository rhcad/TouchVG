//! \file gigraph_.h
//! \brief 定义GiGraphics类的内部成员
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "gigraph.h"
#include "gicanvas.h"
#include "gilock.h"

//! GiGraphics的内部实现类
class GiGraphicsImpl
{
public:
    enum { CLIP_INFLATE = 10 };

    GiTransform*  xform;            //!< 坐标系管理对象
    bool        needFreeXf;         //!< 是否自动释放 xform
    GiCanvas*   canvas;             //!< 显示适配器
    GiContext   ctx;                //!< 当前绘图参数
    int         ctxused;            //!< 画笔和画刷的设置标志
    GiColor     bkcolor;            //!< 背景色
    float       phase;              //!< 虚线其实偏移

    float       maxPenWidth;        //!< 最大像素线宽
    float       minPenWidth;        //!< 最小像素线宽

    long        lastZoomTimes;      //!< 记下的放缩结果改变次数
    volatile long   stopping;       //!< 是否需要停止绘图
    bool        isPrint;            //!< 是否打印或打印预览
    int         drawColors;         //!< 绘图DC颜色数
    RECT_2D     clipBox0;           //!< 开始绘图时的剪裁框(LP)

    RECT_2D     clipBox;            //!< 剪裁框(LP)
    Box2d       rectDraw;           //!< 剪裁矩形，比clipBox略大
    Box2d       rectDrawM;          //!< 剪裁矩形，模型坐标
    Box2d       rectDrawW;          //!< 剪裁矩形，世界坐标
    Box2d       rectDrawMaxM;       //!< 最大剪裁矩形，模型坐标
    Box2d       rectDrawMaxW;       //!< 最大剪裁矩形，世界坐标

    GiGraphicsImpl(GiTransform* x, bool needFree)
        : xform(x), needFreeXf(needFree), canvas((GiCanvas*)0)
    {
        drawColors = 0;
        stopping = 0;
        isPrint = false;
        ctxused = 0;
        bkcolor = GiColor::White();
        phase = -1;
        maxPenWidth = 100;
        minPenWidth = 1;
    }

    ~GiGraphicsImpl()
    {
        if (needFreeXf)
            delete xform;
    }

    void zoomChanged()
    {
        rectDrawM = rectDraw * xform->displayToModel();
        rectDrawMaxM = xform->getWndRectM();
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
