//! \file SurfaceView3.java
//! \brief 基于SurfaceView在线程中不断绘图的测试视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.canvas;

import rhcad.touchvg.core.TestCanvas;
import rhcad.touchvg.view.CanvasAdapter;
import vgtest.app.R;
import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.os.SystemClock;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

//! 基于SurfaceView在线程中不断绘图的测试视图类
public class SurfaceView3 extends SurfaceView {
    protected CanvasAdapter mCanvas;
    protected int mCreateFlags;
    private long mDrawnTime;
    private long mDrawTimes = 0;
    private float lastX = 50;
    private float lastY = 50;
    private float mPhase = 0;

    public SurfaceView3(Context context) {
        super(context);
        mCanvas = new CanvasAdapter(this);
        mCreateFlags = ((Activity) context).getIntent().getExtras().getInt("flags");
        initCanvas();
        getHolder().addCallback(new SurfaceCallback());

        setZOrderOnTop(true);
        if ((mCreateFlags & 0x1000) == 0) {
            getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }

        this.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                showTime(mDrawnTime);
                return true;
            }
        });
    }

    private void initCanvas() {
        CanvasAdapter.setHandleImageIDs(new int[] { R.drawable.vgdot1, R.drawable.vgdot2,
                R.drawable.vgdot3, R.drawable.vg_lock, R.drawable.vg_unlock });
    }

    @Override
    public void setBackgroundColor(int color) {
        mCanvas.setBackgroundColor(color); // 视图仍然是透明色
    }

    protected void showTime(long ms) {
        Activity activity = (Activity) this.getContext();
        String title = activity.getTitle().toString();
        int pos = title.indexOf(' ');
        if (pos >= 0) {
            title = title.substring(0, pos);
        }
        activity.setTitle(title + " - " + Long.toString(ms) + " ms, " + Long.toString(mDrawTimes));
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mCanvas != null) {
            mCanvas.delete();
            mCanvas = null;
        }
        super.onDetachedFromWindow();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        long ms = SystemClock.currentThreadTimeMillis();
        if (mCanvas.beginPaint(canvas)) {
            if ((mCreateFlags & 0x1000) == 0) {
                canvas.drawColor(Color.TRANSPARENT, Mode.CLEAR);
            } else {
                canvas.drawColor(Color.WHITE);
            }
            TestCanvas.test(mCanvas, mCreateFlags);
            dynDraw(mCanvas);
            mCanvas.endPaint();
        }
        mDrawnTime = SystemClock.currentThreadTimeMillis() - ms;
        mDrawTimes++;
    }

    private void dynDraw(CanvasAdapter canvas) {
        mPhase += 1;
        canvas.setPen(0, 0, 1, mPhase, 0);
        canvas.setBrush(0x22005500, 0);
        mCanvas.drawEllipse(lastX - 50, lastY - 50, 100, 100, true, true);
    }

    class SurfaceCallback implements SurfaceHolder.Callback {
        public void surfaceCreated(SurfaceHolder holder) {
            new Thread(new DrawThread()).start();
            // 在视图刚创建就启动线程会因lockCanvas失败而延迟显示，在surfaceCreated中启动更快
        }

        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
        }
    }

    class DrawThread implements Runnable {
        public void run() {
            while (mCanvas != null) {
                if (!mCanvas.isDrawing()) {
                    Canvas canvas = null;
                    try {
                        canvas = getHolder().lockCanvas();
                        if (canvas != null) {
                            draw(canvas);
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
        }
    }
}
