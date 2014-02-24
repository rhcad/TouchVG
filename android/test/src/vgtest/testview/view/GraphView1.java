// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.StdGraphView;
import rhcad.touchvg.view.ViewHelper;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import democmds.core.DemoCmdsGate;

public class GraphView1 extends StdGraphView implements GraphView.OnFirstRegenListener {
    protected static final String PATH = "mnt/sdcard/TouchVG/";

    static {
        System.loadLibrary("democmds");
    }

    public GraphView1(Context context) {
        this(context, null);
    }

    public GraphView1(Context context, Bundle savedInstanceState) {
        super(context);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final ViewHelper hlp = new ViewHelper(this);

        if ((flags & 32) != 0) {
            hlp.addShapesForTest();
        }
        if (savedInstanceState == null && (flags & 64) != 0) {
            setOnFirstRegenListener(this);
        }

        flags = flags & 0x0F;
        if ((flags & 1) != 0) {
            hlp.setCommand("select");
        } else if ((flags >> 1) == 1) {
            hlp.setCommand("splines");
        } else if ((flags >> 1) == 2) {
            hlp.setCommand("line");
        } else if ((flags >> 1) == 3) {
            hlp.setCommand("lines");
        } else if ((flags >> 1) == 4) {
            int n = DemoCmdsGate.registerCmds(hlp.cmdViewHandle());
            hlp.setCommand("hittest");
            Log.d("Test", "DemoCmdsGate.registerCmds = " + n + ", " + hlp.getCommand());
        }
    }

    public void onFirstRegen(GraphView view) {
        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        final ViewHelper helper = new ViewHelper(this);

        if (flags == 64) {
            helper.startPlay(PATH + "record");
        } else if ((flags & 64) != 0) {
            helper.startRecord(PATH + "record");
        }
    }
}
