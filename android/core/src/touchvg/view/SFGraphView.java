// Copyright (c) 2014, https://github.com/rhcad/touchvg

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
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class SFGraphView extends SurfaceView implements GraphView {
    private GiCoreView mCoreView;
    private ViewAdapter mViewAdapter = new ViewAdapter();
    private ImageCache mImageCache = new ImageCache();
    private CanvasAdapter mCanvasAdapter;
    private RenderRunnable mRender;
    private SurfaceView mDynDrawView;
    private CanvasAdapter mDynDrawCanvas;
    private GestureDetector mGestureDetector;
    private GestureListener mGestureListener;
    private boolean mGestureEnable = true;
    
    static {
        System.loadLibrary("touchvg");
    }
    
    public SFGraphView(Context context) {
        super(context);
        mCoreView = new GiCoreView();
        mCoreView.createView(mViewAdapter);
        initView(context);
        if (ViewUtil.activeView == null) {
            ViewUtil.activeView = this;
        }
    }
    
    public SFGraphView(Context context, GraphView mainView) {
        super(context);
        mCoreView = new GiCoreView(mainView.coreView());
        mCoreView.createMagnifierView(mViewAdapter, mainView.viewAdapter());
        initView(context);
    }
    
    private void initView(Context context) {
        getHolder().setFormat(PixelFormat.TRANSPARENT);
        getHolder().addCallback(new SurfaceCallback());
        setZOrderMediaOverlay(true);
        
        mGestureListener = new GestureListener(mCoreView, mViewAdapter);
        mGestureDetector = new GestureDetector(context, mGestureListener);
        ResourceUtil.setContextImages(context);
        
        final DisplayMetrics dm = context.getApplicationContext()
                .getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi, dm.density);
        
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
    
    private void activateView() {
        mViewAdapter.removeContextButtons();
        if (ViewUtil.activeView != this) {
            ViewUtil.activeView = this;
        }
    }
    
    @Override
    protected void onDetachedFromWindow() {
        if (ViewUtil.activeView == this) {
            ViewUtil.activeView = null;
        }
        mDynDrawView = null;
        
        super.onDetachedFromWindow();
        
        if (mImageCache != null) {
            synchronized(mImageCache) {}
            mImageCache.clear();
            mImageCache = null;
        }
        mGestureListener.release();
        mCoreView.destoryView(mViewAdapter);
        mViewAdapter.delete();
        mCoreView.delete();
    }
    
    private class RenderRunnable implements Runnable {
        
        public void requestRender() {
            synchronized(this) {
                this.notify();
            }
        }
        
        @Override
        public void run() {
            while (mCanvasAdapter != null) {
                synchronized (this) {
                    try {
                        this.wait();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                render();
            }
        }
        
        private void render() {
            Canvas canvas = null;
            try {
                canvas = (mCanvasAdapter != null) ? getHolder().lockCanvas() : null;
                if (canvas != null && mCanvasAdapter.beginPaint(canvas)) {
                    int doc, gs;
                    
                    synchronized (mCoreView) {
                        doc = mCoreView.acquireFrontDoc();
                        gs = mCoreView.acquireGraphics(mViewAdapter);
                    }
                    
                    canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
                    mCoreView.drawAll(doc, gs, mCanvasAdapter);
                    
                    mCoreView.releaseDoc(doc);
                    mCoreView.releaseGraphics(mViewAdapter, gs);
                    mCanvasAdapter.endPaint();
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (canvas != null) {
                    getHolder().unlockCanvasAndPost(canvas);
                }
            }
        }
    }
    
    private class SurfaceCallback implements SurfaceHolder.Callback {
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mCoreView.onSize(mViewAdapter, width, height);
            mRender.requestRender();
        }
        
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mCanvasAdapter = new CanvasAdapter(null, mImageCache);
            mRender = new RenderRunnable();
        }
        
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mCanvasAdapter.delete();
            mCanvasAdapter = null;
            mRender.requestRender();
            mRender = null;
        }
    }
    
    private void renderDynamicShapes() {
        Canvas canvas = null;
        try {
            canvas = (mDynDrawCanvas != null) ? mDynDrawView.getHolder().lockCanvas() : null;
            if (canvas != null && mDynDrawCanvas.beginPaint(canvas)) {
                int shapes, gs;
                
                synchronized (mCoreView) {
                    shapes = mCoreView.acquireDynamicShapes();
                    gs = mCoreView.acquireGraphics(mViewAdapter);
                }
                
                canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
                mCoreView.dynDraw(shapes, gs, mDynDrawCanvas);
                
                mCoreView.releaseShapes(shapes);
                mCoreView.releaseGraphics(mViewAdapter, gs);
                mDynDrawCanvas.endPaint();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if (canvas != null) {
                mDynDrawView.getHolder().unlockCanvasAndPost(canvas);
            }
        }
    }
    
    private class DynDrawSurfaceCallback implements SurfaceHolder.Callback {
        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        }
        
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            mDynDrawCanvas = new CanvasAdapter(null, mImageCache);
        }
        
        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            mDynDrawCanvas.delete();
            mDynDrawCanvas = null;
        }
    }
    
    private class ViewAdapter extends BaseViewAdapter {
        
        @Override
        protected GraphView getView() {
            return SFGraphView.this;
        }
        
        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, SFGraphView.this);
        }
        
        @Override
        public void regenAll(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitBackDoc();
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            if (mRender != null) {
                mRender.requestRender();
            }
        }
        
        @Override
        public void regenAppend(int sid) {
            regenAll(true);
        }
        
        @Override
        public void redraw() {
            synchronized (mCoreView) {
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            renderDynamicShapes();
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
        if (mDynDrawView == null) {
            mDynDrawView = new SurfaceView(context);

            mDynDrawView.getHolder().setFormat(PixelFormat.TRANSPARENT);
            mDynDrawView.getHolder().addCallback(new DynDrawSurfaceCallback());
            mDynDrawView.setZOrderMediaOverlay(true);
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
    public void setBackgroundColor(int color) {
        mCoreView.setBkColor(mViewAdapter, color);
        mViewAdapter.regenAll(false);
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
        return null;
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
