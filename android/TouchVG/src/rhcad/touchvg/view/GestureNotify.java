//! \file GestureNotify.java
//! \brief Define GestureNotify interface to hook drawing gestures.
// Copyright (c) 2014, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import android.view.MotionEvent;

//! Interface to hook drawing gestures.
public interface GestureNotify {
    public boolean onPreLongPress(MotionEvent e);
    public boolean onPreSingleTap(MotionEvent e);
    public boolean onPreDoubleTap(MotionEvent e);
}
