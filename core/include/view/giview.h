//! \file giview.h
//! \brief 定义设备相关的视图接口 GiView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_GIVIEW_H
#define TOUCHVG_CORE_GIVIEW_H

#include "mgvector.h"

struct MgStringCallback;

//! 设备相关的视图回调接口
/*! 在派生类中使用某种界面框架实现其视图回调函数
    \ingroup CORE_VIEW
 */
class GiView
{
public:
    virtual ~GiView() {}

    //! 标记视图待重新构建显示
    virtual void regenAll(bool changed) {}

    //! 标记视图待追加显示新图形
    virtual void regenAppend(int sid, long playh) {}

    //! 标记视图待更新显示
    virtual void redraw(bool changed) {}

    //! 使用手指(true)或鼠标(false)交互
    virtual bool useFinger() { return true; }

    //! 返回上下文操作菜单是否可见
    virtual bool isContextActionsVisible() { return false; }

    //! 显示上下文操作菜单(MgContextAction)
    virtual bool showContextActions(const mgvector<int>& actions,
                                    const mgvector<float>& buttonXY,
                                    float x, float y, float w, float h) { return false; }
    
    virtual void hideContextActions() {}    //!< 隐藏上下文操作菜单

    virtual void commandChanged() {}        //!< 当前命令已改变
    virtual void selectionChanged() {}      //!< 选择集改变的通知
    virtual void contentChanged() {}        //!< 图形数据改变的通知
    virtual void dynamicChanged() {}        //!< 图形动态拖拉改变的通知
    virtual void zoomChanged() {}           //!< 放缩改变的通知
    virtual void viewChanged(GiView* oldview) {}    //!< 当前视图改变的通知
    virtual void shapeWillDelete(int sid) {}        //!< 图形将删除的通知
    virtual void shapeDeleted(int sid) {}           //!< 删除图形的通知
    virtual bool shapeDblClick(int type, int sid, int tag) { return false; } //!< 通知图形双击编辑
    
    //! 图形点击的通知，返回false继续显示上下文按钮
    virtual bool shapeClicked(int type, int sid, int tag, float x, float y) { return false; }
    virtual void showMessage(const char* text) {}   //!< 显示提示文字
    //! 得到本地化文字内容
    virtual void getLocalizedString(const char* name, MgStringCallback* c) {}
};

#endif // TOUCHVG_CORE_GIVIEW_H
