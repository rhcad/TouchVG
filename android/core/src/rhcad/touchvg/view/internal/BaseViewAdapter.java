package rhcad.touchvg.view.internal;

import java.util.ArrayList;

import android.util.Log;

import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.GraphView.CommandChangedListener;
import rhcad.touchvg.view.GraphView.ContentChangedListener;
import rhcad.touchvg.view.GraphView.DynamicChangedListener;
import rhcad.touchvg.view.GraphView.SelectionChangedListener;

//! 视图回调适配器
public abstract class BaseViewAdapter extends GiView {
    public static final int START_UNDO = 1;
    public static final int START_RECORD = 2;
    public static final int START_PLAY = 3;
    protected static final String TAG = "touchvg";
    private ContextAction mAction;
    private boolean mActionEnabled = true;
    private ArrayList<CommandChangedListener> commandChangedListeners;
    private ArrayList<SelectionChangedListener> selectionChangedListeners;
    private ArrayList<ContentChangedListener> contentChangedListeners;
    private ArrayList<DynamicChangedListener> dynamicChangedListeners;
    protected RecordRunnable mUndoing;
    protected RecordRunnable mRecorder;
    private boolean mDrawStopping = false;

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
            for (CommandChangedListener listener : commandChangedListeners) {
                listener.onCommandChanged(getGraphView());
            }
        }
    }

    @Override
    public void selectionChanged() {
        if (selectionChangedListeners != null) {
            for (SelectionChangedListener listener : selectionChangedListeners) {
                listener.onSelectionChanged(getGraphView());
            }
        }
    }

    @Override
    public void contentChanged() {
        if (contentChangedListeners != null) {
            for (ContentChangedListener listener : contentChangedListeners) {
                listener.onContentChanged(getGraphView());
            }
        }
    }

    @Override
    public void dynamicChanged() {
        if (dynamicChangedListeners != null) {
            for (DynamicChangedListener listener : dynamicChangedListeners) {
                listener.onDynamicChanged(getGraphView());
            }
        }
    }

    public void setOnCommandChangedListener(CommandChangedListener listener) {
        if (this.commandChangedListeners == null)
            this.commandChangedListeners = new ArrayList<CommandChangedListener>();
        this.commandChangedListeners.add(listener);
    }

    public void setOnSelectionChangedListener(SelectionChangedListener listener) {
        if (this.selectionChangedListeners == null)
            this.selectionChangedListeners = new ArrayList<SelectionChangedListener>();
        this.selectionChangedListeners.add(listener);
    }

    public void setOnContentChangedListener(ContentChangedListener listener) {
        if (this.contentChangedListeners == null)
            this.contentChangedListeners = new ArrayList<ContentChangedListener>();
        this.contentChangedListeners.add(listener);
    }

    public void setOnDynamicChangedListener(DynamicChangedListener listener) {
        if (this.dynamicChangedListeners == null)
            this.dynamicChangedListeners = new ArrayList<DynamicChangedListener>();
        this.dynamicChangedListeners.add(listener);
    }

    private GiCoreView coreView() {
        return getGraphView().coreView();
    }

    public boolean isStopping() {
        return mDrawStopping;
    }

    public void stop() {
        mDrawStopping = true;
        coreView().stopDrawing(this);
        stopRecord(true);
        stopRecord(false);
    }

    public boolean startRecord(String path, int type) {
        if (type == START_UNDO ? (mUndoing != null) : (mRecorder != null))
            return false;

        synchronized (coreView()) {
            int doc = type < START_PLAY ? coreView().acquireFrontDoc() : 0;
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

    public void stopRecord(boolean forUndo) {
        if (forUndo && mUndoing != null) {
            mUndoing.stop();
            mUndoing = null;
        }
        if (!forUndo && mRecorder != null) {
            mRecorder.stop();
            mRecorder = null;
        }
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
                    mCoreView.releaseDoc(doc);
                    mCoreView.releaseShapes(shapes);
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
            coreView().stopRecord(mType == START_UNDO);
            for (int i = 0; i < mPending.length; i++) {
                if (mPending[i] != 0) {
                    mCoreView.releaseDoc(mPending[i + 1]);
                    mCoreView.releaseShapes(mPending[i + 2]);
                }
            }
            mCoreView = null;
        }

        @Override
        public void run() {
            while (!isStopping()) {
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if (!isStopping())
                    process();
            }
            synchronized (mPending) {
                mPending.notify();
            }
            Log.d(TAG, "RecordRunnable exit: " + mType);
        }

        private boolean isStopping() {
            return mStopping || mDrawStopping;
        }

        protected void process() {
            int tick = 0, doc = 0, shapes = 0;
            boolean loop = true;

            while (loop && !isStopping()) {
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
                            mCoreView.releaseShapes(shapes);
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
