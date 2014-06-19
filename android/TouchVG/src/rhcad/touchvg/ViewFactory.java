//! \file ViewFactory.java
//! \brief Android绘图视图工厂类
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg;

import android.view.ViewGroup;
import rhcad.touchvg.core.CmdObserver;
import rhcad.touchvg.view.ViewHelperImpl;

//! 绘图视图工厂类
public class ViewFactory {

    static {
        System.loadLibrary("touchvg");
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
            if (layout != null && layout.getChildCount() > 0) {
                if (ViewGroup.class.isInstance(layout.getChildAt(0))) {
                    return createHelper((ViewGroup) layout.getChildAt(0));
                }
            }
            return createHelper((IGraphView) layout.getChildAt(0));
        } catch (ClassCastException e) {
        }
        return null;
    }

    //! 注册命令观察者
    public static void registerCmdObserver(IViewHelper hlp, CmdObserver observer) {
        ((ViewHelperImpl)hlp).registerCmdObserver(observer);
    }

    //! 注销命令观察者
    public static void unregisterCmdObserver(IViewHelper hlp, CmdObserver observer) {
        ((ViewHelperImpl)hlp).unregisterCmdObserver(observer);
    }
}
