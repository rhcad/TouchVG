// Copyright (c) 2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.internal.ImageCache;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.view.View;

//! 绘图视图接口
public interface GraphView {

    //! 返回内核视图分发器对象
    public GiCoreView coreView();

    //! 返回视图回调适配器对象
    public GiView viewAdapter();

    //! 返回视图回调适配器对象
    public View getView();

    //! 创建动态绘图子视图
    public View createDynamicShapeView(Context context);

    //! 返回图像对象缓存
    public ImageCache getImageCache();

    //! 释放临时缓存
    public void clearCachedData();

    //! 所属的Activity暂停时调用
    public void onPause();

    //! 是否允许上下文操作
    public void setContextActionEnabled(boolean enabled);

    //! 设置是否允许触摸交互
    public void setGestureEnable(boolean enabled);

    //! 传递单指触摸事件，可用于拖放操作
    public boolean onTouchDrag(int action, float x, float y);

    //! 传递单指轻击事件，可用于拖放操作
    public boolean onTap(float x, float y);

    //! 设置背景图，本视图不透明时使用
    public void setBackgroundDrawable(Drawable background);

    //! 得到静态图形的快照
    public Bitmap snapshot(boolean transparent);

    //! 当前命令改变的通知
    public interface OnCommandChangedListener {
        void onCommandChanged(GraphView view);
    }

    //! 图形选择集改变的通知
    public interface OnSelectionChangedListener {
        void onSelectionChanged(GraphView view);
    }

    //! 图形数据改变的通知
    public interface OnContentChangedListener {
        void onContentChanged(GraphView view);
    }

    //! 图形动态改变的通知
    public interface OnDynamicChangedListener {
        void onDynamicChanged(GraphView view);
    }

    //! 添加当前命令改变的观察者
    public void setOnCommandChangedListener(OnCommandChangedListener listener);

    //! 添加图形选择集改变的观察者
    public void setOnSelectionChangedListener(OnSelectionChangedListener listener);

    //! 添加图形数据改变的观察者
    public void setOnContentChangedListener(OnContentChangedListener listener);

    //! 添加图形动态改变的观察者
    public void setOnDynamicChangedListener(OnDynamicChangedListener listener);
}
