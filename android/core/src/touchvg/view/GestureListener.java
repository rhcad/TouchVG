//! \file GestureListener.java
//! \brief Android绘图手势识别类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import touchvg.core.GiCoreView;
import touchvg.core.GiGestureState;
import touchvg.core.GiGestureType;
import touchvg.core.GiView;
import android.view.GestureDetector.SimpleOnGestureListener;
import android.view.MotionEvent;
import android.view.View;

//! Android绘图手势识别类
public class GestureListener extends SimpleOnGestureListener {
    private static final int STOPPED = 0;
    private static final int READY_MOVE = 1;
    private static final int MOVING = 2;
    private static final int END_MOVE = 3;
    private static final int PRESS_MOVING = 4;
    private static final int XY_COUNT = 20;
    private GiCoreView mCoreView;                   // 内核视图分发器
    private GiView mAdapter;                        // 视图回调适配器
    private int mMoving = STOPPED;                  // 移动状态
    private int mFingerCount;                       // 上一次的触摸点数
    private int mPointCount = 0;                    // mPoints值个数
    private float[] mPoints = new float[XY_COUNT];  // 待分发的移动轨迹
    private float mLastX;
    private float mLastY;
    private float mLastX2;
    private float mLastY2;
    
    public GestureListener(GiCoreView coreView, GiView adapter) {
        mCoreView = coreView;
        mAdapter = adapter;
    }
    
    public void release() {
        mCoreView = null;
        mAdapter = null;
    }
    
    public void setGestureEnable(boolean enabled) {
        if (!enabled) {
            cancelDragging();
            mCoreView.setCommand(mAdapter, null);
        }
    }
    
    public void cancelDragging() {
        if (mMoving == MOVING) {
            mMoving = STOPPED;
            onMoved(GiGestureState.kGiGestureCancel, mFingerCount, 0, 0, 0, 0, false);
        }
        else if (mMoving == PRESS_MOVING) {
            mMoving = STOPPED;
            mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePress,
                                GiGestureState.kGiGestureCancel, 0, 0);
        }
    }
    
    @Override
    public boolean onDown(MotionEvent e) {
        mMoving = STOPPED;
        mPointCount = 0;
        if (e.getPointerCount() == 1) {
            mLastX = e.getX(0);
            mLastY = e.getY(0);
            mPoints[mPointCount++] = mLastX;
            mPoints[mPointCount++] = mLastY;
        }
        mLastX2 = mLastX;
        mLastY2 = mLastY;
        
        return true;
    }
    
    @Override
    public boolean onScroll(MotionEvent downEv, MotionEvent e, float dx, float dy) {
        if (mMoving == STOPPED) {
            mMoving = READY_MOVE;
        }
        return mMoving == READY_MOVE || mMoving == MOVING;
    }
    
    public boolean onTouch(View v, MotionEvent e) {
        final int action = e.getActionMasked();
        final float x1 = e.getPointerCount() > 0 ? e.getX(0) : 0;
        final float y1 = e.getPointerCount() > 0 ? e.getY(0) : 0;
        final float x2 = e.getPointerCount() > 1 ? e.getX(1) : x1;
        final float y2 = e.getPointerCount() > 1 ? e.getY(1) : y1;
        
        // 按下后不允许父视图拦截触摸事件，松开后允许
        if (action == MotionEvent.ACTION_UP
            || action == MotionEvent.ACTION_CANCEL) {
            v.getParent().requestDisallowInterceptTouchEvent(false);
        }
        else {
            v.getParent().requestDisallowInterceptTouchEvent(true);
        }
        
        switch (action) {
            case MotionEvent.ACTION_MOVE:
                onTouchMoved(e.getPointerCount(), x1, y1, x2, y2);
                break;
                
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                onTouchEnded(e.getAction() == MotionEvent.ACTION_UP, x1, y1, x2, y2);
                break;
        }
        
        return false;   // to call GestureDetector.onTouchEvent
    }
    
    private void onTouchMoved(int fingerCount, float x1, float y1, float x2, float y2) {
        if (fingerCount == 1 && Math.abs(mLastX - x1) < 1 && Math.abs(mLastY - y1) < 1) {
            return;
        }
        if (fingerCount == 2 && Math.abs(mLastX - x1) < 1 && Math.abs(mLastY - y1) < 1
            && Math.abs(mLastX2 - x2) < 1 && Math.abs(mLastY2 - y2) < 1) {
            return;
        }
        if (mMoving == READY_MOVE || (mMoving == STOPPED && fingerCount == 2)) {
            mFingerCount = fingerCount;
            mLastX = x1;
            mLastY = y1;
            mLastX2 = x2;
            mLastY2 = y2;
            
            if (mMoving == STOPPED) {
                mMoving = READY_MOVE;
            }
            else if (fingerCount == 1 && mPointCount > 0) {
                mMoving = applyPendingPoints() ? MOVING : END_MOVE;
            }
            else {
                mMoving = onMoved(GiGestureState.kGiGestureBegan, mFingerCount,
                                  x1, y1, x2, y2, false) ? MOVING : END_MOVE;
            }
            mPointCount = 0;
        }
        else if (mMoving == STOPPED && fingerCount == 1
                 && mPointCount > 0 && mPointCount + 1 < XY_COUNT) {
            mPoints[mPointCount++] = x1;
            mPoints[mPointCount++] = y1;
        }
        else if (mMoving == PRESS_MOVING) {
            mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePress,
                                GiGestureState.kGiGestureMoved, x1, y1);
        }
        if (mMoving != MOVING) {
            return;
        }
        
        if (mFingerCount != fingerCount) {
            if (onMoved(GiGestureState.kGiGesturePossible, fingerCount, x1, y1, x2, y2, true)) {
                if (mFingerCount == 1) {        // 单指变为双指
                    onMoved(GiGestureState.kGiGestureEnded, mFingerCount, mLastX, mLastY, 0, 0, true);
                }
                else {
                    onMoved(GiGestureState.kGiGestureEnded, mFingerCount, x1, y1, x2, y2, true);
                }
                mFingerCount = fingerCount;
                onMoved(GiGestureState.kGiGestureBegan, mFingerCount, x1, y1, x2, y2, true);
            }
        }
        else {
            onMoved(GiGestureState.kGiGestureMoved, mFingerCount, x1, y1, x2, y2, false);
            mLastX = x1;
            mLastY = y1;
        }
    }
    
    private void onTouchEnded(boolean submit, float x1, float y1, float x2, float y2) {
        if (submit && mPointCount > 2 && applyPendingPoints()) {
            mMoving = MOVING;
        }
        if (mMoving == MOVING) {
            onMoved(submit ? GiGestureState.kGiGestureEnded
                    : GiGestureState.kGiGestureCancel, mFingerCount, x1, y1, x2, y2, false);
        }
        else if (mMoving == PRESS_MOVING) {
            mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePress,
                                submit ? GiGestureState.kGiGestureEnded : GiGestureState.kGiGestureCancel, 0, 0);
        }
        mMoving = STOPPED;
        mFingerCount = 0;
    }
    
    private boolean applyPendingPoints() {
        boolean ret = onMoved(GiGestureState.kGiGestureBegan, mFingerCount,
                              mPoints[0], mPoints[1], 0, 0, false);
        for (int i = 2; i + 1 < mPointCount && ret; i += 2) {
            onMoved(GiGestureState.kGiGestureMoved, mFingerCount,
                    mPoints[i], mPoints[i + 1], 0, 0, false);
        }
        return ret;
    }
    
    private boolean onMoved(GiGestureState state, int fingerCount,
                            float x1, float y1, float x2, float y2, boolean s) {
        return fingerCount > 1 ? mCoreView.twoFingersMove(mAdapter, state, x1, y1, x2, y2, s)
        : mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePan, state, x1, y1, s);
    }
    
    @Override
    public void onLongPress(MotionEvent e) {
        if (mPointCount > 1 && mCoreView.onGesture(mAdapter,
            GiGestureType.kGiGesturePress, GiGestureState.kGiGestureBegan, e.getX(), e.getY())) {
            mPointCount = 0;
            mMoving = PRESS_MOVING;
        }
        else if (mMoving == STOPPED) {  // onDown 后还未移动
            mMoving = READY_MOVE;       // onTouch 中将开始移动
        }
    }
    
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        boolean ret = mPointCount > 1;
        if (ret) {
            ret = mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureTap,
                                      GiGestureState.kGiGesturePossible, mPoints[0], mPoints[1])
            && mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureTap,
                                   GiGestureState.kGiGestureEnded, e.getX(), e.getY());
            mPointCount = 0;
        }
        return ret;
    }
    
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        boolean ret = mPointCount > 1;
        if (ret) {
            ret = mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureDblTap,
                                      GiGestureState.kGiGesturePossible, mPoints[0], mPoints[1])
            && mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureDblTap,
                                   GiGestureState.kGiGestureEnded, e.getX(), e.getY());
            mPointCount = 0;
        }
        return ret;
    }
}
