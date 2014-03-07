package rhcad.touchvg.view.internal;

import java.util.ArrayList;

import rhcad.touchvg.IGraphView.OnCommandChangedListener;
import rhcad.touchvg.IGraphView.OnContentChangedListener;
import rhcad.touchvg.IGraphView.OnDynamicChangedListener;
import rhcad.touchvg.IGraphView.OnFirstRegenListener;
import rhcad.touchvg.IGraphView.OnSelectionChangedListener;
import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.core.MgFindImageCallback;
import rhcad.touchvg.core.MgStringCallback;
import rhcad.touchvg.view.BaseGraphView;
import android.app.Activity;
import android.os.Bundle;
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
    protected UndoRunnable mUndoing;
    protected RecordRunnable mRecorder;
    private int mRegenCount = 0;
    private Bundle mSavedState;

    protected abstract BaseGraphView getGraphView();

    protected abstract ContextAction createContextAction();

    public synchronized void delete() {
        Log.d(TAG, "delete BaseViewAdapter " + getCPtr(this));
        if (mAction != null) {
            mAction.release();
            mAction = null;
        }
        if (commandChangedListeners != null)
            commandChangedListeners.clear();
        if (selectionChangedListeners != null)
            selectionChangedListeners.clear();
        if (contentChangedListeners != null)
            contentChangedListeners.clear();
        if (dynamicChangedListeners != null)
            dynamicChangedListeners.clear();
        if (firstRegenListeners != null)
            firstRegenListeners.clear();

        super.delete();
    }

    protected void finalize() {
        Log.d(TAG, "BaseViewAdapter finalize");
    }

    public BaseViewAdapter(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            mSavedState = savedInstanceState.getBundle("vg");
            mSavedState = mSavedState != null ? mSavedState : savedInstanceState;
        }
        Log.d(TAG, "new BaseViewAdapter " + getCPtr(this) + " restore=" + (mSavedState != null));
    }

    public Bundle getSavedState() {
        return mSavedState;
    }

    public void removeContextButtons() {
        if (mAction != null) {
            mAction.removeButtonLayout();
        }
    }

    public void setContextActionEnabled(boolean enabled) {
        if (!enabled && isContextActionsVisible()) {
            hideContextActions();
        }
        mActionEnabled = enabled;
    }

    public void hideContextActions() {
        showContextActions(null, null, 0, 0, 0, 0);
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
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    removeCallbacks(this);
                    for (OnCommandChangedListener listener : commandChangedListeners) {
                        listener.onCommandChanged(getGraphView());
                    }
                }
            });
        }
    }

    @Override
    public void selectionChanged() {
        if (selectionChangedListeners != null) {
            postDelayed(new Runnable() {
                @Override
                public void run() {
                    removeCallbacks(this);
                    for (OnSelectionChangedListener listener : selectionChangedListeners) {
                        listener.onSelectionChanged(getGraphView());
                    }
                }
            }, 50);
        }
    }

    @Override
    public void contentChanged() {
        if (contentChangedListeners != null) {
            postDelayed(new Runnable() {
                @Override
                public void run() {
                    removeCallbacks(this);
                    for (OnContentChangedListener listener : contentChangedListeners) {
                        listener.onContentChanged(getGraphView());
                    }
                }
            }, 50);
        }
    }

    @Override
    public void dynamicChanged() {
        if (dynamicChangedListeners != null) {
            postDelayed(new Runnable() {
                @Override
                public void run() {
                    removeCallbacks(this);
                    for (OnDynamicChangedListener listener : dynamicChangedListeners) {
                        listener.onDynamicChanged(getGraphView());
                    }
                }
            }, 50);
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

    private Activity getActivity() {
        return (Activity) getGraphView().getView().getContext();
    }

    private void removeCallbacks(Runnable r) {
        getGraphView().getView().removeCallbacks(r);
    }

    private boolean postDelayed(Runnable action, long delayMillis) {
        return getGraphView().getView().postDelayed(action, delayMillis);
    }

    public void onFirstRegen() {
        if (++mRegenCount == 1) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "onFirstRegen restore=" + (mSavedState != null));
                    removeCallbacks(this);
                    if (firstRegenListeners != null) {
                        for (OnFirstRegenListener listener : firstRegenListeners) {
                            listener.onFirstRegen(getGraphView());
                        }
                        firstRegenListeners.clear();
                        firstRegenListeners = null;
                    }
                    if (mSavedState != null) {
                        restoreRecordState(mSavedState);
                        mSavedState = null;
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
            }
            if (!forUndo && mRecorder != null) {
                mRecorder.stop();
            }
        }
    }

    private boolean onStopped(ShapeRunnable r) {
        if (mUndoing == r)
            mUndoing = null;
        if (mRecorder == r)
            mRecorder = null;
        return coreView() != null;
    }

    private boolean isRecording(int type) {
        switch (type) {
            case START_UNDO: return mUndoing != null;
            default: return mRecorder != null;
        }
    }

    public static int getTick() {
        return ShapeRunnable.getTick();
    }

    public boolean startRecord(String path, int type) {
        if (isRecording(type))
            return false;

        synchronized (coreView()) {
            int doc = type < START_PLAY ? coreView().acquireFrontDoc() : 0;
            if (type < START_PLAY && doc == 0) {
                Log.e(TAG, "Fail to record shapes due to no front doc");
                return false;
            }
            if (type == START_RECORD) {
                coreView().traverseImageShapes(doc, new ImageFinder(
                        getGraphView().getImageCache().getImagePath(), path));
            }
            if (!coreView().startRecord(path, doc, type == START_UNDO, getTick()))
                return false;
        }

        if (type == START_UNDO) {
            mUndoing = new UndoRunnable(this, path);
            new Thread(mUndoing, "touchvg.undo").start();
        } else if (type == START_RECORD) {
            mRecorder = new RecordRunnable(this, path);
            new Thread(mRecorder, "touchvg.record").start();
        }

        return true;
    }

    private static class ImageFinder extends MgFindImageCallback {
        private String fromPath, toPath;
        public int count = 0;

        public ImageFinder(String fromPath, String toPath) {
            this.fromPath = fromPath;
            this.toPath = toPath;
        }

        @Override
        public void onFindImage(int sid, String name) {
            if (ImageCache.copyFileTo(fromPath, name, toPath)) {
                Log.d(TAG, ++count + " image files copied: " + name);
            }
        }
    }

    public boolean startPlay(String path) {
        hideContextActions();
        if (startRecord(path, START_PLAY)) {
        }
        return false;
    }

    public String getRecordPath() {
        return mRecorder != null ? mRecorder.getPath() : null;
    }

    public void undo() {
        if (mUndoing != null) {
            hideContextActions();
            mUndoing.requestRecord(UndoRunnable.UNDO);
        }
    }

    public void redo() {
        if (mUndoing != null) {
            hideContextActions();
            mUndoing.requestRecord(UndoRunnable.REDO);
        }
    }

    public static class StringCallback extends MgStringCallback {
        private String text;

        @Override
        public void onGetString(String text) {
            this.text = text;
        }

        @Override
        public String toString() {
            delete();
            return text;
        }
    }

    public String getCommand() {
        final StringCallback c = new StringCallback();
        coreView().getCommand(c);
        return c.toString();
    }

    public void onSaveInstanceState(Bundle outState) {
        final GiCoreView cv = coreView();

        outState.putString("cmdName", getCommand());
        outState.putString("imagePath", getGraphView().getImageCache().getImagePath());

        if (mUndoing != null) {
            outState.putString("undoPath", mUndoing.getPath());
            outState.putInt("undoIndex", cv.getRedoIndex());
            outState.putInt("undoCount", cv.getRedoCount());
            outState.putInt("undoTick", cv.getRecordTick(true, getTick()));
            outState.putInt("changeCount", cv.getChangeCount());
        }
        if (mRecorder != null) {
            outState.putString("recordPath", mRecorder.getPath());
            outState.putInt("frameIndex", cv.getFrameIndex());
            outState.putInt("recordTick", cv.getRecordTick(false, getTick()));
        }
    }

    public void onRestoreInstanceState(Bundle savedState) {
        coreView().setCommand(savedState.getString("cmdName"));
        getGraphView().getImageCache().setImagePath(savedState.getString("imagePath"));
    }

    private void restoreRecordState(Bundle savedState) {
        final GiCoreView cv = coreView();
        final String undoPath = savedState.getString("undoPath");
        final String recordPath = savedState.getString("recordPath");

        if (undoPath != null && mUndoing == null) {
            int index = savedState.getInt("undoIndex");
            int count = savedState.getInt("undoCount");
            int tick = savedState.getInt("undoTick");
            int changeCount = savedState.getInt("changeCount");
            boolean ret;

            synchronized (cv) {
                int doc = coreView().acquireFrontDoc();
                ret = doc != 0
                        && cv.restoreRecord(0, undoPath, doc, changeCount, index,
                                count, tick, getTick());
            }
            if (ret) {
                mUndoing = new UndoRunnable(this, undoPath);
                new Thread(mUndoing, "touchvg.undo").start();
            }
        }

        if (recordPath != null && mRecorder == null) {
            int index = savedState.getInt("frameIndex");
            int tick = savedState.getInt("recordTick");
            boolean playing = savedState.getBoolean("playing");
            boolean ret;

            synchronized (cv) {
                final LogHelper log = new LogHelper("" + coreView().backDoc());
                int doc = playing ? 0 : coreView().acquireFrontDoc();
                ret = (playing || doc != 0)
                        && cv.restoreRecord(playing ? 2 : 1, recordPath, doc, 0, index, 0,
                                tick, getTick());
                if (ret) {
                    if (playing) {
                    } else {
                        mRecorder = new RecordRunnable(this, recordPath);
                        new Thread(mRecorder, "touchvg.record").start();
                    }
                }
                log.r(ret);
            }
        }
    }

    protected static class UndoRunnable extends ShapeRunnable {
        private BaseViewAdapter mViewAdapter;
        public static final int UNDO = 0xFFFFFF10;
        public static final int REDO = 0xFFFFFF20;

        public UndoRunnable(BaseViewAdapter viewAdapter, String path) {
            super(path, START_UNDO, viewAdapter.coreView());
            this.mViewAdapter = viewAdapter;
        }

        @Override
        protected boolean stopRecord() {
            synchronized (GiCoreView.class) {
                boolean ret = mViewAdapter.onStopped(this);
                if (ret) {
                    synchronized (mCoreView) {
                        mCoreView.stopRecord(mViewAdapter, true);
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
        protected void process(int tick, int doc, int shapes) {
            if (tick == UNDO) {
                synchronized (mCoreView) {
                    mCoreView.undo(mViewAdapter);
                }
            } else if (tick == REDO) {
                synchronized (mCoreView) {
                    mCoreView.redo(mViewAdapter);
                }
            } else if (!mCoreView.recordShapes(true, tick, doc, shapes)) {
                Log.e(TAG, "Fail to record shapes for undoing, tick=" + tick + ", doc=" + doc);
            }
        }
    }

    protected static class RecordRunnable extends ShapeRunnable {
        protected BaseViewAdapter mViewAdapter;

        public RecordRunnable(BaseViewAdapter viewAdapter, String path) {
            super(path, START_RECORD, viewAdapter.coreView());
            this.mViewAdapter = viewAdapter;
        }

        @Override
        protected boolean stopRecord() {
            synchronized (GiCoreView.class) {
                boolean ret = mViewAdapter.onStopped(this);
                if (ret) {
                    synchronized (mCoreView) {
                        mCoreView.stopRecord(mViewAdapter, false);
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
        protected void process(int tick, int doc, int shapes) {
            if (!mCoreView.recordShapes(false, tick, doc, shapes)) {
                Log.e(TAG, "Fail to record shapes for playing, tick=" + tick
                        + ", doc=" + doc + ", shapes=" + shapes);
            }
        }
    }
}
