//! \file BaseGraphView.java
//! \brief 绘图视图接口
// Copyright (c) 2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.internal.ImageCache;

//! 绘图视图接口
public interface BaseGraphView extends IGraphView {

    //! 返回视图回调适配器对象
    public GiView viewAdapter();

    //! 返回图像对象缓存
    public ImageCache getImageCache();
}
