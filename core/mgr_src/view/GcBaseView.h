//! \file GcBaseView.h
//! \brief 定义内核视图基类 GcBaseView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_BASEVIEW_H
#define TOUCHVG_CORE_BASEVIEW_H

#include "gigesture.h"
#include "mgcmd.h"
#include "mgshapedoc.h"

class GiView;

//! 内核视图基类
/*! \ingroup CORE_VIEW
 */
class GcBaseView
{
public:
    GcBaseView(MgView* mgview, GiView *view);
    
    //! 析构函数
    virtual ~GcBaseView() {}
    
    //! 返回回调视图对象
    GiView* deviceView() { return _view; }
    
    MgView* cmdView() { return _mgview; }
    MgShapeDoc* doc();
    MgShapes* shapes();
    
    //! 得到坐标系对象
    GiTransform* xform() { return &_xf; }
    
    //! 得到图形显示对象
    GiGraphics* graph() { return &_gs; }
    
    //! 显示所有图形
    virtual int drawAll(GiGraphics& gs);

    //! 显示新图形，在 GiView.regenAppend() 后调用
    virtual int drawAppend(const int* newids, GiGraphics& gs);
    
    //! 显示动态图形
    virtual void dynDraw(const MgMotion& motion, GiGraphics& gs);

    //! 设置视图的宽高
    virtual void onSize(int dpi, int w, int h);
    
    //! 传递单指触摸手势消息
    virtual bool onGesture(const MgMotion& motion);

    //! 传递双指移动手势(可放缩旋转)
    virtual bool twoFingersMove(const MgMotion& motion);

private:
    MgView*     _mgview;
    GiView*     _view;
    GiTransform _xf;
    GiGraphics  _gs;

    Point2d     _lastCenter;
    float       _lastScale;
};

#endif // TOUCHVG_CORE_BASEVIEW_H
