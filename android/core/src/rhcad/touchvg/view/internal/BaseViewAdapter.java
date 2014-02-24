package rhcad.touchvg.view.internal;

import java.util.ArrayList;

import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.GraphView.OnCommandChangedListener;
import rhcad.touchvg.view.GraphView.OnContentChangedListener;
import rhcad.touchvg.view.GraphView.OnDynamicChangedListener;
import rhcad.touchvg.view.GraphView.OnFirstRegenListener;
import rhcad.touchvg.view.GraphView.OnSelectionChangedListener;
import android.app.Activity;
import android.util.Log;

//! 视图回调适配器
public abstract class BaseViewAdapter extends GiView {
    public static final int START_UNDO = 1;
    public static final int START_RECORD = 2;
    public static final int START_PLAY = 3;
    protected static final String TAG = "touchvg";
    private ContextAction mAction;
    private boolean mActionEnabled = true;
    private ArrayList<OnCommandChangedListener> commandChangedListeners;
    private ArrayList<OnSelectionChangedListener> selectionChangedListeners;
    private ArrayList<OnContentChangedListener> contentChangedListeners;
    private ArrayList<OnDynamicChangedListener> dynamicChangedListeners;
    private ArrayList<OnFirstRegenListener> firstRegenListeners;
    protected RecordRunnable mUndoing;
    protected RecordRunnable mRecorder;
    private int mRegenCount = 0;

    protected abstract GraphView getGraphView();

    protected abstract ContextAction createContextAction();

    public synchronized void delete() {
        if (mAction != null) {
            mAction.release();
            mAction = null;
        }
        super.delete();
    }

    public void removeContextButtons() {
        if (mAction != null) {
            mAction.removeButtonLayout();
        }
    }

    public void setContextActionEnabled(boolean enabled) {
        mActionEnabled = enabled;
    }

    @Override
    public boolean isContextActionsVisible() {
        return mAction != null && mAction.isVisible();
    }

    @Override
    public boolean showContextActions(Ints actions, Floats xy, float x, float y, float w, float h) {
        if ((actions == null || actions.count() == 0) && mAction == null) {
            return true;
        }
        if (!mActionEnabled) {
            return false;
        }
        if (mAction == null) {
            mAction = createContextAction();
        }
        return mAction.showActions(getGraphView().getView().getContext(), actions, xy);
    }

    @Override
    public void commandChanged() {
        if (commandChangedListeners != null) {
            for (OnCommandChangedListener listener : commandChangedListeners) {
                listener.onCommandChanged(getGraphView());
            }
        }
    }

    @Override
    public void selectionChanged() {
        if (selectionChangedListeners != null) {
            for (OnSelectionChangedListener listener : selectionChangedListeners) {
                listener.onSelectionChanged(getGraphView());
            }
        }
    }

    @Override
    public void contentChanged() {
        if (contentChangedListeners != null) {
            for (OnContentChangedListener listener : contentChangedListeners) {
                listener.onContentChanged(getGraphView());
            }
        }
    }

    @Override
    public void dynamicChanged() {
        if (dynamicChangedListeners != null) {
            for (OnDynamicChangedListener listener : dynamicChangedListeners) {
                listener.onDynamicChanged(getGraphView());
            }
        }
    }

    public void setOnCommandChangedListener(OnCommandChangedListener listener) {
        if (this.commandChangedListeners == null)
            this.commandChangedListeners = new ArrayList<OnCommandChangedListener>();
        this.commandChangedListeners.add(listener);
    }

    public void setOnSelectionChangedListener(OnSelectionChangedListener listener) {
        if (this.selectionChangedListeners == null)
            this.selectionChangedListeners = new ArrayList<OnSelectionChangedListener>();
        this.selectionChangedListeners.add(listener);
    }

    public void setOnContentChangedListener(OnContentChangedListener listener) {
        if (this.contentChangedListeners == null)
            this.contentChangedListeners = new ArrayList<OnContentChangedListener>();
        this.contentChangedListeners.add(listener);
    }

    public void setOnDynamicChangedListener(OnDynamicChangedListener listener) {
        if (this.dynamicChangedListeners == null)
            this.dynamicChangedListeners = new ArrayList<OnDynamicChangedListener>();
        this.dynamicChangedListeners.add(listener);
    }

    public void setOnFirstRegenListener(OnFirstRegenListener listener) {
        if (this.firstRegenListeners == null)
            this.firstRegenListeners = new ArrayList<OnFirstRegenListener>();
        this.firstRegenListeners.add(listener);
    }

    public void onFirstRegen() {
        if (++mRegenCount == 1) {
            final Activity activity = (Activity) getGraphView().getView().getContext();
            activity.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    if (firstRegenListeners != null) {
                        for (OnFirstRegenListener listener : firstRegenListeners) {
                            listener.onFirstRegen(getGraphView());
                        }
                        firstRegenListeners.clear();
                        firstRegenListeners = null;
                    }
                }
            });
        }
    }

    public int getRegenCount() {
        return mRegenCount;
    }

    private GiCoreView coreView() {
        return getGraphView().coreView();
    }

    public void stop() {
        stopRecord(true);
        stopRecord(false);
    }

    public void stopRecord(boolean forUndo) {
        synchronized (coreView()) {
            if (forUndo && mUndoing != null) {
                mUndoing.stop();
                mUndoing = null;
            }
            if (!forUndo && mRecorder != null) {
                mRecorder.stop();
                mRecorder = null;
            }
        }
    }

    public boolean startRecord(String path, int type) {
        if ((type == START_UNDO ? (mUndoing != null) : (mRecorder != null)))
            return false;

        synchronized (coreView()) {
            int doc = type < START_PLAY ? coreView().acquireFrontDoc() : 0;
            if (type < START_PLAY && doc == 0) {
                Log.e(TAG, "Fail to record shapes due to no front doc");
                return false;
            }
            if (!coreView().startRecord(path, doc, type == START_UNDO))
                return false;
        }

        if (type == START_UNDO) {
            mUndoing = new RecordRunnable(type);
            new Thread(mUndoing, "touchvg.undo").start();
        } else {
            mRecorder = new RecordRunnable(type);
            final String name = type == START_PLAY ? "touchvg.play" : "touchvg.record";
            new Thread(mRecorder, name).start();
        }

        return true;
    }

    public void undo() {
        if (mUndoing != null) {
            mUndoing.requestRecord(RecordRunnable.UNDO);
        }
    }

    public void redo() {
        if (mUndoing != null) {
            mUndoing.requestRecord(RecordRunnable.REDO);
        }
    }

    protected class RecordRunnable implements Runnable {
        private int mType;
        private GiCoreView mCoreView;
        protected boolean mStopping = false;
        protected int[] mPending = new int[60]; // {tick,doc,shapes}
        protected int[] mPendingLock = new int[1];
        public static final int UNDO = 0xFFFFFF10;
        public static final int REDO = 0xFFFFFF20;

        public RecordRunnable(int type) {
            this.mType = type;
            this.mCoreView = coreView();
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
            mStopping = true;
            requestRecord();
            synchronized (mPending) {
                try {
                    mPending.wait(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            mCoreView = null;
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
            mCoreView.stopRecord(mType == START_UNDO);
            for (int i = 0; i < mPending.length; i++) {
                if (mPending[i] != 0) {
                    GiCoreView.releaseDoc(mPending[i + 1]);
                    GiCoreView.releaseShapes(mPending[i + 2]);
                }
            }
            synchronized (mPending) {
                mPending.notify();
            }
            Log.d(TAG, "RecordRunnable exit: " + mType);
        }

        protected void process() {
            int tick = 0, doc = 0, shapes = 0;
            boolean loop = true;

            while (loop && !mStopping) {
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
                if (tick == UNDO) {
                    synchronized (mCoreView) {
                        mCoreView.undo(BaseViewAdapter.this);
                    }
                } else if (tick == REDO) {
                    synchronized (mCoreView) {
                        mCoreView.redo(BaseViewAdapter.this);
                    }
                } else {
                    mCoreView.recordShapes(mType == START_UNDO, tick, doc, shapes);
                }

                loop = (mPending[0] != 0);
            }
        }
    }
}
