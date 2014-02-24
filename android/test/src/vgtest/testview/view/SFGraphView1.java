// Copyright (c) 2014, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.SFGraphView;
import rhcad.touchvg.view.ViewHelper;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.ViewGroup;

public class SFGraphView1 extends SFGraphView implements GraphView.OnFirstRegenListener {
    protected static final String PATH = "mnt/sdcard/TouchVG/";

    public SFGraphView1(Context context) {
        this(context, null);
    }

    public SFGraphView1(Context context, Bundle savedInstanceState) {
        super(context);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final ViewHelper helper = new ViewHelper(this);

        if ((flags & 32) != 0) {
            helper.addShapesForTest();
        }
        if (savedInstanceState == null && (flags & 64) != 0) {
            setOnFirstRegenListener(this);
        }

        flags = flags & 0x0F;
        if ((flags & 1) != 0) {
            helper.setCommand("select");
        } else if ((flags >> 1) == 1) {
            helper.setCommand("splines");
        } else if ((flags >> 1) == 2) {
            helper.setCommand("line");
        } else if ((flags >> 1) == 3) {
            helper.setCommand("lines");
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        if ((flags & 256) != 0) {
            ViewGroup layout = (ViewGroup) getParent();
            this.setBackgroundDrawable(layout.getBackground());
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
