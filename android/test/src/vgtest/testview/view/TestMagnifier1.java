//! \file TestMagnifier1.java
//! \brief 测试放大镜的布局视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.view.SFGraphView;
import rhcad.touchvg.view.StdGraphView;
import vgtest.testview.TestFlags;
import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.widget.LinearLayout;

//! 测试放大镜的布局视图类
public class TestMagnifier1 extends LinearLayout {

    public TestMagnifier1(Context context) {
        super(context);
        this.setOrientation(HORIZONTAL);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");

        final LayoutParams param = new LayoutParams(LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT);
        param.weight = 1;

        final GraphView1 view = new GraphView1(context);
        addView(view, param);

        final StdGraphView magView = new StdGraphView(context, view);
        addView(magView, param);
        magView.setBackgroundColor(Color.GREEN);

        if ((flags & TestFlags.TWO_MAGVIEWS) != 0) {
            final SFGraphView1 view2 = new SFGraphView1(context);
            addView(view2, param);
            addView(view2.createDynamicShapeView(context), param);

            final SFGraphView magView2 = new SFGraphView(context, view2);
            addView(magView2, param);
            magView2.setBackgroundColor(Color.BLUE);
            addView(magView2.createDynamicShapeView(context), param);
        }
    }

}
