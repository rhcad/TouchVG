// Copyright (c) 2014, https://github.com/rhcad/touchvg

package touchvg.view;

import touchvg.core.GiCoreView;
import touchvg.core.GiView;
import touchvg.view.internal.ImageCache;
import android.graphics.Bitmap;
import android.view.View;

//! 绘图视图接口
public interface GraphView {

    //! 返回内核视图分发器对象
    public GiCoreView coreView();

    //! 返回视图回调适配器对象
    public GiView viewAdapter();

    //! 返回视图回调适配器对象
    public View getView();

    //! 返回图像对象缓存
    public ImageCache getImageCache();

    //! 释放临时缓存
    public void clearCachedData();

    //! 设置背景色
    public void setBackgroundColor(int color);

    //! 是否允许上下文操作
    public void setContextActionEnabled(boolean enabled);

    //! 设置是否允许触摸交互
    public void setGestureEnable(boolean enabled);

    //! 传递单指触摸事件，可用于拖放操作
    public boolean onTouch(int action, float x, float y);

    //! 传递单指轻击事件，可用于拖放操作
    public boolean onTap(float x, float y);

    //! 得到静态图形的快照
    public Bitmap snapshot();

    //! 当前命令改变的通知
    public interface CommandChangedListener {
        void onCommandChanged(GraphView view);
    }

    //! 图形选择集改变的通知
    public interface SelectionChangedListener {
        void onSelectionChanged(GraphView view);
    }

    //! 图形数据改变的通知
    public interface ContentChangedListener {
        void onContentChanged(GraphView view);
    }

    //! 图形动态改变的通知
    public interface DynamicChangedListener {
        void onDynamicChanged(GraphView view);
    }

    //! 添加当前命令改变的观察者
    public void setOnCommandChangedListener(CommandChangedListener listener);

    //! 添加图形选择集改变的观察者
    public void setOnSelectionChangedListener(SelectionChangedListener listener);

    //! 添加图形数据改变的观察者
    public void setOnContentChangedListener(ContentChangedListener listener);

    //! 添加图形动态改变的观察者
    public void setOnDynamicChangedListener(DynamicChangedListener listener);
}
