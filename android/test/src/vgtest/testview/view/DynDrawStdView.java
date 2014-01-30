//! \file DynDrawStdView.java
//! \brief 基于View的动态绘图视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.view;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiView;
import rhcad.touchvg.view.CanvasAdapter;
import android.content.Context;
import android.graphics.Canvas;
import android.view.View;

//! 基于View的动态绘图视图类
public class DynDrawStdView extends View implements DynDrawView {
    private CanvasAdapter mCanvasAdapter;
    private GiView mViewAdapter;
    private GiCoreView mCoreView;
    private long mDrawnTime;
    private long mEndPaintTime;
    private long mBeginTime;

    public DynDrawStdView(Context context) {
        super(context);
        mCanvasAdapter = new CanvasAdapter(this);
    }

    public void setCoreView(GiView viewAdapter, GiCoreView coreView) {
        mViewAdapter = viewAdapter;
        mCoreView = coreView;
    }

    public long getDrawnTime() {
        return mDrawnTime;
    }

    public long getEndPaintTime() {
        return mEndPaintTime;
    }

    public boolean isDrawing() {
        return mCanvasAdapter != null && mCanvasAdapter.isDrawing();
    }

    public void doDraw() {
        mBeginTime = android.os.SystemClock.uptimeMillis();
        this.invalidate();
    }

    @Override
    protected void onDraw(Canvas canvas) {
        if (mCanvasAdapter.beginPaint(canvas)) {
            mCoreView.dynDraw(mViewAdapter, mCanvasAdapter);
            mCanvasAdapter.endPaint();
        }
        mEndPaintTime = android.os.SystemClock.uptimeMillis();
        mDrawnTime = mEndPaintTime - mBeginTime;
    }

    @Override
    protected void onDetachedFromWindow() {
        if (mCanvasAdapter != null) {
            mCanvasAdapter.delete();
            mCanvasAdapter = null;
        }
        mCoreView = null;
        mViewAdapter = null;
        super.onDetachedFromWindow();
    }
}
