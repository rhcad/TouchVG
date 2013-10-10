//! \file TestMagnifier1.java
//! \brief 测试放大镜的布局视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.widget.LinearLayout;
import touchvg.view.GraphView;

//! 测试放大镜的布局视图类
public class TestMagnifier1 extends LinearLayout {

    public TestMagnifier1(Context context) {
        super(context);
        this.setOrientation(VERTICAL);
        
        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        
        final LayoutParams param = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        param.weight = 1;
        
        final GraphView1 view = new GraphView1(context);
        addView(view, param);
        
        final GraphView magView = new GraphView(context, view);
        addView(magView, param);
        magView.setBackgroundColor(Color.GREEN);
        
        if ((flags & 16) != 0) {
            final GraphView1 view2 = new GraphView1(context);
            addView(view2, param);
            
            final GraphView magView2 = new GraphView(context, view2);
            addView(magView2, param);
            magView2.setBackgroundColor(Color.BLUE);
        }
    }

}
