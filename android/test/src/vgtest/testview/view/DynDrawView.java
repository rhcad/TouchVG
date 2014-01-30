//! \file DynDrawView.java
//! \brief 动态绘图视图接口
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;

//! 动态绘图视图接口
public interface DynDrawView {

    //! 设置内核视图对象
    public void setCoreView(GiView viewAdapter, GiCoreView coreView);

    //! 返回是否正在显示
    public boolean isDrawing();

    //! 触发重绘
    public void doDraw();

    //! 返回显示所用时间, 毫秒
    public long getDrawnTime();

    //! 返回显示完时的时刻, SystemClock.uptimeMillis()
    public long getEndPaintTime();
}
