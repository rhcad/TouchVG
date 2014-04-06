// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.ViewFactory;
import rhcad.touchvg.view.StdGraphView;
import vgtest.testview.TestFlags;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.widget.Toast;
import democmds.core.DemoCmdsGate;

public class GraphView1 extends StdGraphView implements IGraphView.OnFirstRegenListener {
    protected static final String PATH = "mnt/sdcard/TouchVG/";

    static {
        System.loadLibrary("democmds");
    }

    public GraphView1(Context context) {
        this(context, null);
    }

    public GraphView1(Context context, Bundle savedInstanceState) {
        super(context, savedInstanceState);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final IViewHelper helper = ViewFactory.createHelper(this);

        if ((flags & TestFlags.RAND_SHAPES) != 0) {
            helper.addShapesForTest();
        }
        if (savedInstanceState == null && (flags & TestFlags.RECORD) != 0) {
            setOnFirstRegenListener(this);
        }

        flags = flags & TestFlags.CMD_MASK;
        if (flags == TestFlags.SELECT_CMD) {
            helper.setCommand("select");
        } else if (flags == TestFlags.SPLINES_CMD) {
            helper.setCommand("splines");
        } else if (flags == TestFlags.LINE_CMD) {
            helper.setCommand("line");
        } else if (flags == TestFlags.LINES_CMD) {
            helper.setCommand("lines");
        } else if (flags == TestFlags.HITTEST_CMD) {
            int n = DemoCmdsGate.registerCmds(helper.cmdViewHandle());
            helper.setCommand("hittest");
            Log.d("Test", "DemoCmdsGate.registerCmds = " + n + ", " + helper.getCommand());
        }
    }

    @Override
    public boolean onPreDoubleTap(MotionEvent e) {
        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        final IViewHelper helper = ViewFactory.createHelper(this);

        if ((flags & TestFlags.SWITCH_CMD) != 0) {
            helper.switchCommand();
            Toast.makeText(getContext(), helper.getCommand(), Toast.LENGTH_SHORT).show();
            return true;
        }

        return false;
    }

    public void onFirstRegen(IGraphView view) {
        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        final IViewHelper helper = ViewFactory.createHelper(view);

        if ((flags & TestFlags.RECORD) != 0) {
            helper.startRecord(PATH + "record");
        }
    }
}
