//! \file IGraphView.java
//! \brief 绘图视图接口
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgMotion;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;

//! 绘图视图接口
public interface IGraphView {

    //! 返回视图回调适配器对象
    public View getView();

    //! 本视图为放大镜时返回对应的主视图，否则返回自己
    public IGraphView getMainView();

    //! 返回内核视图分发器对象
    public GiCoreView coreView();

    //! 返回内核视图的句柄, MgView 指针
    public int cmdViewHandle();

    //! 创建动态绘图子视图
    public View createDynamicShapeView(Context context);

    //! 释放临时缓存
    public void clearCachedData();

    //! 停止后台任务，所属的Activity销毁前可调用
    public void stop(OnViewDetachedListener listener);

    //! 暂停后台任务，所属的Activity暂停时可调用
    public boolean onPause();

    //! 恢复后台任务，所属的Activity恢复时可调用
    public boolean onResume();

    //! 是否允许上下文操作
    public void setContextActionEnabled(boolean enabled);

    //! 返回是否允许触摸交互
    public boolean getGestureEnabled();

    //! 设置是否允许触摸交互
    public void setGestureEnabled(boolean enabled);

    //! 传递单指触摸事件，可用于拖放操作
    public boolean onTouchDrag(int action, float x, float y);

    //! 传递单指轻击事件，可用于拖放操作
    public boolean onTap(float x, float y);

    //! 设置背景图，本视图不透明时使用
    public void setBackgroundDrawable(Drawable background);

    //! 得到静态图形的快照
    public Bitmap snapshot(boolean transparent);

    //! 得到静态图形的快照，支持多线程
    public Bitmap snapshot(int doc, int gs, boolean transparent);

    //! 当前命令改变的通知
    public static interface OnCommandChangedListener {
        public void onCommandChanged(IGraphView view);
    }

    //! 图形选择集改变的通知
    public static interface OnSelectionChangedListener {
        public void onSelectionChanged(IGraphView view);
    }

    //! 图形数据改变的通知
    public static interface OnContentChangedListener {
        public void onContentChanged(IGraphView view);
    }

    //! 图形动态改变的通知
    public static interface OnDynamicChangedListener {
        public void onDynamicChanged(IGraphView view);
    }

    //! 第一次后台渲染结束的通知
    public static interface OnFirstRegenListener {
        public void onFirstRegen(IGraphView view);
    }

    //! 图形录制的通知
    public static interface OnShapesRecordedListener {
        public void onShapesRecorded(IGraphView view, Bundle info);
    }

    //! 图形已删除的通知
    public static interface OnShapeDeletedListener {
        public void onShapeDeleted(IGraphView view, int sid);
    }

    //! 图形点击的通知，返回false继续显示上下文按钮
    public static interface OnShapeClickedListener {
        public boolean onShapeClicked(IGraphView view, int sid, int tag, float x, float y);
    }

    //! 上下文按钮点击的通知
    public static interface OnContextActionListener {
        public boolean onContextAction(IGraphView view, MgMotion sender, int action);
    }

    //! 绘图视图销毁后的通知
    public static interface OnViewDetachedListener {
        public void onGraphViewDetached();
    }

    //! 添加当前命令改变的观察者
    public void setOnCommandChangedListener(OnCommandChangedListener listener);

    //! 添加图形选择集改变的观察者
    public void setOnSelectionChangedListener(OnSelectionChangedListener listener);

    //! 添加图形数据改变的观察者
    public void setOnContentChangedListener(OnContentChangedListener listener);

    //! 添加图形动态改变的观察者
    public void setOnDynamicChangedListener(OnDynamicChangedListener listener);

    //! 添加第一次后台渲染结束的观察者
    public void setOnFirstRegenListener(OnFirstRegenListener listener);

    //! 添加图形录制的观察者
    public void setOnShapesRecordedListener(OnShapesRecordedListener listener);

    //! 添加图形已删除的观察者
    public void setOnShapeDeletedListener(OnShapeDeletedListener listener);

    //! 添加图形点击的观察者
    public void setOnShapeClickedListener(OnShapeClickedListener listener);

    //! 添加上下文按钮点击的观察者
    public void setOnContextActionListener(OnContextActionListener listener);
}
