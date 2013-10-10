//! \file giview.h
//! \brief 定义设备相关的视图接口 GiView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_GIVIEW_H
#define TOUCHVG_CORE_GIVIEW_H

#include "mgvector.h"

//! 设备相关的视图回调接口
/*! 在派生类中使用某种界面框架实现其视图回调函数
    \ingroup CORE_VIEW
 */
class GiView
{
public:
    virtual ~GiView() {}

    //! 标记视图待重新构建显示
    virtual void regenAll() = 0;

    //! 标记视图待追加显示新图形
    virtual void regenAppend() = 0;

    //! 标记视图待更新显示
    virtual void redraw() = 0;

    //! 使用手指(true)或鼠标(false)交互
    virtual bool useFinger() { return true; }

    //! 返回上下文操作菜单是否可见
    virtual bool isContextActionsVisible() { return false; }

    //! 显示上下文操作菜单
    virtual bool showContextActions(const mgvector<int>& actions,
                                    const mgvector<float>& buttonXY,
                                    float x, float y, float w, float h) {
        return actions.count() && buttonXY.count() && x == y && w == h;
    }

    virtual void commandChanged() {}        //!< 当前命令已改变
    virtual void selectionChanged() {}      //!< 选择集改变的通知
    virtual void contentChanged() {}        //!< 图形数据改变的通知
};

#endif // TOUCHVG_CORE_GIVIEW_H
