// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import democmds.core.DemoCmdsGate;
import android.app.Activity;
import android.content.Context;
import android.util.Log;
import touchvg.view.GraphView;
import touchvg.view.ViewHelper;
import vgtest.app.R;

public class GraphView1 extends GraphView {
    private static final int[] HANDLEIDS = {
        R.drawable.vgdot1, R.drawable.vgdot2, R.drawable.vgdot3,
        R.drawable.vg_lock, R.drawable.vg_unlock, 0 };
    
    static {
        System.loadLibrary("democmds");
    }
    
    public GraphView1(Context context) {
        super(context);
        
        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final ViewHelper helper = new ViewHelper(this);
        
        ViewHelper.setContextButtonImages(null, R.array.vg_action_captions, null, HANDLEIDS);
        
        if ((flags & 32) != 0) {
            helper.addShapesForTest();
            flags = flags & ~32;
        }
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
        else if ((flags >> 1) == 4) {
            int n = DemoCmdsGate.registerCmds(helper.cmdViewHandle());
            helper.setCommand("hittest");
            Log.d("Test", "DemoCmdsGate.registerCmds = " + n
                    + ", setCommand: " + helper.getCommand());
        }
    }
}
