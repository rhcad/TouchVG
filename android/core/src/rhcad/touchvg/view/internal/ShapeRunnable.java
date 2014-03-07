package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.GiCoreView;
import android.os.SystemClock;
import android.util.Log;

public class ShapeRunnable implements Runnable {
    protected static final String TAG = "touchvg";
    protected String mPath;
    protected int mType;
    protected GiCoreView mCoreView;
    protected boolean mStopping = false;
    protected int[] mPending = new int[60]; // {tick,doc,shapes}
    protected int[] mPendingLock = new int[1];

    public ShapeRunnable(String path, int type, GiCoreView coreView) {
        this.mPath = path;
        this.mType = type;
        this.mCoreView = coreView;
    }

    protected void finalize() {
        Log.d(TAG, "ShapeRunnable finalize, type=" + mType);
    }

    public static int getTick() {
        return (int)SystemClock.elapsedRealtime();
    }

    public String getPath() {
        return mPath;
    }

    public int getType() {
        return mType;
    }

    public void requestRecord() {
        synchronized (this) {
            this.notify();
        }
    }

    public void requestRecord(int command) {
        requestRecord(command, 0, 0);
    }

    public void requestRecord(int tick, int doc, int shapes) {
        synchronized (mPendingLock) {
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
            requestRecord();
        }
    }

    public void stop() {
        final LogHelper log = new LogHelper();
        mStopping = true;
        requestRecord();
        synchronized (mPending) {
            try {
                mPending.wait(1000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        log.r();
    }

    protected boolean stopRecord() {
        return true;
    }

    protected void afterStopped(boolean normal) {
    }

    protected void process(int tick, int doc, int shapes) {
    }

    @Override
    public void run() {
        while (!mStopping) {
            synchronized (this) {
                try {
                    this.wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            if (!mStopping) {
                try {
                    process();
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        }

        boolean normal = stopRecord();

        for (int i = 0; i < mPending.length; i++) {
            if (mPending[i] != 0) {
                GiCoreView.releaseDoc(mPending[i + 1]);
                GiCoreView.releaseShapes(mPending[i + 2]);
            }
        }
        afterStopped(normal);
        mCoreView = null;

        synchronized (mPending) {
            mPending.notify();
        }
        Log.d(TAG, "RecordRunnable exit, type=" + mType);
    }

    protected void process() {
        int tick = 0, doc = 0, shapes = 0;
        boolean loop = true;

        while (loop && !mStopping && mCoreView != null) {
            synchronized (mPendingLock) {
                while (loop) {
                    tick = mPending[0];
                    doc = mPending[1];
                    shapes = mPending[2];

                    for (int i = 3; i < mPending.length; i++) {
                        mPending[i - 3] = mPending[i];
                        mPending[i] = 0;
                    }
                    loop = (doc == 0 && shapes != 0 && mPending[0] != 0);
                    if (loop) {
                        GiCoreView.releaseShapes(shapes);
                    }
                }
            }
            process(tick, doc, shapes);
            loop = (mPending[0] != 0);
        }
    }
}
