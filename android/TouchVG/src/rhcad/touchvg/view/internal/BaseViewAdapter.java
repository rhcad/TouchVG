package rhcad.touchvg.view.internal;

import java.util.ArrayList;

import rhcad.touchvg.IGraphView.OnCommandChangedListener;
import rhcad.touchvg.IGraphView.OnContentChangedListener;
import rhcad.touchvg.IGraphView.OnContextActionListener;
import rhcad.touchvg.IGraphView.OnDynamicChangedListener;
import rhcad.touchvg.IGraphView.OnFirstRegenListener;
import rhcad.touchvg.IGraphView.OnSelectionChangedListener;
import rhcad.touchvg.IGraphView.OnShapeClickedListener;
import rhcad.touchvg.IGraphView.OnShapeDeletedListener;
import rhcad.touchvg.IGraphView.OnShapesRecordedListener;
import rhcad.touchvg.core.CmdObserverDefault;
import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.core.MgFindImageCallback;
import rhcad.touchvg.core.MgMotion;
import rhcad.touchvg.core.MgStringCallback;
import rhcad.touchvg.core.MgView;
import rhcad.touchvg.view.BaseGraphView;
import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

//! 视图回调适配器
public abstract class BaseViewAdapter extends GiView {
    protected static final String TAG = "touchvg";
    private ContextAction mAction;
    private boolean mActionEnabled = true;
    private ArrayList<OnCommandChangedListener> commandChangedListeners;
    private ArrayList<OnSelectionChangedListener> selectionChangedListeners;
    private ArrayList<OnContentChangedListener> contentChangedListeners;
    private ArrayList<OnDynamicChangedListener> dynamicChangedListeners;
    private ArrayList<OnFirstRegenListener> firstRegenListeners;
    private ArrayList<OnShapesRecordedListener> shapesRecordedListeners;
    private ArrayList<OnShapeDeletedListener> shapeDeletedListeners;
    private ArrayList<OnShapeClickedListener> shapeClickedListeners;
    private ArrayList<OnContextActionListener> contextActionListeners;
    private ArrayList<OnPlayingListener> playingListeners;
    protected UndoRunnable mUndoing;
    protected RecordRunnable mRecorder;
    private int mRegenCount = 0;
    private Bundle mSavedState;
    private CmdObserverDelegate mCmdObserver;

    public abstract BaseGraphView getGraphView();

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
        if (shapesRecordedListeners != null)
            shapesRecordedListeners.clear();
        if (shapeDeletedListeners != null)
            shapeDeletedListeners.clear();
        if (shapeClickedListeners != null)
            shapeClickedListeners.clear();
        if (contextActionListeners != null)
            contextActionListeners.clear();
        if (playingListeners != null)
            playingListeners.clear();

        super.delete();
    }

    @Override
    protected void finalize() {
        Log.d(TAG, "BaseViewAdapter finalize");
        super.finalize();
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

    public void setContextActionEnabled(boolean enabled) {
        if (!enabled && isContextActionsVisible()) {
            hideContextActions();
        }
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
    public void hideContextActions() {
        if (mAction != null) {
            mAction.removeButtonLayout();
        }
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

    @Override
    public boolean shapeClicked(int sid, int tag, float x, float y) {
        if (shapeClickedListeners != null) {
            for (OnShapeClickedListener listener : shapeClickedListeners) {
                if (listener.onShapeClicked(getGraphView(), sid, tag, x, y))
                    return true;
            }
        }
        return false;
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

    public void setOnShapesRecordedListener(OnShapesRecordedListener listener) {
        if (this.shapesRecordedListeners == null)
            this.shapesRecordedListeners = new ArrayList<OnShapesRecordedListener>();
        this.shapesRecordedListeners.add(listener);
    }

    public void setOnShapeDeletedListener(OnShapeDeletedListener listener) {
        if (this.shapeDeletedListeners == null)
            this.shapeDeletedListeners = new ArrayList<OnShapeDeletedListener>();
        this.shapeDeletedListeners.add(listener);
    }

    public void setOnShapeClickedListener(OnShapeClickedListener listener) {
        if (this.shapeClickedListeners == null)
            this.shapeClickedListeners = new ArrayList<OnShapeClickedListener>();
        this.shapeClickedListeners.add(listener);
    }

    public void setOnContextActionListener(OnContextActionListener listener) {
        if (this.contextActionListeners == null)
            this.contextActionListeners = new ArrayList<OnContextActionListener>();
        this.contextActionListeners.add(listener);

        if (mCmdObserver == null) {
            mCmdObserver = new CmdObserverDelegate();
            cmdView().getCmdSubject().registerObserver(mCmdObserver);
        }
    }

    private class CmdObserverDelegate extends CmdObserverDefault {
        @Override
        protected void finalize() {
            Log.d(TAG, "CmdObserverDelegate finalize");
            super.finalize();
        }

        @Override
        public boolean doAction(MgMotion sender, int action) {
            for (OnContextActionListener listener : contextActionListeners) {
                if (listener.onContextAction(getGraphView(), sender, action))
                    return true;
            }
            return false;
        }
    }

    public Activity getActivity() {
        return (Activity) getGraphView().getView().getContext();
    }

    public void removeCallbacks(Runnable r) {
        getGraphView().getView().removeCallbacks(r);
    }

    public boolean postDelayed(Runnable action, long delayMillis) {
        return getGraphView().getView().postDelayed(action, delayMillis);
    }

    public void onFirstRegen() {
        if (++mRegenCount == 1) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "onFirstRegen restore=" + (mSavedState != null));
                    removeCallbacks(this);

                    if (coreView() == null)
                        return;
                    coreView().zoomToInitial();

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

    public GiCoreView coreView() {
        return getGraphView().coreView();
    }

    public MgView cmdView() {
        return MgView.fromHandle(coreView().viewAdapterHandle());
    }

    public void stop() {
        final LogHelper log = new LogHelper();
        stopRecord(true);
        stopRecord(false);
        if (playingListeners != null) {
            Log.d(TAG, playingListeners.size() + " playing listeners stopped");
            for (OnPlayingListener listener : playingListeners) {
                listener.onStopped();
            }
            playingListeners.clear();
        }
        if (mCmdObserver != null) {
            cmdView().getCmdSubject().unregisterObserver(mCmdObserver);
            mCmdObserver.delete();
            mCmdObserver = null;
        }
        log.r();
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

    public boolean onStopped(ShapeRunnable r) {
        if (mUndoing == r)
            mUndoing = null;
        if (mRecorder == r)
            mRecorder = null;
        return coreView() != null;
    }

    public static int getTick() {
        return ShapeRunnable.getTick();
    }

    public boolean startUndoRecord(String path) {
        if (mUndoing != null || !startRecordCore(path, UndoRunnable.TYPE))
            return false;

        mUndoing = new UndoRunnable(this, path);
        new Thread(mUndoing, "touchvg.undo").start();

        return true;
    }

    public boolean startRecord(String path) {
        if (mRecorder != null || !startRecordCore(path, RecordRunnable.TYPE))
            return false;

        mRecorder = new RecordRunnable(this, path);
        new Thread(mRecorder, "touchvg.record").start();

        return true;
    }

    private boolean startRecordCore(String path, int type) {
        final GiCoreView cv = coreView();

        synchronized (cv) {
            int doc = cv.acquireFrontDoc();
            if (doc == 0) {
                cv.submitBackDoc(null);
                doc = cv.acquireFrontDoc();
                if (doc == 0) {
                    Log.e(TAG, "Fail to record shapes due to no front doc");
                    return false;
                }
            }
            if (type == RecordRunnable.TYPE) {
                cv.traverseImageShapes(doc,
                        new ImageFinder(getGraphView().getImageCache().getImagePath(), path));
            }
            if (!cv.startRecord(path, doc, type == UndoRunnable.TYPE, getTick(),
                    createRecordCallback(type == RecordRunnable.TYPE))) {
                return false;
            }
        }
        return true;
    }

    private class RecordShapesCallback extends MgStringCallback {
        @Override
        public void onGetString(String filename) {
            final GiCoreView cv = coreView();
            final Bundle info = new Bundle();

            info.putString("filename", filename);
            info.putInt("tick", cv.getFrameTick());
            info.putInt("index", cv.getFrameIndex());
            info.putInt("flags", cv.getFrameFlags());
            info.putInt("tick", cv.getFrameTick());

            for (OnShapesRecordedListener listener : shapesRecordedListeners) {
                listener.onShapesRecorded(getGraphView(), info);
            }
        }
    }

    public MgStringCallback createRecordCallback(boolean r) {
        return r && shapesRecordedListeners != null ? new RecordShapesCallback() : null;
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
                    if (!playing) {
                        mRecorder = new RecordRunnable(this, recordPath);
                        new Thread(mRecorder, "touchvg.record").start();
                    } else if (playingListeners != null) {
                        for (OnPlayingListener listener : playingListeners) {
                            listener.onRestorePlayingState(savedState);
                        }
                    }
                }
                log.r(ret);
            }
        }
    }

    @Override
    public void shapeDeleted(int sid) {
        if (shapeDeletedListeners != null) {
            for (OnShapeDeletedListener listener : shapeDeletedListeners) {
                listener.onShapeDeleted(getGraphView(), sid);
            }
        }
        if (playingListeners != null) {
            for (OnPlayingListener listener : playingListeners) {
                listener.onShapeDeleted(sid);
            }
        }
    }

    public static interface OnPlayingListener {
        public void onRestorePlayingState(Bundle savedState);
        public void onShapeDeleted(int sid);
        public void onStopped();
    }

    public void setOnPlayingListener(OnPlayingListener listener) {
        if (this.playingListeners == null)
            this.playingListeners = new ArrayList<OnPlayingListener>();
        this.playingListeners.add(listener);
    }
}
