//! \file StdGraphView.java
//! \brief Android绘图视图类
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.core.GiCoreView;
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
 * \ingroup GROUP_ANDROID
 * Android绘图视图类.
 * 
 * 建议使用FrameLayout作为容器创建绘图视图，使用LinearLayout将无法显示上下文操作按钮。
 */
public class StdGraphView extends View implements BaseGraphView, GestureNotify {
    protected static final String TAG = "touchvg";
    protected ImageCache mImageCache;                       // 图像对象缓存
    protected CanvasAdapter mCanvasAdapter;                 // onDraw用的画布适配器
    protected CanvasAdapter mCanvasRegen;                   // 渲染线程用的画布适配器
    protected StdViewAdapter mViewAdapter;                  // 视图回调适配器
    protected GiCoreView mCoreView;                         // 内核视图分发器
    protected GestureDetector mGestureDetector;             // 手势识别器
    protected GestureListener mGestureListener;             // 手势识别实现
    protected boolean mGestureEnable = true;                // 是否允许交互
    private boolean mRegenning = false;                     // 是否正在regenAll
    private Bitmap mCachedBitmap;                           // 缓存快照
    private Bitmap mRegenBitmap;                            // 渲染线程用的缓存位图
    private int mBkColor = Color.TRANSPARENT;               // 背景色
    private IGraphView mMainView;                           // 本视图为放大镜时对应的主视图

    static {
        System.loadLibrary("touchvg");
    }

    protected void finalize() {
        Log.d(TAG, "StdGraphView finalize");
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

    //! 放大镜视图的构造函数
    public StdGraphView(Context context, BaseGraphView mainView) {
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
            setLayerType(View.LAYER_TYPE_SOFTWARE, null); // 避免路径太大不能渲染
        }

        this.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if (mCoreView == null) {
                    return false;
                }
                if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                    activateView();
                }
                return mGestureEnable && (mGestureListener.onTouch(v, event)
                        || mGestureDetector.onTouchEvent(event));
            }
        });
    }

    protected void activateView() {
        mViewAdapter.hideContextActions();
        ViewUtil.activateView(this);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
        if (mCachedBitmap != null) {
            drawShapes(canvas, mCanvasAdapter, true);
        } else if (!regen(false)) { // 首次onDraw，但视图太大无法创建缓存位图
            canvas.drawColor(mBkColor);
            drawShapes(canvas, mCanvasAdapter, true);
        }
    }

    private int drawShapes(Canvas canvas, CanvasAdapter adapter, boolean dyndraw) {
        int gs;
        final Longs docs = new Longs();
        final Longs shapes = new Longs();

        synchronized (mCoreView) {
            if (mCachedBitmap == null || !dyndraw)
                mCoreView.acquireFrontDocs(docs);
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

        if (adapter.beginPaint(canvas, dyndraw)) {
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
        if (getWidth() < 2 || getHeight() < 2 || mRegenning) {
            return true;
        }

        try {
            if (mCachedBitmap == null) {
                mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(),
                        Bitmap.Config.ARGB_8888);
            } else if (mRegenBitmap == null) {
                mRegenBitmap = Bitmap
                        .createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (mCachedBitmap != null) {
            mRegenning = true;
            new Thread(new Runnable() {
                protected void finalize() {
                    Log.d(TAG, "RegenRunnable finalize");
                }

                @Override
                public void run() {
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
                        e.printStackTrace();
                    }
                    mRegenBitmap = null;
                    mRegenning = false;
                    if (count >= 0 && !mCoreView.isStopping()) {
                        mViewAdapter.onFirstRegen();
                    }
                }
            }, "touchvg.regen").start();
        } else if (fromRegenAll) { // 视图太大，无法后台绘制，将直接在onDraw中显示
            postInvalidate();
        }

        return mCachedBitmap != null;
    }

    @Override
    protected void onDetachedFromWindow() {
        ViewUtil.onRemoveView(this);
        mViewAdapter.stop();

        if (mImageCache != null) {
            synchronized (mImageCache) {
            }
            mImageCache.clear();
            mImageCache = null;
        }
        if (mRegenBitmap != null) {
            synchronized (mRegenBitmap) {
            }
            mRegenBitmap.recycle();
            mRegenBitmap = null;
        }
        if (mCachedBitmap != null) {
            synchronized (mCachedBitmap) {
            }
            mCachedBitmap.recycle();
            mCachedBitmap = null;
        }
        if (mViewAdapter != null) {
            synchronized (GiCoreView.class) {
                mCoreView.destoryView(mViewAdapter);
                mViewAdapter.delete();
                mViewAdapter = null;
                mCoreView.delete();
                mCoreView = null;
            }
        }
        if (mCanvasAdapter != null) {
            mCanvasAdapter.delete();
            mCanvasAdapter = null;
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

        super.onDetachedFromWindow();
    }

    //! 视图回调适配器
    protected class StdViewAdapter extends BaseViewAdapter {
        protected void finalize() {
            Log.d(TAG, "StdViewAdapter finalize");
        }

        public StdViewAdapter(Bundle savedInstanceState) {
            super(savedInstanceState);
        }

        @Override
        public BaseGraphView getGraphView() {
            return StdGraphView.this;
        }

        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, StdGraphView.this);
        }

        @Override
        public void regenAll(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                if (!mCoreView.isUndoLoading()) {
                    if (changed || mViewAdapter.getRegenCount() == 0)
                        mCoreView.submitBackDoc(mViewAdapter, changed);
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
                mCoreView.submitBackDoc(mViewAdapter, true);
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
            if (mCachedBitmap != null && !mRegenning) {
                int docd = mCoreView.acquireFrontDoc(playh);
                int gs = mCoreView.acquireGraphics(mViewAdapter);

                synchronized (mCachedBitmap) {
                    if (mCanvasAdapter.beginPaint(new Canvas(mCachedBitmap))) {
                        mCoreView.drawAppend(docd, gs, mCanvasAdapter, sid);
                        mCanvasAdapter.endPaint();
                    }
                }
                GiCoreView.releaseDoc(docd);
                mCoreView.releaseGraphics(gs);
            }
            postInvalidate();
        }

        @Override
        public void redraw(boolean changed) {
            if (mCoreView != null && !mCoreView.isPlaying()) {
                if (changed) {
                    mCoreView.submitDynamicShapes(mViewAdapter);

                    if (mRecorder != null) {
                        int tick1 = mCoreView.getRecordTick(false, getTick());
                        int shapes1 = mCoreView.acquireDynamicShapes();
                        mRecorder.requestRecord(tick1, 0, shapes1);
                    }
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
    public void stop() {
        if (mViewAdapter != null) {
            mViewAdapter.stop();
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
        if (mCachedBitmap == null) {
            mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            synchronized (mCachedBitmap) {
                mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
                mCachedBitmap.eraseColor(transparent ? Color.TRANSPARENT : mBkColor);
                drawShapes(new Canvas(mCachedBitmap), mCanvasAdapter, false);
            }
        }
        return mCachedBitmap;
    }

    @Override
    public Bitmap snapshot(int doc, int gs, boolean transparent) {
        if (mCachedBitmap == null) {
            mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            synchronized (mCachedBitmap) {
                mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
                mCachedBitmap.eraseColor(transparent ? Color.TRANSPARENT : mBkColor);
                final Longs docs = new Longs(doc, 0);
                drawShapes(docs, gs, null, new Canvas(mCachedBitmap), mCanvasAdapter, false);
            }
        }
        return mCachedBitmap;
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
    public void setOnShapesRecordedListener(OnShapesRecordedListener listener) {
        mViewAdapter.setOnShapesRecordedListener(listener);
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
    public void setOnContextActionListener(OnContextActionListener listener) {
        mViewAdapter.setOnContextActionListener(listener);
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
