/**
 * \file ViewFactory.java
 * \brief Android绘图视图工厂类
 * Copyright (c) 2012-2014, https://github.com/rhcad/touchvg
 */

package rhcad.touchvg;

import rhcad.touchvg.core.CmdObserver;
import rhcad.touchvg.view.ViewHelperImpl;

//! 绘图视图工厂类
public class ViewFactory {

    //! 获取当前活动视图，构造辅助对象
    public static IViewHelper createHelper() {
        return new ViewHelperImpl();
    }

    //! 指定视图构造辅助对象
    public static IViewHelper createHelper(IGraphView view) {
        return new ViewHelperImpl(view);
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
