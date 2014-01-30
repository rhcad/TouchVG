//! \file GestureListener.java
//! \brief Android绘图手势识别类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.GiGestureState;
import rhcad.touchvg.core.GiGestureType;
import rhcad.touchvg.core.GiView;
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
    private int mXYCount = 0;                    // mPoints值个数
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
            mCoreView.setCommand(null);
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
        mXYCount = 0;
        if (e.getPointerCount() == 1) {
            mLastX = e.getX(0);
            mLastY = e.getY(0);
            mPoints[mXYCount++] = mLastX;
            mPoints[mXYCount++] = mLastY;
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
        float x1 = e.getPointerCount() > 0 ? e.getX(0) : 0;
        float y1 = e.getPointerCount() > 0 ? e.getY(0) : 0;
        float x2 = e.getPointerCount() > 1 ? e.getX(1) : x1;
        float y2 = e.getPointerCount() > 1 ? e.getY(1) : y1;

        onTouch_(v, e.getActionMasked(), e.getPointerCount(), x1, y1, x2, y2);
        return false;   // to call GestureDetector.onTouchEvent
    }
    
    public boolean onTouch(View v, int action, float x, float y) {
        if (action == MotionEvent.ACTION_DOWN) {
            mMoving = READY_MOVE;
            mXYCount = 0;
            mLastX = x;
            mLastY = y;
            mPoints[mXYCount++] = mLastX;
            mPoints[mXYCount++] = mLastY;
            mLastX2 = mLastX;
            mLastY2 = mLastY;
        }
        return onTouch_(v, action, 1, x, y, x, y);
    }
    
    private boolean onTouch_(View v, int action, int count, 
            float x1, float y1, float x2, float y2) {
        if (action == MotionEvent.ACTION_UP
            || action == MotionEvent.ACTION_CANCEL) {
            v.getParent().requestDisallowInterceptTouchEvent(false);
        }
        else {  // 按下后不允许父视图拦截触摸事件
            v.getParent().requestDisallowInterceptTouchEvent(true);
        }
        
        boolean ret = false;
        
        switch (action) {
            case MotionEvent.ACTION_DOWN:
                ret = true;
                break;
            case MotionEvent.ACTION_MOVE:
                ret = onTouchMoved(count, x1, y1, x2, y2);
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_CANCEL:
                ret = onTouchEnded(action == MotionEvent.ACTION_UP, x1, y1, x2, y2);
                break;
        }
        
        return ret;
    }
    
    private boolean onTouchMoved(int fingerCount, float x1, float y1, float x2, float y2) {
        if (fingerCount == 1 && Math.abs(mLastX - x1) < 1 && Math.abs(mLastY - y1) < 1) {
            return false;
        }
        if (fingerCount == 2 && Math.abs(mLastX - x1) < 1 && Math.abs(mLastY - y1) < 1
            && Math.abs(mLastX2 - x2) < 1 && Math.abs(mLastY2 - y2) < 1) {
            return false;
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
            else if (fingerCount == 1 && mXYCount > 0) {
                mMoving = applyPendingPoints() ? MOVING : END_MOVE;
            }
            else {
                mMoving = (onMoved(GiGestureState.kGiGesturePossible, mFingerCount,
                                   x1, y1, x2, y2, false)
                           && onMoved(GiGestureState.kGiGestureBegan, mFingerCount,
                                      x1, y1, x2, y2, false)) ? MOVING : END_MOVE;
            }
            mXYCount = 0;
        }
        else if (mMoving == STOPPED && fingerCount == 1
                 && mXYCount > 0 && mXYCount + 1 < XY_COUNT) {
            mPoints[mXYCount++] = x1;
            mPoints[mXYCount++] = y1;
        }
        else if (mMoving == PRESS_MOVING) {
            mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePress,
                                GiGestureState.kGiGestureMoved, x1, y1);
        }
        if (mMoving != MOVING) {
            return false;
        }
        
        boolean ret = false;
        
        if (mFingerCount != fingerCount) {
            if (onMoved(GiGestureState.kGiGesturePossible, fingerCount, x1, y1, x2, y2, true)) {
                if (mFingerCount == 1) {        // 单指变为双指
                    ret = onMoved(GiGestureState.kGiGestureEnded,
                            mFingerCount, mLastX, mLastY, 0, 0, true);
                }
                else {
                    ret = onMoved(GiGestureState.kGiGestureEnded,
                            mFingerCount, x1, y1, x2, y2, true);
                }
                mFingerCount = fingerCount;
                ret = onMoved(GiGestureState.kGiGestureBegan, mFingerCount, x1, y1, x2, y2, true);
            }
        }
        else {
            ret = onMoved(GiGestureState.kGiGestureMoved, mFingerCount, x1, y1, x2, y2, false);
            mLastX = x1;
            mLastY = y1;
        }
        
        return ret;
    }
    
    private boolean onTouchEnded(boolean submit, float x1, float y1, float x2, float y2) {
        if (submit && mXYCount > 2 && applyPendingPoints()) {
            mMoving = MOVING;
        }
        boolean ret = false;
        if (mMoving == MOVING) {
            ret = onMoved(submit ? GiGestureState.kGiGestureEnded
                    : GiGestureState.kGiGestureCancel, mFingerCount, x1, y1, x2, y2, false);
        }
        else if (mMoving == PRESS_MOVING) {
            ret = mCoreView.onGesture(mAdapter, GiGestureType.kGiGesturePress,
                                submit ? GiGestureState.kGiGestureEnded : GiGestureState.kGiGestureCancel, 0, 0);
        }
        mMoving = STOPPED;
        mFingerCount = 0;
        
        return ret;
    }
    
    private boolean applyPendingPoints() {
        boolean ret = (onMoved(GiGestureState.kGiGesturePossible, mFingerCount,
                               mPoints[0], mPoints[1], 0, 0, false)
                       && onMoved(GiGestureState.kGiGestureBegan, mFingerCount,
                                  mPoints[0], mPoints[1], 0, 0, false));
        for (int i = 2; i + 1 < mXYCount && ret; i += 2) {
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
        if (mXYCount > 1 && mCoreView.onGesture(mAdapter,
            GiGestureType.kGiGesturePress, GiGestureState.kGiGestureBegan, e.getX(), e.getY())) {
            mXYCount = 0;
            mMoving = PRESS_MOVING;
        }
        else if (mMoving == STOPPED) {  // onDown 后还未移动
            mMoving = READY_MOVE;       // onTouch 中将开始移动
        }
    }
    
    @Override
    public boolean onSingleTapConfirmed(MotionEvent e) {
        return mXYCount > 1 && onTap(mPoints[0], mPoints[1]);
    }
    
    //! 传递单指轻击事件，可用于拖放操作
    public boolean onTap(float x, float y) {
        return mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureTap, GiGestureState.kGiGesturePossible, x, y)
                && mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureTap, GiGestureState.kGiGestureEnded, x, y);
    }
    
    @Override
    public boolean onDoubleTap(MotionEvent e) {
        boolean ret = mXYCount > 1;
        if (ret) {
            ret = mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureDblTap,
                                      GiGestureState.kGiGesturePossible, mPoints[0], mPoints[1])
            && mCoreView.onGesture(mAdapter, GiGestureType.kGiGestureDblTap,
                                   GiGestureState.kGiGestureEnded, e.getX(), e.getY());
            mXYCount = 0;
        }
        return ret;
    }
}
