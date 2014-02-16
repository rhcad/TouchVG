//! \file StdGraphView.java
//! \brief Android绘图视图类
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
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
public class StdGraphView extends View implements GraphView {
    protected static final String TAG = "touchvg";
    protected ImageCache mImageCache;           // 图像对象缓存
    protected CanvasAdapter mCanvasAdapter;     // onDraw用的画布适配器
    protected CanvasAdapter mCanvasRegen;       // regen用的画布适配器
    protected StdViewAdapter mViewAdapter;      // 视图回调适配器
    protected GiCoreView mCoreView;             // 内核视图分发器
    protected GestureDetector mGestureDetector; // 手势识别器
    protected GestureListener mGestureListener; // 手势识别实现
    protected boolean mGestureEnable = true;    // 是否允许交互
    private boolean mRegenning = false;         // 是否正在regenAll
    private Bitmap mCachedBitmap;               // 缓存快照
    private Bitmap mRegenBitmap;                // regen用的缓存位图
    private int mBkColor = Color.TRANSPARENT;

    static {
        System.loadLibrary("touchvg");
    }

    //! 普通绘图视图的构造函数
    public StdGraphView(Context context) {
        super(context);
        createAdapter(context);
        mCoreView = new GiCoreView(null);
        mCoreView.createView(mViewAdapter);
        initView(context);
        if (ViewUtil.activeView == null) {
            ViewUtil.activeView = this;
        }
    }

    //! 放大镜视图的构造函数
    public StdGraphView(Context context, GraphView mainView) {
        super(context);
        createAdapter(context);
        mCoreView = new GiCoreView(mainView.coreView());
        mCoreView.createMagnifierView(mViewAdapter, mainView.viewAdapter());
        initView(context);
    }

    protected void createAdapter(Context context) {
        mImageCache = new ImageCache();
        mCanvasAdapter = new CanvasAdapter(this, mImageCache);
        mCanvasRegen = new CanvasAdapter(this, mImageCache);
        mViewAdapter = new StdViewAdapter();
    }

    protected void initView(Context context) {
        mGestureListener = new GestureListener(mCoreView, mViewAdapter);
        mGestureDetector = new GestureDetector(context, mGestureListener);
        ResourceUtil.setContextImages(context);

        final DisplayMetrics dm = context.getApplicationContext().getResources()
                .getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi, dm.density);
        if (mCanvasRegen != null) {
            setLayerType(View.LAYER_TYPE_SOFTWARE, null); // 避免路径太大不能渲染
        }

        this.setOnTouchListener(new OnTouchListener() {
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
        if (ViewUtil.activeView != this) {
            ViewUtil.activeView = this;
        }
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
        int n = 0, doc = 0, shapes, gs;

        synchronized (mCoreView) {
            if (mCachedBitmap == null || !dyndraw)
                doc = mCoreView.acquireFrontDoc();
            shapes = dyndraw ? mCoreView.acquireDynamicShapes() : 0;
            gs = mCoreView.acquireGraphics(mViewAdapter);
        }
        if (adapter.beginPaint(canvas)) {
            if (mCachedBitmap == null || !dyndraw) {
                if (this.getBackground() != null) {
                    this.getBackground().draw(canvas);
                }
                n = mCoreView.drawAll(doc, gs, adapter);
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
        GiCoreView.releaseDoc(doc);
        GiCoreView.releaseShapes(shapes);
        mCoreView.releaseGraphics(gs);

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
                public void run() {
                    Bitmap bmp = mRegenBitmap != null ? mRegenBitmap : mCachedBitmap;
                    try {
                        synchronized (bmp) {
                            bmp.eraseColor(mBkColor);
                            drawShapes(new Canvas(bmp), mCanvasRegen, false);

                            if (bmp == mRegenBitmap) {
                                if (mCachedBitmap != null) {
                                    mCachedBitmap.recycle();
                                }
                                mCachedBitmap = mRegenBitmap;
                            }
                        }
                        postInvalidate();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    mRegenBitmap = null;
                    mRegenning = false;
                }
            }, "touchvg.regen").start();
        } else if (fromRegenAll) { // 视图太大，无法后台绘制，将直接在onDraw中显示
            postInvalidate();
        }

        return mCachedBitmap != null;
    }

    @Override
    protected void onDetachedFromWindow() {
        if (ViewUtil.activeView == this) {
            ViewUtil.activeView = null;
        }
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
            mCoreView.destoryView(mViewAdapter);
            mViewAdapter.delete();
            mViewAdapter = null;
        }
        if (mCoreView != null) {
            mCoreView.delete();
            mCoreView = null;
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

        super.onDetachedFromWindow();
    }

    //! 视图回调适配器
    protected class StdViewAdapter extends BaseViewAdapter {

        @Override
        protected GraphView getGraphView() {
            return StdGraphView.this;
        }

        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, StdGraphView.this);
        }

        @Override
        public void regenAll(boolean changed) {
            if (!mCoreView.isPlaying()) {
                if (mCoreView.isUndoLoading()) {
                    if (mRecorder != null) {
                        int tick1 = mCoreView.getRecordTick(false);
                        int doc1 = changed ? mCoreView.acquireFrontDoc() : 0;
                        int shapes1 = mCoreView.acquireDynamicShapes();
                        mRecorder.requestRecord(tick1, doc1, shapes1);
                    }
                } else {
                    synchronized (mCoreView) {
                        if (changed)
                            mCoreView.submitBackDoc(mViewAdapter);
                        mCoreView.submitDynamicShapes(mViewAdapter);

                        if (mUndoing != null) {
                            int tick0 = mCoreView.getRecordTick(true);
                            int doc0 = changed ? mCoreView.acquireFrontDoc() : 0;
                            int shapes0 = mCoreView.acquireDynamicShapes();
                            mUndoing.requestRecord(tick0, doc0, shapes0);
                        }
                        if (mRecorder != null) {
                            int tick1 = mCoreView.getRecordTick(false);
                            int doc1 = changed ? mCoreView.acquireFrontDoc() : 0;
                            int shapes1 = mCoreView.acquireDynamicShapes();
                            mRecorder.requestRecord(tick1, doc1, shapes1);
                        }
                    }
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
        public void regenAppend(int sid) {
            int docd = 0, gs = 0;

            if (!mCoreView.isPlaying()) {
                synchronized (mCoreView) {
                    mCoreView.submitBackDoc(mViewAdapter);
                    mCoreView.submitDynamicShapes(mViewAdapter);

                    if (mCachedBitmap != null && !mRegenning) {
                        docd = mCoreView.acquireFrontDoc();
                        gs = mCoreView.acquireGraphics(mViewAdapter);
                    }
                    if (mUndoing != null) {
                        int tick0 = mCoreView.getRecordTick(true);
                        int doc0 = mCoreView.acquireFrontDoc();
                        int shapes0 = mCoreView.acquireDynamicShapes();
                        mUndoing.requestRecord(tick0, doc0, shapes0);
                    }
                    if (mRecorder != null) {
                        int tick1 = mCoreView.getRecordTick(false);
                        int doc1 = mCoreView.acquireFrontDoc();
                        int shapes1 = mCoreView.acquireDynamicShapes();
                        mRecorder.requestRecord(tick1, doc1, shapes1);
                    }
                }
            } else if (mCachedBitmap != null && !mRegenning) {
                docd = mCoreView.acquireFrontDoc();
                gs = mCoreView.acquireGraphics(mViewAdapter);
            }
            if (mCachedBitmap != null && !mRegenning) {
                synchronized (mCachedBitmap) {
                    if (mCanvasAdapter.beginPaint(new Canvas(mCachedBitmap))) {
                        mCoreView.drawAppend(docd, gs, mCanvasAdapter, sid);
                        mCanvasAdapter.endPaint();
                    }
                }
            }
            GiCoreView.releaseDoc(docd);
            mCoreView.releaseGraphics(gs);
            postInvalidate();
        }

        @Override
        public void redraw() {
            if (!mCoreView.isPlaying()) {
                synchronized (mCoreView) {
                    mCoreView.submitDynamicShapes(mViewAdapter);
                    if (mUndoing != null) {
                        int tick0 = mCoreView.getRecordTick(true);
                        int shapes0 = mCoreView.acquireDynamicShapes();
                        mUndoing.requestRecord(tick0, 0, shapes0);
                    }
                    if (mRecorder != null) {
                        int tick1 = mCoreView.getRecordTick(false);
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
    public View getView() {
        return this;
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
    public void onPause() {
        mViewAdapter.stop();
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
}
