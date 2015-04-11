//! \file StdGraphView.java
//! \brief Android绘图视图类
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
import rhcad.touchvg.view.internal.ResourceUtil;
import rhcad.touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

/**
 * Android绘图视图类.
 * 
 * 建议使用FrameLayout作为容器创建绘图视图，使用LinearLayout将无法显示上下文操作按钮。
 */
public class StdGraphView extends View implements BaseGraphView {
    protected static final String TAG = "touchvg";
    protected ImageCache mImageCache;
    protected CanvasAdapter mCanvasOnDraw;
    protected CanvasAdapter mCanvasRegen;
    protected StdViewAdapter mViewAdapter;
    protected GiCoreView mCoreView;
    protected GestureDetector mGestureDetector;
    protected GestureListener mGestureListener;
    protected boolean mGestureEnable = true;
    private boolean mRegenning = false;
    private Bitmap mCachedBitmap;
    private Bitmap mRegenBitmap;
    private int mDrawCount;
    private int mBkColor = Color.TRANSPARENT;
    private IGraphView mMainView;

    static {
        System.loadLibrary(TAG);
    }

    //! 普通绘图视图的构造函数
    public StdGraphView(Context context) {
        this(context, (Bundle)null);
    }

    //! 构造绘图视图，允许在Activity的onCreate(Bundle)或onRestoreInstanceState(Bundle)中调用
    public StdGraphView(Context context, Bundle savedInstanceState) {
        super(context);
        mImageCache = new ImageCache();
        createAdapter(context, savedInstanceState);
        mCoreView = GiCoreView.createView(mViewAdapter);
        initView(context);
        ViewUtil.onAddView(this);
    }

    //! 放大镜视图(mainView有效)或临时视图(mainView=nil)的构造函数
    public StdGraphView(Context context, BaseGraphView mainView) {
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
        mCanvasOnDraw = new CanvasAdapter(this, mImageCache);
        mCanvasRegen = new CanvasAdapter(this, mImageCache);
        mViewAdapter = new StdViewAdapter(savedInstanceState);
    }

    protected void initView(Context context) {
        mGestureListener = new GestureListener(mCoreView, mViewAdapter, this);
        mGestureDetector = new GestureDetector(context, mGestureListener);
        ResourceUtil.setContextImages(context);

        final DisplayMetrics dm = context.getApplicationContext().getResources()
                .getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi, dm.density);
        if (mCanvasRegen != null) {
            // 避免路径太大不能渲染
            setLayerType(View.LAYER_TYPE_SOFTWARE, null);
        }

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
    protected void onDraw(Canvas canvas) {
        mDrawCount++;
        mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
        if (mCachedBitmap != null) {
            drawShapes(canvas, mCanvasOnDraw, true);
            mViewAdapter.fireDynDrawEnded();
        } else if (!regen(false)) {
            // 首次onDraw，但视图太大无法创建缓存位图
            canvas.drawColor(mBkColor);
            drawShapes(canvas, mCanvasOnDraw, true);
        }
    }

    private int drawShapes(Canvas canvas, CanvasAdapter adapter, boolean dyndraw) {
        int gs;
        final Longs docs = new Longs();
        final Longs shapes = new Longs();

        synchronized (mCoreView) {
            if (mCachedBitmap == null || !dyndraw) {
                mCoreView.acquireFrontDocs(docs);
            }
            gs = mCoreView.acquireGraphics(mViewAdapter);
            mCoreView.acquireDynamicShapesArray(shapes);
        }
        try {
            return drawShapes(docs, gs, shapes, canvas, adapter, dyndraw);
        } finally {
            GiCoreView.releaseDocs(docs);
            GiCoreView.releaseShapesArray(shapes);
            shapes.delete();
            mCoreView.releaseGraphics(gs);
        }
    }

    private int drawShapes(Longs docs, int gs, Longs shapes, Canvas canvas,
            CanvasAdapter adapter, boolean dyndraw) {
        int n = 0;

        if (adapter.beginPaint(canvas)) {
            if (mCachedBitmap == null || !dyndraw) {
                if (this.getBackground() != null) {
                    this.getBackground().draw(canvas);
                }
                n = mCoreView.drawAll(docs, gs, adapter);
            } else if (mCachedBitmap != null) {
                synchronized (mCachedBitmap) {
                    canvas.drawBitmap(mCachedBitmap, 0, 0, null);
                    n++;
                }
            }
            if (dyndraw) {
                mCoreView.dynDraw(shapes, gs, adapter);
            }
            adapter.endPaint();
        }

        return n;
    }

    private boolean regen(boolean fromRegenAll) {
        if (mDrawCount == 0 || getWidth() < 2 || getHeight() < 2 || mRegenning) {
            return true;
        }

        createCachedOrRegenBitmap();
        if (mCachedBitmap != null) {
            mRegenning = true;
            new Thread(new Runnable() {
                @Override
                public void run() {
                    regenInRunnable();
                }
            }, "touchvg.regen").start();
        } else if (fromRegenAll) {
            // 视图太大，无法后台绘制，将直接在onDraw中显示
            postInvalidate();
        }

        return mCachedBitmap != null;
    }

    private void createCachedOrRegenBitmap() {
        try {
            if (mCachedBitmap == null) {
                mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Config.ARGB_8888);
            } else if (mRegenBitmap == null) {
                mRegenBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Config.ARGB_8888);
            }
        } catch (Exception e) {
            Log.w(TAG, "Fail to create the cached bitmap", e);
        }
    }

    private void regenInRunnable() {
        Bitmap bmp = mRegenBitmap != null ? mRegenBitmap : mCachedBitmap;
        int count = -1;
        try {
            synchronized (bmp) {
                bmp.eraseColor(mBkColor);
                count = drawShapes(new Canvas(bmp), mCanvasRegen, false);

                if (bmp == mRegenBitmap) {
                    if (mCachedBitmap != null) {
                        mCachedBitmap.recycle();
                    }
                    mCachedBitmap = mRegenBitmap;
                }
            }
            if (!mCoreView.isStopping()) {
                postInvalidate();
            }
        } catch (Exception e) {
            Log.w(TAG, "Fail to draw on the cached bitmap", e);
        }
        mRegenBitmap = null;
        mRegenning = false;
        if (count >= 0 && !mCoreView.isStopping()) {
            mViewAdapter.onFirstRegen();
        }
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

        ViewUtil.onRemoveView(this);
        mViewAdapter.stop(null);

        if (mImageCache != null) {
            synchronized (mImageCache) {
                mImageCache.clear();
            }
            mImageCache = null;
        }
        if (mRegenBitmap != null) {
            synchronized (mRegenBitmap) {
                mRegenBitmap.recycle();
            }
            mRegenBitmap = null;
        }
        if (mCachedBitmap != null) {
            synchronized (mCachedBitmap) {
                mCachedBitmap.recycle();
            }
            mCachedBitmap = null;
        }
        synchronized (GiCoreView.class) {
            mCoreView.destoryView(mViewAdapter);
            mViewAdapter.delete();
            mViewAdapter = null;
            mCoreView.delete();
            mCoreView = null;
        }
        if (mCanvasOnDraw != null) {
            mCanvasOnDraw.delete();
            mCanvasOnDraw = null;
        }
        if (mCanvasRegen != null) {
            mCanvasRegen.delete();
            mCanvasRegen = null;
        }
        if (mGestureListener != null) {
            mGestureListener.release();
            mGestureListener = null;
        }
        mGestureDetector = null;
        mMainView = null;
    }

    //! 视图回调适配器
    protected class StdViewAdapter extends BaseViewAdapter {

        public StdViewAdapter(Bundle savedInstanceState) {
            super(savedInstanceState);
        }

        @Override
        public BaseGraphView getGraphView() {
            return StdGraphView.this;
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
            return new ContextAction(mCoreView, StdGraphView.this);
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
            if (mCachedBitmap != null
                    && !mRegenning
                    && (mCachedBitmap.getWidth() != getWidth()
                    || mCachedBitmap.getHeight() != getHeight())) {
                mCachedBitmap.recycle();
                mCachedBitmap = null;
            }
            regen(true);
        }

        @Override
        public void regenAppend(int sid, int playh) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                recordForRegenAll(true, mCoreView.getChangeCount());
            }
            if (mCachedBitmap != null && !mRegenning) {
                int docd = mCoreView.acquireFrontDoc(playh);
                int gs = mCoreView.acquireGraphics(mViewAdapter);

                synchronized (mCachedBitmap) {
                    if (mCanvasOnDraw.beginPaint(new Canvas(mCachedBitmap))) {
                        mCoreView.drawAppend(docd, gs, mCanvasOnDraw, sid);
                        mCanvasOnDraw.endPaint();
                    }
                }
                GiCoreView.releaseDoc(docd);
                mCoreView.releaseGraphics(gs);
            }
            postInvalidate();
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
            postInvalidate();
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

    @Override
    public View createDynamicShapeView(Context context) {
        return null;
    }

    @Override
    public ImageCache getImageCache() {
        return mImageCache;
    }

    @Override
    public void clearCachedData() {
        mCoreView.clearCachedData();
        if (mCachedBitmap != null) {
            mCachedBitmap.recycle();
            mCachedBitmap = null;
        }
    }

    @Override
    public void stop(OnViewDetachedListener listener) {
        if (mViewAdapter != null) {
            mViewAdapter.stop(listener);
        }
    }

    @Override
    public boolean onPause() {
        if (mViewAdapter != null) {
            mViewAdapter.hideContextActions();
            return mCoreView.onPause(BaseViewAdapter.getTick());
        }
        return false;
    }

    @Override
    public boolean onResume() {
        return mCoreView.onResume(BaseViewAdapter.getTick());
    }

    @Override
    public void setBackgroundColor(int color) {
        mBkColor = color;
        mCoreView.setBkColor(mViewAdapter, color);
        regen(false);
    }

    @Override
    public void setBackgroundDrawable(Drawable background) {
        Log.e(TAG, "Called unimplemented API: setBackgroundDrawable");
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
        if (mCachedBitmap != null && transparent == (mBkColor == Color.TRANSPARENT)) {
            synchronized (mCachedBitmap) {
                return mCachedBitmap.copy(Config.ARGB_8888, false);
            }
        }

        final Bitmap bitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        final CanvasAdapter canvasAdapter = new CanvasAdapter(this, mImageCache);

        bitmap.eraseColor(transparent ? Color.TRANSPARENT : mBkColor);
        int n = drawShapes(new Canvas(bitmap), canvasAdapter, false);
        Log.d(TAG, "snapshot: " + n);
        canvasAdapter.delete();

        return bitmap;
    }

    @Override
    public Bitmap snapshot(int doc, int gs, boolean transparent) {
        final Bitmap bitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        final CanvasAdapter canvasAdapter = new CanvasAdapter(this, mImageCache);
        final Longs docs = new Longs(doc, 0);

        bitmap.eraseColor(transparent ? Color.TRANSPARENT : mBkColor);
        int n = drawShapes(docs, gs, null, new Canvas(bitmap), canvasAdapter, false);
        Log.d(TAG, "snapshot(doc): " + n);
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
