// Copyright (c) 2014, https://github.com/rhcad/touchvg

package touchgl.view;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import touchgl.core.GiCoreView;
import touchgl.core.GiView;
import touchgl.core.TouchGLView;
import android.content.Context;
import android.opengl.GLSurfaceView;

public class PaintView extends GLSurfaceView {
    private TouchGLView mGlView;
    private GiCoreView mCoreView;
    private ViewAdapter mViewAdapter;
    
    static {
        System.loadLibrary("touchgl");
    }
    
    public PaintView(Context context) {
        super(context);
        
        setEGLContextClientVersion(2); // OpenGL ES 2.0
        setRenderMode(RENDERMODE_WHEN_DIRTY);
        setRenderer(new GLRenderer());
        
        mCoreView = new GiCoreView();
        mViewAdapter = new ViewAdapter();
        mCoreView.createView(mViewAdapter);
    }
    
    public GiCoreView getCoreView() {
        return mCoreView;
    }
    
    public ViewAdapter viewAdapter() {
        return mViewAdapter;
    }
    
    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mCoreView.destoryView(mViewAdapter);
        mViewAdapter.delete();
        mCoreView.delete();
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
        }
        
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        }
    }
    
    private class ViewAdapter extends GiView {
        
        @Override
        public void regenAll(boolean changed) {
            mGlView.clear();
            requestRender();
        }
        
        @Override
        public void regenAppend(int sid) {
            requestRender();
        }
        
        @Override
        public void redraw() {
            requestRender();
        }
    }
}
