//! \file GraphSfView.java
//! \brief Android绘图视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiGestureState;
import rhcad.touchvg.core.GiGestureType;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.CanvasAdapter;
import android.app.Activity;
import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.PorterDuff.Mode;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

//! Android绘图视图类
/**
 * 默认是底部媒体窗口类型，调用 setBackgroundColor(TRANSPARENT) 后将设置为顶部面板窗口类型
 */
public class GraphSfView extends SurfaceView {
    private CanvasAdapter mCanvasAdapter;
    private ViewAdapter mViewAdapter;
    private GiCoreView mCoreView;
    private DynDrawView mDynDrawView;
    private long mDrawnTime;
    private long mEndPaintTime;
    private long mBeginTime;
    private int mBkColor = Color.WHITE;

    public GraphSfView(Context context) {
        super(context);
        mCanvasAdapter = new CanvasAdapter(this);
        mViewAdapter = new ViewAdapter();
        mCoreView = GiCoreView.createView(mViewAdapter, 0);

        DisplayMetrics dm = context.getApplicationContext().getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi);

        getHolder().addCallback(new SurfaceCallback());

        this.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                if (event.getAction() == MotionEvent.ACTION_DOWN) {
                    mCoreView.onGesture(mViewAdapter, GiGestureType.kGiGesturePan,
                            GiGestureState.kGiGestureBegan, event.getX(), event.getY());
                } else if (event.getAction() == MotionEvent.ACTION_UP) {
                    mCoreView.onGesture(mViewAdapter, GiGestureType.kGiGesturePan,
                            GiGestureState.kGiGestureEnded, event.getX(), event.getY());
                    showTime();
                } else if (mDynDrawView != null
                        && event.getEventTime() > mDynDrawView.getEndPaintTime()) {
                    mCoreView.onGesture(mViewAdapter, GiGestureType.kGiGesturePan,
                            GiGestureState.kGiGestureMoved, event.getX(), event.getY());
                    showTime();
                } else if (mDynDrawView == null && event.getEventTime() > mEndPaintTime) {
                    mCoreView.onGesture(mViewAdapter, GiGestureType.kGiGesturePan,
                            GiGestureState.kGiGestureMoved, event.getX(), event.getY());
                    showTime();
                }
                return true;
            }
        });
    }

    public GiCoreView coreView() {
        return mCoreView;
    }

    public void setDynDrawView(DynDrawView view) {
        mDynDrawView = view;
        if (mDynDrawView != null) {
            mDynDrawView.setCoreView(mViewAdapter, mCoreView);
        }
    }

    public void setBackgroundColor(int color) {
        mBkColor = color;
        if (mBkColor == Color.TRANSPARENT) {
            setZOrderOnTop(true);
            getHolder().setFormat(PixelFormat.TRANSPARENT);
        }
    }

    public long getDrawnTime() {
        return mDrawnTime;
    }

    public boolean isDrawing() {
        return mCanvasAdapter != null && mCanvasAdapter.isDrawing();
    }

    private void showTime() {
        Activity activity = (Activity) this.getContext();
        String title = activity.getTitle().toString();
        int pos = title.indexOf(" - ");
        if (pos >= 0) {
            title = title.substring(0, pos);
        }
        String dyntext = mDynDrawView != null ? (mDynDrawView.getDrawnTime() + "/") : "";
        activity.setTitle(title + " - " + dyntext + mDrawnTime + " ms");
    }

    public void drawShapes(Canvas canvas) {
        if (mCanvasAdapter.beginPaint(canvas)) {
            if (getBackground() == null) {
                if (mBkColor == Color.TRANSPARENT)
                    canvas.drawColor(mBkColor, Mode.CLEAR);
                else
                    canvas.drawColor(mBkColor);
            } else {
                getBackground().draw(canvas);
            }
            mCoreView.drawAll(mViewAdapter, mCanvasAdapter);
            if (mDynDrawView == null) {
                mCoreView.dynDraw(mViewAdapter, mCanvasAdapter);
            }
            mCanvasAdapter.endPaint();
        }
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mDynDrawView != null) {
            mDynDrawView.setCoreView(null, null);
            mDynDrawView = null;
        }
        if (mViewAdapter != null) {
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
        super.onDetachedFromWindow();
    }

    private void doDraw() {
        mBeginTime = android.os.SystemClock.uptimeMillis();
        new Thread(new DrawThread()).start();
    }

    private class SurfaceCallback implements SurfaceHolder.Callback {
        public void surfaceCreated(SurfaceHolder holder) {
            doDraw();
        }

        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            mCoreView.onSize(mViewAdapter, width, height);
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
        }
    }

    private class DrawThread implements Runnable {

        public void run() {
            Canvas canvas = null;

            try {
                mEndPaintTime = Integer.MAX_VALUE;
                canvas = getHolder().lockCanvas();
                if (canvas != null) {
                    drawShapes(canvas);
                }
            } catch (Exception e) {
                e.printStackTrace();
            } finally {
                if (canvas != null) {
                    getHolder().unlockCanvasAndPost(canvas);
                }
            }
            mEndPaintTime = android.os.SystemClock.uptimeMillis();
            mDrawnTime = mEndPaintTime - mBeginTime;
        }
    }

    private class ViewAdapter extends GiView {
        @Override
        public void regenAll(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitBackDoc(mViewAdapter);
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            doDraw();
            if (mDynDrawView != null) {
                mDynDrawView.doDraw();
            }
        }

        @Override
        public void regenAppend(int sid, int playh) {
            regenAll(true);
        }

        @Override
        public void redraw(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitDynamicShapes(mViewAdapter);
            }
            if (mDynDrawView != null) {
                mDynDrawView.doDraw();
            } else {
                doDraw();
            }
        }
    }
}
