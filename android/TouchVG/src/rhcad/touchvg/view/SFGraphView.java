//! \file SFGraphView.java
//! \brief Graphics view with media overlay surface placed behind its window.
// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiGestureType;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.core.Longs;
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
 * Graphics view with media overlay surface placed behind its window.
 * It uses a surface view placed on top of its window to draw dynamic shapes.
 */
public class SFGraphView extends SurfaceView implements BaseGraphView {
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
        System.loadLibrary(TAG);
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
        mImageCache = mainView != null ? mainView.getImageCache() : new ImageCache();
        createAdapter(context, null);
        mMainView = mainView;
        if (mainView != null) {
            mCoreView = GiCoreView.createMagnifierView(mViewAdapter,
                    mainView.coreView(), mainView.viewAdapter());
        } else {
            mCoreView = GiCoreView.createView(mViewAdapter, GiCoreView.kNoCmdType);
        }
        initView(context);
    }

    protected void createAdapter(Context context, Bundle savedInstanceState) {
        mCanvasAdapter = new CanvasAdapter(this, mImageCache);
        mDynDrawCanvas = new CanvasAdapter(this, mImageCache);
        mCanvasOnDraw = new CanvasAdapter(this, mImageCache);
        mViewAdapter = new SFViewAdapter(savedInstanceState);
    }

    protected void initView(Context context) {
        // Avoid black screen before surface created
        setWillNotDraw(false);
        // avoid unnecessary usage of the memory.
        setWillNotCacheDrawing(true);
        // see setBackgroundColor
        setZOrderMediaOverlay(true);

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
                if (mCoreView == null || !mGestureEnable) {
                    return false;
                }
                if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                    activateView();
                }
                boolean ret = mGestureListener.onTouch(v, event) || mGestureDetector.onTouchEvent(event);
                if (mGestureListener.getLastGesture() == GiGestureType.kGiGestureTap) {
                    v.performClick();
                }
                return ret;
            }
        });
    }

    @Override
    public boolean performClick() {
        return super.performClick();
    }

    protected void activateView() {
        mViewAdapter.hideContextActions();
        ViewUtil.activateView(this);
    }

    @Override
    protected void onDetachedFromWindow() {
        tearDown();
        super.onDetachedFromWindow();
    }

    @Override
    public void tearDown() {
        if (mViewAdapter == null) {
            return;
        }

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
                mImageCache.clear();
            }
            mImageCache = null;
        }
        synchronized (GiCoreView.class) {
            final LogHelper log2 = new LogHelper("GiCoreView.class synchronized");
            mCoreView.destoryView(mViewAdapter);
            mViewAdapter.delete();
            mViewAdapter = null;
            mCoreView.release();
            mCoreView = null;
            log2.r();
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

        if (mCanvasOnDraw != null && mCoreView != null && mCanvasOnDraw.beginPaint(canvas)) {
            drawShapes(mCanvasOnDraw);
            mCanvasOnDraw.setPen(0x40FF0000, 2, 0, 0, 0);
            mCanvasOnDraw.drawLine(0, 0, getWidth(), getHeight());
            mCanvasOnDraw.drawLine(0, getHeight(), getWidth(), 0);
            mCanvasOnDraw.endPaint();
        }
    }

    private int drawShapes(CanvasAdapter canvasAdapter) {
        int gs, n;
        final Longs docs = new Longs();

        synchronized (mCoreView) {
            mCoreView.acquireFrontDocs(docs);
            gs = mCoreView.acquireGraphics(mViewAdapter);
        }
        try {
            n = mCoreView.drawAll(docs, gs, canvasAdapter);
        } finally {
            GiCoreView.releaseDocs(docs);
            mCoreView.releaseGraphics(gs);
        }

        return n;
    }

    protected static class RenderRunnable implements Runnable {
        private SFGraphView mView;

        public RenderRunnable(SFGraphView view) {
            this.mView = view;
        }

        public void requestRender() {
            synchronized (this) {
                this.notify();
            }
        }

        public void stop() {
            if (mView != null) {
                final LogHelper log = new LogHelper();
                mView.getHolder().setFixedSize(1, 1);
                mView.mCoreView.stopDrawing();
                requestRender();
                synchronized (mView.mCanvasAdapter) {
                    try {
                        mView.mCanvasAdapter.wait(1000);
                    } catch (InterruptedException e) {
                        Log.w(TAG, "Render stop", e);
                    }
                }
                mView = null;
                log.r();
            }
        }

        @Override
        public void run() {
            mView.mCoreView.stopDrawing(false);
            while (!mView.mCoreView.isStopping()) {
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        Log.w(TAG, "Render run", e);
                    }
                }
                if (mView != null && !mView.mCoreView.isStopping()) {
                    render();
                }
            }
            synchronized (mView.mCanvasAdapter) {
                mView.mCanvasAdapter.notify();
            }
            Log.d(TAG, "RenderRunnable exit");
        }

        private int drawInRender(Canvas canvas) {
            int count = -1;
            int oldcnt = (mView.mDynDrawRender != null) ? mView.mDynDrawRender.getAppendCount() : 0;

            if (mView.mCanvasAdapter.beginPaint(canvas)) {
                if (mView.mBackground != null) {
                    mView.mBackground.draw(canvas);
                } else {
                    canvas.drawColor(mView.mBkColor,
                            mView.mBkColor == Color.TRANSPARENT ? Mode.CLEAR : Mode.SRC_OVER);
                }
                count = mView.drawShapes(mView.mCanvasAdapter);
                mView.mCanvasAdapter.endPaint();

                if (mView.mDynDrawRender != null) {
                    mView.mDynDrawRender.afterRegen(oldcnt);
                }
            }
            return count;
        }

        private void notUseOnDraw() {
            if (!mView.willNotDraw()) {
                try {
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    Log.w(TAG, "notUseOnDraw", e);
                }
                ((Activity) mView.getContext()).runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        if (mView != null) {
                            mView.removeCallbacks(this);
                            mView.setWillNotDraw(true);
                        }
                    }
                });
            }
        }

        protected void render() {
            Canvas canvas = null;
            final SurfaceHolder holder = mView.getHolder();
            int count = -1;

            try {
                canvas = holder.lockCanvas();
                count = drawInRender(canvas);
            } catch (Exception e) {
                Log.w(TAG, "Fail to render on the canvas", e);
            } finally {
                if (canvas != null) {
                    holder.unlockCanvasAndPost(canvas);
                    notUseOnDraw();
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

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mView.mCoreView.onSize(mView.mViewAdapter, width, height);
            mView.mCoreView.zoomToInitial();

            if (mView.mRender == null) {
                mView.mRender = new RenderRunnable(mView);
                new Thread(mView.mRender, "touchvg.render").start();
            }

            // 延时执行，只执行一次
            mView.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (mView != null) {
                        mView.removeCallbacks(this);
                        if (mView.mViewAdapter != null) {
                            mView.mViewAdapter.regenAll(false);
                        }
                    }
                }
            }, 10);
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            // start render in surfaceChanged
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
        private int[] mAppendShapeIDs = new int[20];
        private SurfaceHolder mHolder;
        private SFGraphView mView;

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
                    Log.w(TAG, "stop DynRender", e);
                }
            }
            log.r();
            mView = null;
            mHolder = null;
            mAppendShapeIDs = null;
        }

        public int getAppendCount() {
            int n = 0;
            for (int i = 0; i < mAppendShapeIDs.length; i += 2) {
                if (mAppendShapeIDs[i] != 0) {
                    n++;
                }
            }
            return n;
        }

        public final void afterRegen(int count) {
            int n = Math.min(count, getAppendCount());
            if (n > 0) {
                for (int i = 0, j = n * 2; i + 1 < mAppendShapeIDs.length; i += 2, j += 2) {
                    mAppendShapeIDs[i] = j < mAppendShapeIDs.length ? mAppendShapeIDs[j] : 0;
                    mAppendShapeIDs[i+1] = j+1 < mAppendShapeIDs.length ? mAppendShapeIDs[j+1] : 0;
                }
            }
            requestRender();
        }

        private final boolean existRequest(int i, int sid, int playh) {
            return mAppendShapeIDs[i] == sid && mAppendShapeIDs[i + 1] == playh;
        }

        public final void requestAppendRender(int sid, int playh) {
            for (int i = 0; i < mAppendShapeIDs.length && !existRequest(i, sid, playh); i += 2) {
                if (mAppendShapeIDs[i] == 0) {
                    mAppendShapeIDs[i] = sid;
                    mAppendShapeIDs[i + 1] = playh;
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
                        Log.w(TAG, "DynRender run", e);
                    }
                }
                if (mView != null && !mView.mCoreView.isStopping()) {
                    render();
                }
            }
            synchronized (mView.mDynDrawCanvas) {
                mView.mDynDrawCanvas.notify();
            }
            Log.d(TAG, "DynRenderRunnable exit");
        }

        private void drawAppendShapes(GiCoreView coreView, int doc, int gs) {
            for (int i = 0; i < mAppendShapeIDs.length; i += 2) {
                int sid = mAppendShapeIDs[i];
                if (sid != 0) {
                    coreView.drawAppend(doc, gs, mView.mDynDrawCanvas, sid);
                }
            }
        }

        protected void render() {
            Canvas canvas = null;
            try {
                canvas = mHolder.lockCanvas();
                if (mView.mDynDrawCanvas.beginPaint(canvas)) {
                    int doc = 0, gs;
                    final Longs shapes = new Longs();
                    final GiCoreView coreView = mView.mCoreView;

                    synchronized (coreView) {
                        doc = mAppendShapeIDs[0] != 0 ?
                                coreView.acquireFrontDoc(mAppendShapeIDs[1]) : 0;
                        gs = coreView.acquireGraphics(mView.mViewAdapter);
                        coreView.acquireDynamicShapesArray(shapes);
                    }
                    try {
                        canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
                        drawAppendShapes(coreView, doc, gs);
                        coreView.dynDraw(shapes, gs, mView.mDynDrawCanvas);
                    } finally {
                        GiCoreView.releaseDoc(doc);
                        GiCoreView.releaseShapesArray(shapes);
                        shapes.delete();
                        coreView.releaseGraphics(gs);
                        mView.mDynDrawCanvas.endPaint();
                    }
                }
            } catch (Exception e) {
                Log.w(TAG, "dynrender fail", e);
            } finally {
                if (canvas != null) {
                    mHolder.unlockCanvasAndPost(canvas);
                    mView.mViewAdapter.fireDynDrawEnded();
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
        public SFViewAdapter(Bundle savedInstanceState) {
            super(savedInstanceState);
        }

        @Override
        public BaseGraphView getGraphView() {
            return SFGraphView.this;
        }

        @Override
        public GestureListener getGestureListener() {
            return mGestureListener;
        }

        @Override
        public ImageCache getImageCache() {
            return mImageCache;
        }

        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, SFGraphView.this);
        }

        @Override
        public void regenAll(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                int changeCount = mCoreView.getChangeCount();

                if (!mCoreView.isUndoLoading()) {
                    recordForRegenAll(changed, changeCount);
                } else if (mRecorder != null && changed) {
                    int tick1 = mCoreView.getRecordTick(false, getTick());
                    int doc1 = mCoreView.acquireFrontDoc();
                    int shapes1 = mCoreView.acquireDynamicShapes();
                    mRecorder.requestRecord(tick1, changeCount, doc1, shapes1);
                }
            }
            if (mRender != null) {
                mRender.requestRender();
            }
        }

        @Override
        public void regenAppend(int sid, int playh) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                recordForRegenAll(true, mCoreView.getChangeCount());
            }
            if (mDynDrawRender != null) {
                mDynDrawRender.requestAppendRender(sid, playh);
            }
            if (mRender != null) {
                mRender.requestRender();
            }
        }

        @Override
        public void redraw(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying() && changed) {
                mCoreView.submitDynamicShapes(mViewAdapter);

                if (mRecorder != null) {
                    int tick1 = mCoreView.getRecordTick(false, getTick());
                    int shapes1 = mCoreView.acquireDynamicShapes();
                    mRecorder.requestRecord(tick1, 0, 0, shapes1);
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
    public int cmdViewHandle() {
        return mCoreView != null ? mCoreView.viewAdapterHandle() : 0;
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
    public void stop(OnViewDetachedListener listener) {
        if (mViewAdapter != null) {
            mViewAdapter.stop(listener);
            if (mRender != null) {
                mRender.stop();
                mRender = null;
            }
            if (mDynDrawRender != null) {
                mDynDrawRender.stop();
                mDynDrawRender = null;
            }
        }
    }

    @Override
    public boolean onPause() {
        if (mViewAdapter != null) {
            final LogHelper log = new LogHelper();
            mViewAdapter.hideContextActions();
            return log.r(mCoreView.onPause(BaseViewAdapter.getTick()));
        }
        return false;
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
        mBkColor = color;
        getHolder().setFormat(color == Color.TRANSPARENT ? PixelFormat.TRANSPARENT : PixelFormat.OPAQUE);
        if (color != Color.TRANSPARENT) {
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
    public boolean getGestureEnabled() {
        return mGestureEnable;
    }

    @Override
    public void setGestureEnabled(boolean enabled) {
        mGestureEnable = enabled;
        mGestureListener.setGestureEnabled(enabled);
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
            int n = drawShapes(canvasAdapter);
            Log.d(TAG, "snapshot: " + n);
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
            int n = mCoreView.drawAll(doc, gs, canvasAdapter);
            Log.d(TAG, "snapshot(doc): " + n);
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
    public void setOnZoomChangedListener(OnZoomChangedListener listener) {
        mViewAdapter.setOnZoomChangedListener(listener);
    }

    @Override
    public void setOnFirstRegenListener(OnFirstRegenListener listener) {
        mViewAdapter.setOnFirstRegenListener(listener);
    }

    @Override
    public void setOnDynDrawEndedListener(OnDynDrawEndedListener listener) {
        mViewAdapter.setOnDynDrawEndedListener(listener);
    }

    @Override
    public void setOnShapesRecordedListener(OnShapesRecordedListener listener) {
        mViewAdapter.setOnShapesRecordedListener(listener);
    }

    @Override
    public void setOnShapeWillDeleteListener(OnShapeWillDeleteListener listener) {
        mViewAdapter.setOnShapeWillDeleteListener(listener);
    }

    @Override
    public void setOnShapeDeletedListener(OnShapeDeletedListener listener) {
        mViewAdapter.setOnShapeDeletedListener(listener);
    }

    @Override
    public void setOnShapeClickedListener(OnShapeClickedListener listener) {
        mViewAdapter.setOnShapeClickedListener(listener);
    }

    @Override
    public void setOnShapeDblClickedListener(OnShapeDblClickedListener listener) {
        mViewAdapter.setOnShapeDblClickedListener(listener);
    }

    @Override
    public void setOnContextActionListener(OnContextActionListener listener) {
        mViewAdapter.setOnContextActionListener(listener);
    }

    @Override
    public void setOnGestureListener(OnDrawGestureListener listener) {
        mViewAdapter.setOnGestureListener(listener);
    }
}
