//! \file GraphViewCached.java
//! \brief Android绘图视图类，使用缓存位图
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiGestureState;
import rhcad.touchvg.core.GiGestureType;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.CanvasAdapter;
import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;

//! Android绘图视图类，使用缓存位图
public class GraphViewCached extends View {
    private CanvasAdapter mCanvasAdapter;
    private ViewAdapter mViewAdapter;
    private GiCoreView mCoreView;
    private DynDrawView mDynDrawView;
    private Bitmap mCacheBitmap;
    private long mDrawnTime;
    private long mEndPaintTime;
    private long mBeginTime;

    public GraphViewCached(Context context) {
        super(context);
        mCanvasAdapter = new CanvasAdapter(this);
        mViewAdapter = new ViewAdapter();
        mCoreView = GiCoreView.createView(mViewAdapter, 0);

        DisplayMetrics dm = context.getApplicationContext().getResources().getDisplayMetrics();
        GiCoreView.setScreenDpi(dm.densityDpi);

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

    public long getDrawnTime() {
        return mDrawnTime;
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

    private void doDraw() {
        mBeginTime = android.os.SystemClock.uptimeMillis();
        invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        autoBuildCache();
        mCoreView.onSize(mViewAdapter, this.getWidth(), this.getHeight());

        if (mCanvasAdapter.beginPaint(canvas)) {
            if (mCacheBitmap != null) {
                canvas.drawBitmap(mCacheBitmap, 0, 0, null);
            } else {
                mCoreView.drawAll(mViewAdapter, mCanvasAdapter);
            }
            if (mDynDrawView == null) {
                mCoreView.dynDraw(mViewAdapter, mCanvasAdapter);
            }
            mCanvasAdapter.endPaint();
        }
        mEndPaintTime = android.os.SystemClock.uptimeMillis();
        mDrawnTime = mEndPaintTime - mBeginTime;
    }

    private void autoBuildCache() {
        if (mCacheBitmap != null
                && (mCacheBitmap.getWidth() != getWidth() || mCacheBitmap.getHeight() != getHeight())) {
            mCacheBitmap.recycle();
            mCacheBitmap = null;
        }
        if (mCacheBitmap == null) {
            mCacheBitmap = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
            final Canvas canvas = new Canvas(mCacheBitmap);

            if (mCanvasAdapter.beginPaint(canvas)) {
                canvas.drawColor(0);
                mCoreView.drawAll(mViewAdapter, mCanvasAdapter);
                mCanvasAdapter.endPaint();
            } else {
                mCacheBitmap.recycle();
                mCacheBitmap = null;
            }
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
        if (mCacheBitmap != null) {
            mCacheBitmap.recycle();
            mCacheBitmap = null;
        }
        super.onDetachedFromWindow();
    }

    private class ViewAdapter extends GiView {
        @Override
        public void regenAll(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitBackDoc(mViewAdapter);
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            if (mCacheBitmap != null) {
                mCacheBitmap.recycle();
                mCacheBitmap = null;
            }
            doDraw();
        }

        @Override
        public void regenAppend(int sid, int playh) {
            synchronized (mCoreView) {
                mCoreView.submitBackDoc(mViewAdapter);
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            if (mCacheBitmap != null) {
                final Canvas canvas = new Canvas(mCacheBitmap);
                if (mCanvasAdapter.beginPaint(canvas)) {
                    mCoreView.drawAppend(mViewAdapter, mCanvasAdapter, sid);
                    mCanvasAdapter.endPaint();
                }
            }
            doDraw();
            if (mDynDrawView != null) {
                mDynDrawView.doDraw();
            }
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
