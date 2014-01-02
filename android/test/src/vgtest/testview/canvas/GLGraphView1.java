// Copyright (c) 2014, https://github.com/rhcad/touchvg

package vgtest.testview.canvas;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import touchvg.core.GiCanvas;
import touchvg.core.TestCanvas;
import touchvg.core.TouchGLView;
import android.app.Activity;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.View;

public class GLGraphView1 extends GLSurfaceView {
    private TouchGLView mGlView;
    protected int mCreateFlags;
    private float lastX = 50;
    private float lastY = 50;
    private float mPhase = 0;
    
    static {
        System.loadLibrary("touchvg");
    }

    public GLGraphView1(Context context) {
        super(context);
        setEGLContextClientVersion(2);
        setRenderer(new GLRenderer());
        
        mCreateFlags = ((Activity) context).getIntent().getExtras().getInt("flags");
        
        this.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                lastX = event.getX();
                lastY = event.getY();
                return true;
            }
        });
    }
    
    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        mGlView.delete();
    }
    
    private void dynDraw(GiCanvas canvas) {
        mPhase += 1;
        canvas.setPen(0, 0, 1, mPhase, 0);
        canvas.setBrush(0x88005500, 0);
        canvas.drawEllipse(lastX - 50, lastY - 50, 100, 100, true, true);
    }

    private class GLRenderer implements GLSurfaceView.Renderer {

    public void onDrawFrame(GL10 gl) {
        if (mGlView != null) {
            gl.glClear(GL10.GL_COLOR_BUFFER_BIT);
            
            mGlView.prepareToDraw();
            
            boolean testDynCurves = (mCreateFlags & 0x400) != 0;
            TestCanvas.test(mGlView.beginPaint(!testDynCurves), mCreateFlags);
            mGlView.endPaint();
            dynDraw(mGlView.beginPaint(false));
            mGlView.endPaint();
        }
    }
    
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        if (mGlView == null) {
            mGlView = new TouchGLView(width, height);
        } else {
            mGlView.resize(width, height);
        }
        gl.glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        gl.glViewport(0, 0, width, height);
    }
    
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    }
    }
}
