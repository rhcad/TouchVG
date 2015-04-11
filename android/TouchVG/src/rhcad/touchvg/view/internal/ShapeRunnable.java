// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.GiCoreView;
import android.os.SystemClock;
import android.util.Log;

public class ShapeRunnable implements Runnable {
    protected static final String TAG = "touchvg";
    protected String mPath;
    protected int mType;
    protected boolean mStopping = false;
    protected GiCoreView mCoreView;
    private Object mMonitor = new Object();
    protected int[] mPending = new int[60];

    public ShapeRunnable(String path, int type, GiCoreView coreView) {
        this.mPath = path;
        this.mType = type;
        this.mCoreView = coreView;
        coreView.addRef();
    }

    public String getPath() {
        return mPath;
    }

    public int getType() {
        return mType;
    }

    public static int getTick() {
        return (int) SystemClock.elapsedRealtime();
    }

    public final void stop() {
        final LogHelper log = new LogHelper();
        mStopping = true;
        requestProcess();
        synchronized (mMonitor) {
            try {
                mMonitor.wait(1000);
            } catch (InterruptedException e) {
                Log.w(TAG, "wait stop", e);
            }
        }
        log.r();
    }

    public final void requestProcess() {
        synchronized (this) {
            this.notify();
        }
    }

    public final void requestRecord(int command) {
        requestRecord(command, 0, 0, 0);
    }

    public final void requestRecord(int tick, int change, int doc, int shapes) {
        boolean released = false;

        synchronized (mPending) {
            int i = 0;
            while (i < mPending.length && mPending[i] != 0) {
                i += 4;
            }
            if (i + 3 < mPending.length) {
                mPending[i] = tick;
                mPending[i + 1] = change;
                mPending[i + 2] = doc;
                mPending[i + 3] = shapes;
            } else {
                GiCoreView.releaseDoc(doc);
                GiCoreView.releaseShapes(shapes);
                released = true;
            }
        }
        if (tick != 0 && !released) {
            requestProcess();
        }
    }

    protected boolean beforeStopped() {
        return true;
    }

    protected void afterStopped(boolean normal) {
        Log.d(TAG, "empty afterStopped");
    }

    protected void process(int tick, int change, int doc, int shapes) {
        Log.d(TAG, "empty process");
    }

    @Override
    public void run() {
        while (!mStopping && waitProcess()) {
            try {
                process();
            } catch (Exception e) {
                Log.w(TAG, "ShapeRunnable run", e);
            }
        }

        boolean normal = beforeStopped();
        afterStopped(normal);
        cleanup();

        synchronized (mMonitor) {
            mMonitor.notify();
        }
    }

    private boolean waitProcess() {
        synchronized (this) {
            try {
                this.wait();
            } catch (InterruptedException e) {
                Log.w(TAG, "waitProcess", e);
            }
        }
        return !mStopping;
    }

    private void process() {
        int tick = 0, change = 0, doc = 0, shapes = 0;
        boolean loop = true;
        int nextTick = 0;

        while (loop && !mStopping && mCoreView != null) {
            synchronized (mPending) {
                while (loop) {
                    tick = mPending[0];
                    change = mPending[1];
                    doc = mPending[2];
                    shapes = mPending[3];
                    popFirst4Items();

                    nextTick = mPending[0];
                    loop = (doc == 0 && shapes != 0 && nextTick != 0);
                    if (loop) {
                        GiCoreView.releaseShapes(shapes);
                    }
                }
            }
            process(tick, change, doc, shapes);
            if (nextTick == 0) {
                synchronized (mPending) {
                    nextTick = mPending[0];
                }
            }
            loop = (nextTick != 0);
        }
    }

    private void popFirst4Items() {
        for (int i = 4; i + 3 < mPending.length; i += 4) {
            for (int j = 0; j < 4; j++) {
                mPending[i + j - 4] = mPending[i + j];
                mPending[i + j] = 0;
            }
        }
    }

    private void cleanup() {
        synchronized (mPending) {
            for (int i = 0; i < mPending.length; i++) {
                if (mPending[i] != 0) {
                    GiCoreView.releaseDoc(mPending[i + 2]);
                    GiCoreView.releaseShapes(mPending[i + 3]);
                }
            }
        }
        mCoreView.release();
        mCoreView = null;
        Log.d(TAG, "ShapeRunnable exit, type=" + mType);
    }
}
