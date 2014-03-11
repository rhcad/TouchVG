// Copyright (c) 2014, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.ViewFactory;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.Matrix2d;
import rhcad.touchvg.core.MgShapes;
import rhcad.touchvg.core.Point2d;
import rhcad.touchvg.view.SFGraphView;
import vgtest.testview.TestFlags;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.ViewGroup;
import android.widget.Toast;

public class SFGraphView1 extends SFGraphView implements IGraphView.OnFirstRegenListener {
    protected static final String PATH = "mnt/sdcard/TouchVG/";

    public SFGraphView1(Context context) {
        this(context, null);
    }

    public SFGraphView1(Context context, Bundle savedInstanceState) {
        super(context, savedInstanceState);

        int flags = ((Activity) context).getIntent().getExtras().getInt("flags");
        final IViewHelper helper = ViewFactory.createHelper(this);

        if ((flags & TestFlags.RAND_SHAPES) != 0) {
            helper.addShapesForTest();
        }
        if (savedInstanceState == null && (flags & (TestFlags.RECORD | TestFlags.PROVIDER)) != 0) {
            setOnFirstRegenListener(this);
        }
        if ((flags & TestFlags.RECORD) != 0) {
            this.setOnPlayEndedListener(new OnPlayEndedListener() {
                public boolean onPlayWillEnd(IGraphView view) {
                    return false;
                }

                public void onPlayEnded(IGraphView view) {
                    Toast.makeText(getContext(), "Play has ended.", Toast.LENGTH_SHORT).show();
                }
            });
        }

        flags = flags & TestFlags.CMD_MASK;
        if ((flags & TestFlags.SELECT_CMD) != 0) {
            helper.setCommand("select");
        } else if (flags == TestFlags.SPLINES_CMD) {
            helper.setCommand("splines");
        } else if (flags == TestFlags.LINE_CMD) {
            helper.setCommand("line");
        } else if (flags == TestFlags.LINES_CMD) {
            helper.setCommand("lines");
        }
    }

    @Override
    protected void onAttachedToWindow() {
        super.onAttachedToWindow();

        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        if ((flags & TestFlags.HAS_BACKDRAWABLE) != 0) {
            ViewGroup layout = (ViewGroup) getParent();
            this.setBackgroundDrawable(layout.getBackground());
        }
    }

    public void onFirstRegen(IGraphView view) {
        int flags = ((Activity) getContext()).getIntent().getExtras().getInt("flags");
        final IViewHelper helper = ViewFactory.createHelper(view);

        if ((flags & TestFlags.RECORD) != 0) {
            if ((flags & TestFlags.CMD_MASK) != 0) {
                helper.startRecord(PATH + "record");
            } else {
                helper.startPlay(PATH + "record");
            }
        }
        if ((flags & TestFlags.PROVIDER) != 0) {
            helper.addPlayProvider(new PlayProvider() {

                public int provideFrame(IGraphView view, int tag, Object extra,
                        int hShapes, int tick, int lastTick) {
                    MgShapes shapes = MgShapes.fromHandle(hShapes);
                    Point2d center = (Point2d) extra;
                    int ret = 0;

                    if (shapes.getShapeCount() == 0) {
                        final GiCoreView cv = GiCoreView.createView(null, 0);
                        ret = cv.loadFromFile(PATH + "resume.vg") ? 1 : -1;
                        shapes.copyShapes(MgShapes.fromHandle(cv.backShapes()), false);
                        cv.delete();
                        center.set(shapes.getExtent().center());
                    } else if (tick > lastTick + 60 - tag * 10) {
                        float angle = (float) Math.toRadians(2.5);
                        shapes.transform(Matrix2d.rotation(angle, center));
                        ret = 1;
                    }

                    return ret;
                }

                public void onPlayEnded(IGraphView view, int tag, Object extra) {
                }
            }, 1, new Point2d());
        }
    }
}
