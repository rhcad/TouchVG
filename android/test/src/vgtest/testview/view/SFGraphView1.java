// Copyright (c) 2014, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import touchvg.view.SFGraphView;
import touchvg.view.ViewHelper;
import android.app.Activity;
import android.content.Context;

public class SFGraphView1 extends SFGraphView {

    public SFGraphView1(Context context) {
        super(context);
        
        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final ViewHelper helper = new ViewHelper(this);
        
        if ((flags & 32) != 0) {
            helper.addShapesForTest();
        }
        flags = flags & 0x0F;
        if ((flags & 1) != 0) {
            helper.setCommand("select");
        }
        else if ((flags >> 1) == 1) {
            helper.setCommand("splines");
        }
        else if ((flags >> 1) == 2) {
            helper.setCommand("line");
        }
        else if ((flags >> 1) == 3) {
            helper.setCommand("lines");
        }
    }
}
