// Copyright (c) 2014, https://github.com/rhcad/touchvg

package touchvg.view;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import touchvg.core.GiCoreView;
import touchvg.core.GiView;
import touchvg.core.TouchGLView;
import touchvg.view.internal.BaseViewAdapter;
import touchvg.view.internal.ContextAction;
import touchvg.view.internal.GestureListener;
import touchvg.view.internal.ImageCache;
import touchvg.view.internal.ResourceUtil;
import touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;

public class GLGraphView extends GLSurfaceView implements GraphView {
    private TouchGLView mGlView;
    private GiCoreView mCoreView;
    private GLViewAdapter mViewAdapter = new GLViewAdapter();
    private ImageCache mImageCache = new ImageCache();
    private GestureDetector mGestureDetector;
    private GestureListener mGestureListener;
    private boolean mGestureEnable = true;
    
    static {
        System.loadLibrary("touchvg");
    }
    
    public GLGraphView(Context context) {
        super(context);
        mCoreView = new GiCoreView();
        mCoreView.createView(mViewAdapter);
        initView(context);
        if (ViewUtil.activeView == null) {
            ViewUtil.activeView = this;
        }
    }
    
    public GLGraphView(Context context, GraphView mainView) {
        super(context);
        mCoreView = new GiCoreView(mainView.coreView());
        mCoreView.createMagnifierView(mViewAdapter, mainView.viewAdapter());
        initView(context);
    }
    
    private void initView(Context context) {
        setEGLContextClientVersion(2);          // OpenGL ES 2.0
        setRenderer(new GLRenderer());
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        
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
        mGlView.delete();
    }
    
    private class GLRenderer implements GLSurfaceView.Renderer {
        
        @Override
        public void onDrawFrame(GL10 gl) {
            if (mGlView != null) {
                gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
                
                mGlView.prepareToDraw();
                
                int doc, shapes, gs;
                
                synchronized (mCoreView) {
                    doc = mCoreView.acquireFrontDoc();
                    shapes = mCoreView.acquireDynamicShapes();
                    gs = mCoreView.acquireGraphics(mViewAdapter);
                }
                
                mCoreView.drawAll(doc, gs, mGlView.beginPaint(true));
                mGlView.endPaint();
                mCoreView.dynDraw(shapes, gs, mGlView.beginPaint(false));
                mGlView.endPaint();
                
                mCoreView.releaseDoc(doc);
                mCoreView.releaseShapes(shapes);
                mCoreView.releaseGraphics(mViewAdapter, gs);
            }
        }
        
        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            if (mGlView == null) {
                mGlView = new TouchGLView(width, height);
            } else {
                mGlView.resize(width, height);
            }
            gl.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            gl.glViewport(0, 0, width, height);
            
            mCoreView.onSize(mViewAdapter, width, height);
        }
        
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        }
    }
    
    private class GLViewAdapter extends BaseViewAdapter {
        
        @Override
        protected GraphView getView() {
            return GLGraphView.this;
        }
        
        @Override
        protected ContextAction createContextAction() {
            return new ContextAction(mCoreView, GLGraphView.this);
        }
        
        @Override
        public void regenAll(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitBackDoc();
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            if (mGlView != null) {
                mGlView.clear();
                requestRender();
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
            requestRender();
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
    public ImageCache getImageCache() {
        return mImageCache;
    }

    @Override
    public void clearCachedData() {
        mCoreView.clearCachedData();
        mGlView.clear();
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
