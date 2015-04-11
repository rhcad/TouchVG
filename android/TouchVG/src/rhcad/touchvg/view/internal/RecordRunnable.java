// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.GiCoreView;
import android.util.Log;

public class RecordRunnable extends ShapeRunnable {
    public static final int TYPE = 2;
    protected BaseViewAdapter mViewAdapter;

    public RecordRunnable(BaseViewAdapter viewAdapter, String path) {
        super(path, TYPE, viewAdapter.coreView());
        this.mViewAdapter = viewAdapter;
    }

    @Override
    protected boolean beforeStopped() {
        synchronized (GiCoreView.class) {
            boolean ret = mViewAdapter.onStopped(this);
            if (ret) {
                synchronized (mCoreView) {
                    mCoreView.stopRecord(false);
                }
            }
            return ret;
        }
    }

    @Override
    protected void afterStopped(boolean normal) {
        mViewAdapter = null;
    }

    @Override
    protected void process(int tick, int change, int doc, int shapes) {
        if (!mCoreView.recordShapes(false, tick, change, doc, shapes, null,
                mViewAdapter.createRecordCallback(true))) {
            Log.e(TAG, "Fail to record shapes for playing, tick=" + tick
                    + ", doc=" + doc + ", shapes=" + shapes);
        }
    }
}
