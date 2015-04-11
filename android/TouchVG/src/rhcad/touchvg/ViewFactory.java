//! \file ViewFactory.java
//! \brief Android绘图视图工厂类
// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg;

import android.util.Log;
import android.view.ViewGroup;
import rhcad.touchvg.core.CmdObserver;
import rhcad.touchvg.view.ViewHelperImpl;

/**
 * \ingroup GROUP_ANDROID
 * 绘图视图工厂类
 */
public class ViewFactory {
    private static final String TAG = "touchvg";

    static {
        System.loadLibrary(TAG);
    }

    private ViewFactory() {
    }

    //! 获取当前活动视图，构造辅助对象
    public static IViewHelper createHelper() {
        return new ViewHelperImpl();
    }

    //! 指定视图构造辅助对象
    public static IViewHelper createHelper(IGraphView view) {
        return new ViewHelperImpl(view);
    }

    //! 从布局中查找绘图视图构造辅助对象，查不到则返回null
    public static IViewHelper createHelper(ViewGroup layout) {
        try {
            if (layout != null && layout.getChildCount() > 0
                    && ViewGroup.class.isInstance(layout.getChildAt(0))) {
                return createHelper((ViewGroup) layout.getChildAt(0));
            }
            return createHelper((IGraphView) layout.getChildAt(0));
        } catch (ClassCastException e) {
            Log.w(TAG, "The layout is not kind of IGraphView", e);
            return null;
        }
    }

    //! 注册命令观察者
    public static void registerCmdObserver(IViewHelper hlp, CmdObserver observer) {
        ((ViewHelperImpl)hlp).registerCmdObserver(observer);
    }

    //! 注销命令观察者
    public static void unregisterCmdObserver(IViewHelper hlp, CmdObserver observer) {
        ((ViewHelperImpl)hlp).unregisterCmdObserver(observer);
    }

    //! 注册命令观察者
    public static void registerCmdObserver(IGraphView view, CmdObserver observer) {
        new ViewHelperImpl(view).registerCmdObserver(observer);
    }

    //! 注销命令观察者
    public static void unregisterCmdObserver(IGraphView view, CmdObserver observer) {
        new ViewHelperImpl(view).unregisterCmdObserver(observer);
    }
}
