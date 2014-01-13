//! \file StdGraphView.java
//! \brief Android绘图视图类
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package touchvg.view;

import touchvg.core.GiCoreView;
import touchvg.core.GiView;
import touchvg.view.internal.BaseViewAdapter;
import touchvg.view.internal.ContextAction;
import touchvg.view.internal.GestureListener;
import touchvg.view.internal.ImageCache;
import touchvg.view.internal.ResourceUtil;
import touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

//! Android绘图视图类
/*! \ingroup GROUP_ANDROID
 *  建议使用FrameLayout作为容器创建绘图视图，使用LinearLayout将无法显示上下文操作按钮。
 */
public class StdGraphView extends View implements GraphView {
    protected static final String TAG = "touchvg";
    protected ImageCache mImageCache;         // 图像对象缓存
    protected CanvasAdapter mCanvasAdapter;   // onDraw用的画布适配器
    protected CanvasAdapter mCanvasRegen;     // regen用的画布适配器
    protected StdViewAdapter mViewAdapter;    // 视图回调适配器
    protected GiCoreView mCoreView;           // 内核视图分发器
    protected GestureDetector mGestureDetector; // 手势识别器
    protected GestureListener mGestureListener; // 手势识别实现
    protected boolean mGestureEnable = true;  // 是否允许交互
    private boolean mRegenning = false;       // 是否正在regenAll
    private Bitmap mCachedBitmap;             // 缓存快照
    private Bitmap mRegenBitmap;              // regen用的缓存位图
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
        
        final DisplayMetrics dm = context.getApplicationContext().getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi, dm.density);
        if (mCanvasRegen != null) {
            setLayerType(View.LAYER_TYPE_SOFTWARE, null);   // 避免路径太大不能渲染
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
        }
        else if (!regen(false)) {       // 首次onDraw，但视图太大无法创建缓存位图
            canvas.drawColor(mBkColor);
            drawShapes(canvas, mCanvasAdapter, true);
        }
    }
    
    private int drawShapes(Canvas canvas, CanvasAdapter adapter, boolean dyndraw) {
        int n = 0;
        
        if (adapter.beginPaint(canvas)) {
            if (mCachedBitmap == null || !dyndraw) {
                if (this.getBackground() != null) {
                    this.getBackground().draw(canvas);
                }
                n = mCoreView.drawAll(mViewAdapter, adapter);
            }
            else if (mCachedBitmap != null) {
                synchronized(mCachedBitmap) {
                    canvas.drawBitmap(mCachedBitmap, 0, 0, null);
                    n++;
                }
            }
            
            if (dyndraw) {
                mCoreView.dynDraw(mViewAdapter, adapter);
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
            }
            else if (mRegenBitmap == null) {
                mRegenBitmap = Bitmap.createBitmap(getWidth(), getHeight(),
                                                   Bitmap.Config.ARGB_8888);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        if (mCachedBitmap != null) {
            mRegenning = true;
            new Thread(new Runnable() {
                public void run() {
                    Bitmap bmp = mRegenBitmap != null ? mRegenBitmap : mCachedBitmap;
                    synchronized(bmp) {
                        bmp.eraseColor(mBkColor);
                        drawShapes(new Canvas(bmp), mCanvasRegen, false);
                        
                        if (bmp == mRegenBitmap) {
                            if (mCachedBitmap != null) {
                                mCachedBitmap.recycle();
                            }
                            mCachedBitmap = mRegenBitmap;
                        }
                    }
                    mRegenBitmap = null;
                    mRegenning = false;
                    postInvalidate();
                }
            }).start();
        }
        else if (fromRegenAll) {    // 视图太大，无法后台绘制，将直接在onDraw中显示
            invalidate();
        }
        
        return mCachedBitmap != null;
    }
    
    @Override
    protected void onDetachedFromWindow() {
        if (ViewUtil.activeView == this) {
            ViewUtil.activeView = null;
        }
        if (mImageCache != null) {
            synchronized(mImageCache) {}
            mImageCache.clear();
            mImageCache = null;
        }
        if (mCachedBitmap != null) {
            synchronized(mCachedBitmap) {}
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
        protected GraphView getView() {
            return StdGraphView.this;
        }
        
        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, StdGraphView.this);
        }
        
        @Override
        public void regenAll(boolean changed) {
            mCoreView.submitBackDoc();
            mCoreView.submitDynamicShapes(mViewAdapter);
            if (mCachedBitmap != null && !mRegenning
                    && (mCachedBitmap.getWidth() != getWidth()
                    || mCachedBitmap.getHeight() != getHeight())) {
                mCachedBitmap.recycle();
                mCachedBitmap = null;
            }
            regen(true);
        }
        
        @Override
        public void regenAppend(int sid) {
            mCoreView.submitBackDoc();
            mCoreView.submitDynamicShapes(mViewAdapter);
            if (mCachedBitmap != null && !mRegenning) {
                synchronized(mCachedBitmap) {
                    if (mCanvasAdapter.beginPaint(new Canvas(mCachedBitmap))) {
                        mCoreView.drawAppend(mViewAdapter, mCanvasAdapter, sid);
                        mCanvasAdapter.endPaint();
                    }
                }
            }
            invalidate();
        }
        
        @Override
        public void redraw() {
            mCoreView.submitDynamicShapes(mViewAdapter);
            invalidate();
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
    public void setBackgroundColor(int color) {
        mBkColor = color;
        mCoreView.setBkColor(mViewAdapter, color);
        regen(false);
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
    public boolean onTouch(int action, float x, float y) {
        return mGestureListener.onTouch(this, action, x, y);
    }
    
    @Override
    public boolean onTap(float x, float y) {
        return mGestureListener.onTap(x, y);
    }
    
    @Override
    public Bitmap snapshot() {
        if (mCachedBitmap == null) {
            mCachedBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            synchronized(mCachedBitmap) {
                mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
                mCachedBitmap.eraseColor(mBkColor);
                drawShapes(new Canvas(mCachedBitmap), mCanvasAdapter, false);
            }
        }
        return mCachedBitmap;
    }
    
    @Override
    public void setOnCommandChangedListener(CommandChangedListener listener) {
        mViewAdapter.setOnCommandChangedListener(listener);
    }
    
    @Override
    public void setOnSelectionChangedListener(SelectionChangedListener listener) {
        mViewAdapter.setOnSelectionChangedListener(listener);
    }
    
    @Override
    public void setOnContentChangedListener(ContentChangedListener listener) {
        mViewAdapter.setOnContentChangedListener(listener);
    }
    
    @Override
    public void setOnDynamicChangedListener(DynamicChangedListener listener) {
        mViewAdapter.setOnDynamicChangedListener(listener);
    }
}
