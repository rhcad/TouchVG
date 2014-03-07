//! \file TestDoubleViews.java
//! \brief 测试双视图布局的测试视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import vgtest.testview.TestFlags;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.widget.Button;
import android.widget.FrameLayout;

//! 测试双视图布局的测试视图类
public class TestDoubleViews extends FrameLayout {

    public TestDoubleViews(Context context) {
        super(context);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final LayoutParams params = new LayoutParams(LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT);

        if ((flags & 0x08) != 0) { // 主视图使用GraphViewCached
            final GraphViewCached view = new GraphViewCached(context);
            addView(view, params);

            if ((flags & 0x02) != 0) { // 动态视图使用普通View
                final DynDrawStdView dynview = new DynDrawStdView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            } else if ((flags & 0x04) != 0) { // 动态视图使用SurfaceView
                final DynDrawSfView dynview = new DynDrawSfView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            }
        } else if ((flags & 0x01) == 0) { // 主视图使用普通View
            final GraphView view = new GraphView(context);
            addView(view, params);

            if ((flags & 0x02) != 0) { // 动态视图使用普通View
                final DynDrawStdView dynview = new DynDrawStdView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            } else if ((flags & 0x04) != 0) { // 动态视图使用SurfaceView
                final DynDrawSfView dynview = new DynDrawSfView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            }
        } else { // 主视图使用SurfaceView
            final GraphSfView view = new GraphSfView(context);

            if ((flags & TestFlags.MODEL_SURFACE) == 0) {
                view.setBackgroundColor(Color.TRANSPARENT);
            }
            addView(view, params);

            if ((flags & 0x02) != 0) { // 动态视图使用普通View
                final DynDrawStdView dynview = new DynDrawStdView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            } else if ((flags & 0x04) != 0) { // 动态视图使用SurfaceView
                final DynDrawSfView dynview = new DynDrawSfView(context);
                view.setDynDrawView(dynview);
                addView(dynview, params);
            }
        }

        final Button midview = new Button(context);
        midview.setText("This is a button in the FrameLayout.");
        midview.setBackgroundColor(Color.GRAY);
        addView(midview, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
    }

}
