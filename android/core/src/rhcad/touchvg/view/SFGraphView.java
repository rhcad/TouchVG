// Copyright (c) 2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.internal.BaseViewAdapter;
import rhcad.touchvg.view.internal.ContextAction;
import rhcad.touchvg.view.internal.GestureListener;
import rhcad.touchvg.view.internal.ImageCache;
import rhcad.touchvg.view.internal.LogHelper;
import rhcad.touchvg.view.internal.ResourceUtil;
import rhcad.touchvg.view.internal.ViewUtil;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

/**
 * \ingroup GROUP_ANDROID
 * Graphics view with media overlay surface placed behind its window.
 * It uses a surface view placed on top of its window to draw dynamic shapes.
 */
public class SFGraphView extends SurfaceView implements BaseGraphView, GestureNotify {
    protected static final String TAG = "touchvg";
    protected GiCoreView mCoreView;
    protected SFViewAdapter mViewAdapter;
    protected ImageCache mImageCache;
    private IGraphView mMainView;

    protected RenderRunnable mRender;
    protected SurfaceView mDynDrawView;
    protected DynRenderRunnable mDynDrawRender;
    protected CanvasAdapter mCanvasAdapter;
    protected CanvasAdapter mDynDrawCanvas;
    private CanvasAdapter mCanvasOnDraw;
    private SurfaceCallback mSurfaceCallback;
    private DynSurfaceCallback mDynSurfaceCallback;

    protected int mBkColor = Color.WHITE;
    protected Drawable mBackground;
    protected GestureDetector mGestureDetector;
    protected GestureListener mGestureListener;
    protected boolean mGestureEnable = true;

    static {
        System.loadLibrary("touchvg");
    }

    protected void finalize() {
        Log.d(TAG, "SFGraphView finalize");
    }

    //! 构造绘图视图
    public SFGraphView(Context context) {
        this(context, (Bundle)null);
    }

    //! 构造绘图视图，允许在Activity的onCreate(Bundle)或onRestoreInstanceState(Bundle)中调用
    public SFGraphView(Context context, Bundle savedInstanceState) {
        super(context);
        mImageCache = new ImageCache();
        createAdapter(context, savedInstanceState);
        mCoreView = GiCoreView.createView(mViewAdapter);
        initView(context);
        ViewUtil.onAddView(this);
    }

    public SFGraphView(Context context, BaseGraphView mainView) {
        super(context);
        mImageCache = mainView.getImageCache();
        createAdapter(context, null);
        mMainView = mainView;
        mCoreView = GiCoreView.createMagnifierView(mViewAdapter,
                mainView.coreView(), mainView.viewAdapter());
        initView(context);
    }

    protected void createAdapter(Context context, Bundle savedInstanceState) {
        mCanvasAdapter = new CanvasAdapter(this, mImageCache);
        mDynDrawCanvas = new CanvasAdapter(this, mImageCache);
        mCanvasOnDraw = new CanvasAdapter(this, mImageCache);
        mViewAdapter = new SFViewAdapter(savedInstanceState);
    }

    protected void initView(Context context) {
        setWillNotDraw(false);          // Avoid black screen before surface created
        setWillNotCacheDrawing(true);   // avoid unnecessary usage of the memory.
        setZOrderMediaOverlay(true);    // see setBackgroundColor
        mSurfaceCallback = new SurfaceCallback(this);
        getHolder().addCallback(mSurfaceCallback);

        mGestureListener = new GestureListener(mCoreView, mViewAdapter, this);
        mGestureDetector = new GestureDetector(context, mGestureListener);
        ResourceUtil.setContextImages(context);

        final DisplayMetrics dm = context.getApplicationContext()
                .getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi, dm.density);

        this.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                    activateView();
                }
                return mGestureEnable && (mGestureListener.onTouch(v, event)
                                          || mGestureDetector.onTouchEvent(event));
            }
        });
    }

    protected void activateView() {
        mViewAdapter.removeContextButtons();
        ViewUtil.activateView(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        final LogHelper log = new LogHelper();

        ViewUtil.onRemoveView(this);
        if (mSurfaceCallback != null) {
            getHolder().removeCallback(mSurfaceCallback);
            mSurfaceCallback.release();
            mSurfaceCallback = null;
        }
        if (mDynSurfaceCallback != null) {
            mDynDrawView.getHolder().removeCallback(mDynSurfaceCallback);
            mDynSurfaceCallback.release();
            mDynSurfaceCallback = null;
        }
        mDynDrawView = null;
        mMainView = null;

        if (mCanvasAdapter != null) {
            mCanvasAdapter.delete();
            mCanvasAdapter = null;
        }
        if (mDynDrawCanvas != null) {
            mDynDrawCanvas.delete();
            mDynDrawCanvas = null;
        }
        if (mCanvasOnDraw != null) {
            mCanvasOnDraw.delete();
            mCanvasOnDraw = null;
        }
        if (mImageCache != null) {
            synchronized (mImageCache) {
            }
            mImageCache.clear();
            mImageCache = null;
        }
        if (mViewAdapter != null) {
            synchronized (GiCoreView.class) {
                final LogHelper log2 = new LogHelper("GiCoreView.class synchronized");
                mCoreView.destoryView(mViewAdapter);
                mViewAdapter.delete();
                mViewAdapter = null;
                mCoreView.release();
                mCoreView = null;
                log2.r();
            }
        }
        if (mGestureListener != null) {
            mGestureListener.release();
            mGestureListener = null;
        }
        mGestureDetector = null;
        log.r();

        super.onDetachedFromWindow();
    }

    @Override
    public void onDraw(Canvas canvas) {
        if (mBackground != null) {
            mBackground.draw(canvas);
        } else {
            canvas.drawColor(mBkColor, mBkColor == Color.TRANSPARENT ? Mode.CLEAR : Mode.SRC_OVER);
        }

        if (mCanvasOnDraw != null && mCanvasOnDraw.beginPaint(canvas)) {
            drawShapes(mCanvasOnDraw);
            mCanvasOnDraw.setPen(0x40FF0000, 2, 0, 0, 0);
            mCanvasOnDraw.drawLine(0, 0, getWidth(), getHeight());
            mCanvasOnDraw.drawLine(0, getHeight(), getWidth(), 0);
            mCanvasOnDraw.endPaint();
        }
    }

    private int drawShapes(CanvasAdapter canvasAdapter) {
        int doc, gs, n;

        synchronized (mCoreView) {
            doc = mCoreView.acquireFrontDoc();
            gs = mCoreView.acquireGraphics(mViewAdapter);
        }
        try {
            n = mCoreView.drawAll(doc, gs, canvasAdapter);
        } finally {
            GiCoreView.releaseDoc(doc);
            mCoreView.releaseGraphics(gs);
        }

        return n;
    }

    protected static class RenderRunnable implements Runnable {
        private SFGraphView mView;

        public RenderRunnable(SFGraphView view) {
            this.mView = view;
        }

        protected void finalize() {
            Log.d(TAG, "RenderRunnable finalize");
        }

        public void requestRender() {
            synchronized (this) {
                this.notify();
            }
        }

        public void stop() {
            final LogHelper log = new LogHelper();
            mView.mCoreView.stopDrawing();
            requestRender();
            synchronized (mView.mCanvasAdapter) {
                try {
                    mView.mCanvasAdapter.wait(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            mView = null;
            log.r();
        }

        @Override
        public void run() {
            mView.mCoreView.stopDrawing(false);
            while (!mView.mCoreView.isStopping()) {
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if (!mView.mCoreView.isStopping())
                    render();
            }
            synchronized (mView.mCanvasAdapter) {
                mView.mCanvasAdapter.notify();
            }
            Log.d(TAG, "RenderRunnable exit");
        }

        protected void render() {
            Canvas canvas = null;
            int oldcnt = (mView.mDynDrawRender != null) ? mView.mDynDrawRender.getAppendCount() : 0;
            final SurfaceHolder holder = mView.getHolder();
            int count = -1;

            try {
                canvas = holder.lockCanvas();
                if (mView.mCanvasAdapter.beginPaint(canvas)) {
                    if (mView.mBackground != null) {
                        mView.mBackground.draw(canvas);
                    } else {
                        canvas.drawColor(mView.mBkColor, mView.mBkColor == Color.TRANSPARENT
                                ? Mode.CLEAR : Mode.SRC_OVER);
                    }
                    count = mView.drawShapes(mView.mCanvasAdapter);
                    mView.mCanvasAdapter.endPaint();

                    if (mView.mDynDrawRender != null) {
                        mView.mDynDrawRender.afterRegen(oldcnt);
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (canvas != null) {
                    holder.unlockCanvasAndPost(canvas);

                    if (!mView.willNotDraw()) {
                        try {
                            Thread.sleep(100);
                        } catch (InterruptedException e) {
                        }
                        ((Activity) mView.getContext()).runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                mView.removeCallbacks(this);
                                mView.setWillNotDraw(true); // Not use onDraw now
                            }
                        });
                    }
                    if (count >= 0 && !mView.mCoreView.isStopping()) {
                        mView.mViewAdapter.onFirstRegen();
                    }
                }
            }
        }
    }

    protected static class SurfaceCallback implements SurfaceHolder.Callback {
        private SFGraphView mView;

        public SurfaceCallback(SFGraphView view) {
            this.mView = view;
        }

        public void release() {
            mView = null;
        }

        protected void finalize() {
            Log.d(TAG, "SurfaceCallback finalize");
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mView.mCoreView.onSize(mView.mViewAdapter, width, height);

            mView.postDelayed(new Runnable() {  // 延时执行，只执行一次
                @Override
                public void run() {
                    mView.removeCallbacks(this);
                    if (mView.mViewAdapter != null)
                        mView.mViewAdapter.regenAll(false);
                }
            }, 10);
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mView.mRender = new RenderRunnable(mView);
            new Thread(mView.mRender, "touchvg.render").start();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            if (mView.mRender != null) {
                mView.mRender.stop();
                mView.mRender = null;
            }
        }
    }

    protected static class DynRenderRunnable implements Runnable {
        private int[] mAppendShapeIDs = new int[] { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        private SurfaceHolder mHolder;
        private SFGraphView mView;

        protected void finalize() {
            Log.d(TAG, "DynRenderRunnable finalize");
        }

        public DynRenderRunnable(SFGraphView view, SurfaceHolder holder) {
            this.mView = view;
            this.mHolder = holder;
        }

        public void requestRender() {
            synchronized (this) {
                this.notify();
            }
        }

        public void stop() {
            final LogHelper log = new LogHelper();
            mView.mCoreView.stopDrawing();
            requestRender();
            synchronized (mView.mDynDrawCanvas) {
                try {
                    mView.mDynDrawCanvas.wait(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            log.r();
            mView = null;
            mHolder = null;
            mAppendShapeIDs = null;
        }

        public int getAppendCount() {
            int n = 0;
            for (int i = 0; i < mAppendShapeIDs.length; i++) {
                if (mAppendShapeIDs[i] != 0) {
                    n++;
                }
            }
            return n;
        }

        public void afterRegen(int count) {
            int maxCount = getAppendCount();
            count = Math.min(count, maxCount);
            for (int i = 0, j = count; i < mAppendShapeIDs.length; i++, j++) {
                mAppendShapeIDs[i] = j < mAppendShapeIDs.length ? mAppendShapeIDs[j] : 0;
            }
            requestRender();
        }

        public void requestAppendRender(int sid) {
            for (int i = 0; i < mAppendShapeIDs.length; i++) {
                if (mAppendShapeIDs[i] == sid)
                    break;
                if (mAppendShapeIDs[i] == 0) {
                    mAppendShapeIDs[i] = sid;
                    break;
                }
            }
            requestRender();
        }

        @Override
        public void run() {
            mView.mCoreView.stopDrawing(false);
            while (!mView.mCoreView.isStopping()) {
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                if (!mView.mCoreView.isStopping()) {
                    render();
                }
            }
            synchronized (mView.mDynDrawCanvas) {
                mView.mDynDrawCanvas.notify();
            }
            Log.d(TAG, "DynRenderRunnable exit");
        }

        protected void render() {
            Canvas canvas = null;
            try {
                canvas = mHolder.lockCanvas();
                if (mView.mDynDrawCanvas.beginPaint(canvas)) {
                    int doc, shapes, gs;

                    synchronized (mView.mCoreView) {
                        doc = mAppendShapeIDs[0] != 0 ? mView.mCoreView.acquireFrontDoc() : 0;
                        shapes = mView.mCoreView.acquireDynamicShapes();
                        gs = mView.mCoreView.acquireGraphics(mView.mViewAdapter);
                    }
                    try {
                        canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
                        for (int sid : mAppendShapeIDs) {
                            if (sid != 0) {
                                mView.mCoreView.drawAppend(doc, gs, mView.mDynDrawCanvas, sid);
                            }
                        }
                        mView.mCoreView.dynDraw(shapes, gs, mView.mDynDrawCanvas,
                                mView.mViewAdapter.acquirePlayings());
                    } finally {
                        GiCoreView.releaseDoc(doc);
                        GiCoreView.releaseShapes(shapes);
                        mView.mCoreView.releaseGraphics(gs);
                        mView.mDynDrawCanvas.endPaint();
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (canvas != null) {
                    mHolder.unlockCanvasAndPost(canvas);
                }
            }
        }
    }

    protected static class DynSurfaceCallback implements SurfaceHolder.Callback {
        private SFGraphView mView;

        public DynSurfaceCallback(SFGraphView view) {
            this.mView = view;
        }

        public void release() {
            mView = null;
        }

        protected void finalize() {
            Log.d(TAG, "DynSurfaceCallback finalize");
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mView.mDynDrawRender.requestRender();
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mView.mDynDrawRender = new DynRenderRunnable(mView, holder);
            new Thread(mView.mDynDrawRender, "touchvg.dynrender").start();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            if (mView.mDynDrawRender != null) {
                mView.mDynDrawRender.stop();
                mView.mDynDrawRender = null;
            }
        }
    }

    protected class SFViewAdapter extends BaseViewAdapter {
        protected void finalize() {
            Log.d(TAG, "SFViewAdapter finalize");
        }

        public SFViewAdapter(Bundle savedInstanceState) {
            super(savedInstanceState);
        }

        @Override
        protected BaseGraphView getGraphView() {
            return SFGraphView.this;
        }

        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, SFGraphView.this);
        }

        @Override
        public void regenAll(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                if (!mCoreView.isUndoLoading()) {
                    if (changed || mViewAdapter.getRegenCount() == 0)
                        mCoreView.submitBackDoc(mViewAdapter);
                    mCoreView.submitDynamicShapes(mViewAdapter);

                    if (mUndoing != null && changed) {
                        int tick0 = mCoreView.getRecordTick(true, getTick());
                        int doc0 = mCoreView.acquireFrontDoc();
                        mUndoing.requestRecord(tick0, doc0, 0);
                    }
                }
                if (mRecorder != null && changed) {
                    int tick1 = mCoreView.getRecordTick(false, getTick());
                    int doc1 = mCoreView.acquireFrontDoc();
                    int shapes1 = mCoreView.acquireDynamicShapes();
                    mRecorder.requestRecord(tick1, doc1, shapes1);
                }
            }
            if (mRender != null) {
                mRender.requestRender();
            }
        }

        @Override
        public void regenAppend(int sid) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                mCoreView.submitBackDoc(mViewAdapter);
                mCoreView.submitDynamicShapes(mViewAdapter);

                if (mUndoing != null) {
                    int tick0 = mCoreView.getRecordTick(true, getTick());
                    int doc0 = mCoreView.acquireFrontDoc();
                    mUndoing.requestRecord(tick0, doc0, 0);
                }
                if (mRecorder != null) {
                    int tick1 = mCoreView.getRecordTick(false, getTick());
                    int doc1 = mCoreView.acquireFrontDoc();
                    int shapes1 = mCoreView.acquireDynamicShapes();
                    mRecorder.requestRecord(tick1, doc1, shapes1);
                }
            }
            if (mDynDrawRender != null) {
                mDynDrawRender.requestAppendRender(sid);
            }
            if (mRender != null) {
                mRender.requestRender();
            }
        }

        @Override
        public void redraw(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                if (changed) {
                    mCoreView.submitDynamicShapes(mViewAdapter);
                }
                if (mRecorder != null) {
                    int tick1 = mCoreView.getRecordTick(false, getTick());
                    int shapes1 = mCoreView.acquireDynamicShapes();
                    mRecorder.requestRecord(tick1, 0, shapes1);
                }
            }
            if (mDynDrawRender != null) {
                mDynDrawRender.requestRender();
            }
        }
    }

    @Override
    public GiCoreView coreView() {
        return mCoreView;
    }

    @Override
    public GiView viewAdapter() {
        return mViewAdapter;
    }

    @Override
    public View getView() {
        return this;
    }

    @Override
    public IGraphView getMainView() {
        return mMainView != null ? mMainView : this;
    }

    private static class DynSurfaceView extends SurfaceView {
        protected void finalize() {
            Log.d(TAG, "SFGraphView finalize");
        }

        public DynSurfaceView(Context context) {
            super(context);
            setZOrderOnTop(true);
            setWillNotCacheDrawing(true);
            getHolder().setFormat(PixelFormat.TRANSPARENT);
        }
    }

    @Override
    public View createDynamicShapeView(Context context) {
        if (mDynDrawView == null) {
            mDynDrawView = new DynSurfaceView(context);
            mDynSurfaceCallback = new DynSurfaceCallback(this);
            mDynDrawView.getHolder().addCallback(mDynSurfaceCallback);
        }
        return mDynDrawView;
    }

    @Override
    public ImageCache getImageCache() {
        return mImageCache;
    }

    @Override
    public void clearCachedData() {
        mCoreView.clearCachedData();
    }

    @Override
    public void stop() {
        mViewAdapter.stop();
        if (mRender != null) {
            mRender.stop();
            mRender = null;
        }
        if (mDynDrawRender != null) {
            mDynDrawRender.stop();
            mDynDrawRender = null;
        }
    }

    @Override
    public boolean onPause() {
        final LogHelper log = new LogHelper();
        setWillNotDraw(false);
        mViewAdapter.hideContextActions();
        return log.r(mCoreView.onPause(BaseViewAdapter.getTick()));
    }

    @Override
    public boolean onResume() {
        final LogHelper log = new LogHelper();
        if (mRender != null) {
            setWillNotDraw(true);
        }
        return log.r(mCoreView.onResume(BaseViewAdapter.getTick()));
    }

    @Override
    public void setBackgroundColor(int color) {
        boolean transparent = (color == Color.TRANSPARENT);
        mBkColor = color;
        getHolder().setFormat(transparent ? PixelFormat.TRANSPARENT : PixelFormat.OPAQUE);
        if (!transparent) {
            mCoreView.setBkColor(mViewAdapter, color);
        }
        mViewAdapter.regenAll(false);
    }

    @Override
    public void setBackgroundDrawable(Drawable background) {
        this.mBackground = background;
    }

    @Override
    public void setContextActionEnabled(boolean enabled) {
        mViewAdapter.setContextActionEnabled(enabled);
    }

    @Override
    public void setGestureEnable(boolean enabled) {
        mGestureEnable = enabled;
        mGestureListener.setGestureEnable(enabled);
    }

    @Override
    public boolean onTouchDrag(int action, float x, float y) {
        return mGestureListener.onTouchDrag(this, action, x, y);
    }

    @Override
    public boolean onTap(float x, float y) {
        return mGestureListener.onTap(x, y);
    }

    @Override
    public Bitmap snapshot(boolean transparent) {
        final Bitmap bitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        final CanvasAdapter canvasAdapter = new CanvasAdapter(this, mImageCache);

        if (canvasAdapter.beginPaint(new Canvas(bitmap))) {
            if (transparent) {
                bitmap.eraseColor(Color.TRANSPARENT);
            } else {
                bitmap.eraseColor(mBkColor);
                if (mBackground != null) {
                    mBackground.draw(canvasAdapter.getCanvas());
                }
            }
            drawShapes(canvasAdapter);
            canvasAdapter.endPaint();
        }
        canvasAdapter.delete();

        return bitmap;
    }

    @Override
    public Bitmap snapshot(int doc, int gs, boolean transparent) {
        final Bitmap bitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        final CanvasAdapter canvasAdapter = new CanvasAdapter(this, mImageCache);

        if (canvasAdapter.beginPaint(new Canvas(bitmap))) {
            if (transparent) {
                bitmap.eraseColor(Color.TRANSPARENT);
            } else {
                bitmap.eraseColor(mBkColor);
                if (mBackground != null) {
                    mBackground.draw(canvasAdapter.getCanvas());
                }
            }
            mCoreView.drawAll(doc, gs, canvasAdapter);
            canvasAdapter.endPaint();
        }
        canvasAdapter.delete();

        return bitmap;
    }

    @Override
    public void setOnCommandChangedListener(OnCommandChangedListener listener) {
        mViewAdapter.setOnCommandChangedListener(listener);
    }

    @Override
    public void setOnSelectionChangedListener(OnSelectionChangedListener listener) {
        mViewAdapter.setOnSelectionChangedListener(listener);
    }

    @Override
    public void setOnContentChangedListener(OnContentChangedListener listener) {
        mViewAdapter.setOnContentChangedListener(listener);
    }

    @Override
    public void setOnDynamicChangedListener(OnDynamicChangedListener listener) {
        mViewAdapter.setOnDynamicChangedListener(listener);
    }

    @Override
    public void setOnFirstRegenListener(OnFirstRegenListener listener) {
        mViewAdapter.setOnFirstRegenListener(listener);
    }

    @Override
    public void setOnPlayEndedListener(OnPlayEndedListener listener) {
        mViewAdapter.setOnPlayEndedListener(listener);
    }

    @Override
    public boolean onPreLongPress(MotionEvent e) {
        return false;
    }

    @Override
    public boolean onPreSingleTap(MotionEvent e) {
        return false;
    }

    @Override
    public boolean onPreDoubleTap(MotionEvent e) {
        return false;
    }
}
