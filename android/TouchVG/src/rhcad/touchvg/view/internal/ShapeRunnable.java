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
    protected int[] mPending = new int[60]; // {tick,doc,shapes}

    public ShapeRunnable(String path, int type, GiCoreView coreView) {
        this.mPath = path;
        this.mType = type;
        this.mCoreView = coreView;
        coreView.addRef();
    }

    protected void finalize() {
        Log.d(TAG, "ShapeRunnable finalize, type=" + mType);
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
                e.printStackTrace();
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
        requestRecord(command, 0, 0);
    }

    public final void requestRecord(int tick, int doc, int shapes) {
        synchronized (mPending) {
            int i = 0;
            for (; i < mPending.length && mPending[i] != 0; i += 3) {
            }
            if (i + 2 < mPending.length) {
                mPending[i] = tick;
                mPending[i + 1] = doc;
                mPending[i + 2] = shapes;
            } else {
                GiCoreView.releaseDoc(doc);
                GiCoreView.releaseShapes(shapes);
                tick = 0;
                doc = 0;
                shapes = 0;
            }
        }
        if (tick != 0) {
            requestProcess();
        }
    }

    protected boolean beforeStopped() {
        return true;
    }

    protected void afterStopped(boolean normal) {
    }

    protected void process(int tick, int doc, int shapes) {
    }

    @Override
    public void run() {
        while (!mStopping && waitProcess()) {
            try {
                process();
            } catch (Exception e) {
                e.printStackTrace();
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
                e.printStackTrace();
            }
        }
        return !mStopping;
    }

    private void process() {
        int tick = 0, doc = 0, shapes = 0;
        boolean loop = true;
        int nextTick = 0;

        while (loop && !mStopping && mCoreView != null) {
            synchronized (mPending) {
                while (loop) {
                    tick = mPending[0];
                    doc = mPending[1];
                    shapes = mPending[2];

                    for (int i = 3; i < mPending.length; i++) {
                        mPending[i - 3] = mPending[i];
                        mPending[i] = 0;
                    }
                    nextTick = mPending[0];
                    loop = (doc == 0 && shapes != 0 && nextTick != 0);
                    if (loop) {
                        GiCoreView.releaseShapes(shapes);
                    }
                }
            }
            process(tick, doc, shapes);
            if (nextTick == 0) {
                synchronized (mPending) {
                    nextTick = mPending[0];
                }
            }
            loop = (nextTick != 0);
        }
    }

    private void cleanup() {
        synchronized (mPending) {
            for (int i = 0; i < mPending.length; i++) {
                if (mPending[i] != 0) {
                    GiCoreView.releaseDoc(mPending[i + 1]);
                    GiCoreView.releaseShapes(mPending[i + 2]);
                }
            }
        }
        mCoreView.release();
        mCoreView = null;
        Log.d(TAG, "ShapeRunnable exit, type=" + mType);
    }
}
