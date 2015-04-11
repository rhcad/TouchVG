// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import java.util.ArrayList;
import java.util.List;

import rhcad.touchvg.IGraphView.OnCommandChangedListener;
import rhcad.touchvg.IGraphView.OnContentChangedListener;
import rhcad.touchvg.IGraphView.OnContextActionListener;
import rhcad.touchvg.IGraphView.OnDrawGestureListener;
import rhcad.touchvg.IGraphView.OnDynDrawEndedListener;
import rhcad.touchvg.IGraphView.OnDynamicChangedListener;
import rhcad.touchvg.IGraphView.OnFirstRegenListener;
import rhcad.touchvg.IGraphView.OnSelectionChangedListener;
import rhcad.touchvg.IGraphView.OnShapeClickedListener;
import rhcad.touchvg.IGraphView.OnShapeDblClickedListener;
import rhcad.touchvg.IGraphView.OnShapeDeletedListener;
import rhcad.touchvg.IGraphView.OnShapeWillDeleteListener;
import rhcad.touchvg.IGraphView.OnShapesRecordedListener;
import rhcad.touchvg.IGraphView.OnViewDetachedListener;
import rhcad.touchvg.IGraphView.OnZoomChangedListener;
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
import android.content.Context;
import android.os.Bundle;
import android.util.Log;
import android.widget.Toast;

//! 视图回调适配器
public abstract class BaseViewAdapter extends GiView implements OnDrawGestureListener {
    protected static final String TAG = "touchvg";
    private static final String UNDO_THREAD = "touchvg.undo";
    private static final String RECORD_THREAD = "touchvg.record";
    private static final String KEY_CMDNAME = "cmdName";
    private static final String KEY_IMAGEPATH = "imagePath";
    private static final String KEY_UNDOPATH = "undoPath";
    private static final String KEY_UNDOINDEX = "undoIndex";
    private static final String KEY_UNDOCOUNT = "undoCount";
    private static final String KEY_UNDOTICK = "undoTick";
    private static final String KEY_CHANGECOUNT = "changeCount";
    private static final String KEY_RECORD_PATH = "recordPath";
    private static final String KEY_FRAME_INDEX = "frameIndex";
    private static final String KEY_RECORD_TICK = "recordTick";

    private ContextAction mAction;
    private boolean mActionEnabled = true;
    private List<OnCommandChangedListener> commandChangedListeners;
    private List<OnSelectionChangedListener> selectionChangedListeners;
    private List<OnContentChangedListener> contentChangedListeners;
    private List<OnDynamicChangedListener> dynamicChangedListeners;
    private List<OnZoomChangedListener> zoomChangedListeners;
    private List<OnFirstRegenListener> firstRegenListeners;
    private List<OnDynDrawEndedListener> dynDrawEndedListeners;
    private List<OnShapesRecordedListener> shapesRecordedListeners;
    private List<OnShapeWillDeleteListener> shapeWillDeleteListeners;
    private List<OnShapeDeletedListener> shapeDeletedListeners;
    private List<OnShapeClickedListener> shapeClickedListeners;
    private List<OnShapeDblClickedListener> shapeDblClickedListeners;
    private List<OnContextActionListener> contextActionListeners;
    private List<OnDrawGestureListener> gestureListeners;
    private List<OnPlayingListener> playingListeners;
    protected UndoRunnable mUndoing;
    protected RecordRunnable mRecorder;
    private int mRegenCount = 0;
    private Bundle mSavedState;
    private CmdObserverDelegate mCmdObserver;
    private OnViewDetachedListener mDetachedListener;

    public BaseViewAdapter(Bundle savedInstanceState) {
        if (savedInstanceState != null) {
            mSavedState = savedInstanceState.getBundle("vg");
            mSavedState = mSavedState != null ? mSavedState : savedInstanceState;
        }
        Log.d(TAG, "new BaseViewAdapter " + getCPtr(this) + " restore=" + (mSavedState != null));
    }

    public abstract BaseGraphView getGraphView();
    public abstract GestureListener getGestureListener();
    public abstract ImageCache getImageCache();
    protected abstract ContextAction createContextAction();

    public final Context getContext() {
        return getGraphView().getView().getContext();
    }

    public static BaseViewAdapter getMainAdapter(BaseGraphView view) {
        if (view == null || view.getMainView() == null) {
            return null;
        }
        return (BaseViewAdapter)((BaseGraphView)view.getMainView()).viewAdapter();
    }

    public synchronized void delete() {
        Log.d(TAG, "delete BaseViewAdapter " + getCPtr(this));
        if (mAction != null) {
            mAction.release();
            mAction = null;
        }
        if (mDetachedListener != null) {
            mDetachedListener.onGraphViewDetached();
            mDetachedListener = null;
        }

        final List<?>[] listeners = new List<?>[] { commandChangedListeners,
                selectionChangedListeners, contentChangedListeners, dynamicChangedListeners,
                firstRegenListeners, shapesRecordedListeners, shapeDeletedListeners,
                shapeClickedListeners, shapeDblClickedListeners, contextActionListeners,
                gestureListeners, playingListeners, shapeWillDeleteListeners,
                dynDrawEndedListeners, zoomChangedListeners };

        for (final List<?> listener : listeners) {
            if (listener != null) {
                listener.clear();
            }
        }
        super.delete();
    }

    public Bundle getSavedState() {
        return mSavedState;
    }

    public boolean getContextActionEnabled() {
        return mActionEnabled;
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
        return mAction.showActions(getContext(), actions, xy);
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
    public void zoomChanged() {
        if (zoomChangedListeners != null) {
            postDelayed(new Runnable() {
                @Override
                public void run() {
                    removeCallbacks(this);
                    for (OnZoomChangedListener listener : zoomChangedListeners) {
                        listener.onZoomChanged(getGraphView());
                    }
                }
            }, 50);
        }
    }

    @Override
    public boolean shapeClicked(int type, int sid, int tag, float x, float y) {
        if (shapeClickedListeners != null) {
            for (OnShapeClickedListener listener : shapeClickedListeners) {
                if (listener.onShapeClicked(getGraphView(), type, sid, tag, x, y)) {
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public boolean shapeDblClick(int type, int sid, int tag) {
        if (shapeDblClickedListeners != null) {
            for (OnShapeDblClickedListener listener : shapeDblClickedListeners) {
                if (listener.onShapeDblClicked(getGraphView(), type, sid, tag)) {
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public void showMessage(String text) {
        if (coreView() != null && text.startsWith("@")) {
            final String localstr = ResourceUtil.getStringFromName(getContext(), text.substring(1));
            Toast.makeText(getContext(), localstr, Toast.LENGTH_SHORT).show();
        } else {
            Toast.makeText(getContext(), text, Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    public void getLocalizedString(String name, MgStringCallback result) {
        result.onGetString(ResourceUtil.getStringFromName(getContext(), name));
    }

    public void setOnCommandChangedListener(OnCommandChangedListener listener) {
        if (this.commandChangedListeners == null) {
            this.commandChangedListeners = new ArrayList<OnCommandChangedListener>();
        }
        this.commandChangedListeners.add(listener);
    }

    public void setOnSelectionChangedListener(OnSelectionChangedListener listener) {
        if (this.selectionChangedListeners == null) {
            this.selectionChangedListeners = new ArrayList<OnSelectionChangedListener>();
        }
        this.selectionChangedListeners.add(listener);
    }

    public void setOnContentChangedListener(OnContentChangedListener listener) {
        if (this.contentChangedListeners == null) {
            this.contentChangedListeners = new ArrayList<OnContentChangedListener>();
        }
        this.contentChangedListeners.add(listener);
    }

    public void setOnDynamicChangedListener(OnDynamicChangedListener listener) {
        if (this.dynamicChangedListeners == null) {
            this.dynamicChangedListeners = new ArrayList<OnDynamicChangedListener>();
        }
        this.dynamicChangedListeners.add(listener);
    }

    public void setOnZoomChangedListener(OnZoomChangedListener listener) {
        if (this.zoomChangedListeners == null) {
            this.zoomChangedListeners = new ArrayList<OnZoomChangedListener>();
        }
        this.zoomChangedListeners.add(listener);
    }

    public void setOnFirstRegenListener(OnFirstRegenListener listener) {
        if (this.firstRegenListeners == null) {
            this.firstRegenListeners = new ArrayList<OnFirstRegenListener>();
        }
        this.firstRegenListeners.add(listener);
    }

    public void setOnDynDrawEndedListener(OnDynDrawEndedListener listener) {
        if (this.dynDrawEndedListeners == null) {
            this.dynDrawEndedListeners = new ArrayList<OnDynDrawEndedListener>();
        }
        this.dynDrawEndedListeners.add(listener);
    }

    public void setOnShapesRecordedListener(OnShapesRecordedListener listener) {
        if (this.shapesRecordedListeners == null) {
            this.shapesRecordedListeners = new ArrayList<OnShapesRecordedListener>();
        }
        this.shapesRecordedListeners.add(listener);
    }

    public void setOnShapeWillDeleteListener(OnShapeWillDeleteListener listener) {
        if (this.shapeWillDeleteListeners == null) {
            this.shapeWillDeleteListeners = new ArrayList<OnShapeWillDeleteListener>();
        }
        this.shapeWillDeleteListeners.add(listener);
    }

    public void setOnShapeDeletedListener(OnShapeDeletedListener listener) {
        if (this.shapeDeletedListeners == null) {
            this.shapeDeletedListeners = new ArrayList<OnShapeDeletedListener>();
        }
        this.shapeDeletedListeners.add(listener);
    }

    public void setOnShapeClickedListener(OnShapeClickedListener listener) {
        if (this.shapeClickedListeners == null) {
            this.shapeClickedListeners = new ArrayList<OnShapeClickedListener>();
        }
        this.shapeClickedListeners.add(listener);
    }

    public void setOnShapeDblClickedListener(OnShapeDblClickedListener listener) {
        if (this.shapeDblClickedListeners == null) {
            this.shapeDblClickedListeners = new ArrayList<OnShapeDblClickedListener>();
        }
        this.shapeDblClickedListeners.add(listener);
    }

    public void setOnContextActionListener(OnContextActionListener listener) {
        if (this.contextActionListeners == null) {
            this.contextActionListeners = new ArrayList<OnContextActionListener>();
        }
        this.contextActionListeners.add(listener);

        if (mCmdObserver == null) {
            mCmdObserver = new CmdObserverDelegate();
            cmdView().getCmdSubject().registerObserver(mCmdObserver);
        }
    }

    public void setOnGestureListener(OnDrawGestureListener listener) {
        if (this.gestureListeners == null) {
            this.gestureListeners = new ArrayList<OnDrawGestureListener>();
        }
        this.gestureListeners.add(listener);
    }

    @Override
    public boolean onPreGesture(int gestureType, float x, float y) {
        if (gestureListeners != null) {
            for (OnDrawGestureListener listener : gestureListeners) {
                if (listener.onPreGesture(gestureType, x, y)) {
                    return true;
                }
            }
        }
        return false;
    }

    @Override
    public void onPostGesture(int gestureType, float x, float y) {
        if (gestureListeners != null) {
            for (OnDrawGestureListener listener : gestureListeners) {
                listener.onPostGesture(gestureType, x, y);
            }
        }
    }

    private class CmdObserverDelegate extends CmdObserverDefault {
        @Override
        public boolean doAction(MgMotion sender, int action) {
            for (OnContextActionListener listener : contextActionListeners) {
                if (listener.onContextAction(getGraphView(), sender, action)) {
                    return true;
                }
            }
            return false;
        }
    }

    public Activity getActivity() {
        return (Activity) getContext();
    }

    public void removeCallbacks(Runnable r) {
        getGraphView().getView().removeCallbacks(r);
    }

    public boolean postDelayed(Runnable action, long delayMillis) {
        return getGraphView().getView().postDelayed(action, delayMillis);
    }

    public void fireDynDrawEnded() {
        if (dynDrawEndedListeners != null) {
            for (OnDynDrawEndedListener listener : dynDrawEndedListeners) {
                listener.onDynDrawEnded(getGraphView());
            }
        }
    }

    public void onFirstRegen() {
        if (++mRegenCount == 1) {
            getActivity().runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Log.d(TAG, "onFirstRegen restore=" + (mSavedState != null));
                    removeCallbacks(this);

                    if (coreView() == null) {
                        return;
                    }
                    coreView().zoomToInitial();

                    fireFirstRegen();
                    if (mSavedState != null) {
                        restoreRecordState(mSavedState);
                        mSavedState = null;
                    }
                }
            });
        }
    }

    private void fireFirstRegen() {
        if (firstRegenListeners != null) {
            for (OnFirstRegenListener listener : firstRegenListeners) {
                listener.onFirstRegen(getGraphView());
            }
            firstRegenListeners.clear();
            firstRegenListeners = null;
        }
    }

    protected void recordForRegenAll(boolean changed, int changeCount) {
        final GiCoreView cv = coreView();

        if (changed || getRegenCount() == 0) {
            cv.submitBackDoc(this, changed);
            cv.submitDynamicShapes(this);

            if (mUndoing != null && changed) {
                int doc0 = cv.acquireFrontDoc();
                mUndoing.requestRecord(cv.getRecordTick(true, getTick()), changeCount, doc0, 0);
            }
            if (mRecorder != null && changed) {
                int doc1 = cv.acquireFrontDoc();
                int s = cv.acquireDynamicShapes();
                mRecorder.requestRecord(cv.getRecordTick(false, getTick()), changeCount, doc1, s);
            }
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

    public void stop(OnViewDetachedListener detachedListener) {
        final LogHelper log = new LogHelper();
        stopRecord(true);
        stopRecord(false);
        if (detachedListener != null) {
            mDetachedListener = detachedListener;
        }
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
        if (mUndoing == r) {
            mUndoing = null;
        }
        if (mRecorder == r) {
            mRecorder = null;
        }
        return coreView() != null;
    }

    public static int getTick() {
        return ShapeRunnable.getTick();
    }

    public boolean startUndoRecord(String path) {
        if (mUndoing != null || !startRecordCore(path, UndoRunnable.TYPE)) {
            return false;
        }

        mUndoing = new UndoRunnable(this, path);
        new Thread(mUndoing, UNDO_THREAD).start();

        return true;
    }

    public boolean startRecord(String path) {
        if (mRecorder != null || !startRecordCore(path, RecordRunnable.TYPE)) {
            return false;
        }

        mRecorder = new RecordRunnable(this, path);
        new Thread(mRecorder, RECORD_THREAD).start();

        return true;
    }

    private boolean startRecordCore(String path, int type) {
        final GiCoreView cv = coreView();

        synchronized (cv) {
            int doc = cv.acquireFrontDoc();
            if (doc == 0) {
                cv.submitBackDoc(null, false);
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
        private int count = 0;

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

        outState.putString(KEY_CMDNAME, getCommand());
        outState.putString(KEY_IMAGEPATH, getGraphView().getImageCache().getImagePath());

        if (mUndoing != null) {
            outState.putString(KEY_UNDOPATH, mUndoing.getPath());
            outState.putInt(KEY_UNDOINDEX, cv.getRedoIndex());
            outState.putInt(KEY_UNDOCOUNT, cv.getRedoCount());
            outState.putInt(KEY_UNDOTICK, cv.getRecordTick(true, getTick()));
            outState.putInt(KEY_CHANGECOUNT, cv.getChangeCount());
        }
        if (mRecorder != null) {
            outState.putString(KEY_RECORD_PATH, mRecorder.getPath());
            outState.putInt(KEY_FRAME_INDEX, cv.getFrameIndex());
            outState.putInt(KEY_RECORD_TICK, cv.getRecordTick(false, getTick()));
        }
    }

    public void onRestoreInstanceState(Bundle savedState) {
        coreView().setCommand(savedState.getString(KEY_CMDNAME));
        getGraphView().getImageCache().setImagePath(savedState.getString(KEY_IMAGEPATH));
    }

    private void restoreRecordState(Bundle savedState) {
        final GiCoreView cv = coreView();
        final String undoPath = savedState.getString(KEY_UNDOPATH);

        if (undoPath != null && mUndoing == null) {
            int index = savedState.getInt(KEY_UNDOINDEX);
            int count = savedState.getInt(KEY_UNDOCOUNT);
            int tick = savedState.getInt(KEY_UNDOTICK);
            int changeCount = savedState.getInt(KEY_CHANGECOUNT);
            boolean ret;

            synchronized (cv) {
                int doc = coreView().acquireFrontDoc();
                ret = doc != 0
                        && cv.restoreRecord(0, undoPath, doc, changeCount, index,
                                count, tick, getTick());
            }
            if (ret) {
                mUndoing = new UndoRunnable(this, undoPath);
                new Thread(mUndoing, UNDO_THREAD).start();
            }
        }

        final String path = savedState.getString(KEY_RECORD_PATH);
        if (path != null && mRecorder == null) {
            synchronized (cv) {
                restorePlayState(savedState, cv, path);
            }
        }
    }

    private void restorePlayState(Bundle savedState, GiCoreView cv, String path) {
        final LogHelper log = new LogHelper("" + coreView().backDoc());
        boolean playing = savedState.getBoolean("playing");
        boolean ret = restoreRecord(savedState, cv, path, playing);

        if (ret && !playing) {
            mRecorder = new RecordRunnable(this, path);
            new Thread(mRecorder, RECORD_THREAD).start();
        }
        if (ret && playing && playingListeners != null) {
            for (OnPlayingListener listener : playingListeners) {
                listener.onRestorePlayingState(savedState);
            }
        }
        log.r(ret);
    }

    private boolean restoreRecord(Bundle savedState, GiCoreView cv, String path, boolean playing) {
        int index = savedState.getInt(KEY_FRAME_INDEX);
        int tick = savedState.getInt(KEY_RECORD_TICK);
        int doc = playing ? 0 : coreView().acquireFrontDoc();
        return (playing || doc != 0)
                && cv.restoreRecord(playing ? 2 : 1, path, doc, 0, index, 0, tick, getTick());
    }

    @Override
    public void shapeWillDelete(int sid) {
        if (shapeWillDeleteListeners != null) {
            for (OnShapeWillDeleteListener listener : shapeWillDeleteListeners) {
                listener.onShapeWillDelete(getGraphView(), sid);
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
        if (this.playingListeners == null) {
            this.playingListeners = new ArrayList<OnPlayingListener>();
        }
        this.playingListeners.add(listener);
    }
}
