// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import java.util.ArrayList;
import java.util.List;

import rhcad.touchvg.view.BaseGraphView;

//! 记录当前视图和所有视图对象的辅助类
public class ViewUtil {
    private static BaseGraphView mActiveView;
    private static List<BaseGraphView> mViews = new ArrayList<BaseGraphView>();

    private ViewUtil() {
    }

    public static void onAddView(BaseGraphView view) {
        mViews.add(view);
        if (mActiveView == null) {
            mActiveView = view;
        }
    }

    public static void onRemoveView(BaseGraphView view) {
        mViews.remove(view);
        if (mActiveView == view) {
            mActiveView = mViews.isEmpty() ? null : mViews.get(0);
        }
    }

    public static List<BaseGraphView> views() {
        return mViews;
    }

    public static BaseGraphView activeView() {
        return mActiveView;
    }

    public static void activateView(BaseGraphView view) {
        if (mActiveView != view) {
            mActiveView = view;
        }
    }
}
