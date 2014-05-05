//! \file ContextAction.java
//! \brief Android绘图视图类

package rhcad.touchvg.view.internal;

import rhcad.touchvg.core.Floats;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.Ints;
import android.content.Context;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

//! 绘图视图的上下文操作布局类
public class ContextAction {
    private static final String TAG = "touchvg";
    private static final int EXTRA_ACTION_BEGIN = 40;
    private GiCoreView mCoreView;
    private View mView;
    private RelativeLayout mButtonLayout;
    private static int[] mImageIDs;
    private static int[] mExtImageIDs;
    private static int mCaptionsID;
    private static String[] mCaptions;
    private static float mDensity;

    public ContextAction(GiCoreView core, View view) {
        this.mCoreView = core;
        this.mView = view;
        mDensity = view.getContext().getApplicationContext().getResources().getDisplayMetrics().density;
    }

    protected void finalize() {
        Log.d(TAG, "ContextAction finalize");
    }

    public void release() {
        this.mCoreView = null;
        this.mView = null;
    }

    public boolean isVisible() {
        return mButtonLayout != null;
    }

    public boolean showActions(Context context, Ints actions, Floats buttonXY) {
        removeButtonLayout();

        int n = actions != null ? actions.count() : 0;
        if (n == 0) {
            return true;
        }

        mButtonLayout = new RelativeLayout(context);

        for (int i = 0; i < n; i++) {
            int xc = (int) buttonXY.get(2 * i);
            int yc = (int) buttonXY.get(2 * i + 1);
            addContextAction(n, i, actions.get(i), xc, yc, context, mButtonLayout);
        }

        final ViewGroup f = (ViewGroup) mView.getParent();
        if (mButtonLayout != null) {
            f.addView(mButtonLayout);
        }

        return isVisible();
    }

    private boolean addContextAction(int n, int index, int action, int xc, int yc, Context context,
            RelativeLayout layout) {
        final Button btn = new Button(context);
        boolean hasImage = setButtonBackground(context, btn, action);

        btn.setId(action);
        btn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                int action = v.getId();
                removeButtonLayout();
                mCoreView.doContextAction(action);
            }
        });
        btn.setOnTouchListener(new OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                mView.getParent().requestDisallowInterceptTouchEvent(
                        event.getActionMasked() != MotionEvent.ACTION_UP);
                return false;
            }
        });

        if (layout != null) {
            final LayoutParams params = new LayoutParams((hasImage ? Math.round(mDensity * 32)
                    : LayoutParams.WRAP_CONTENT), (hasImage ? Math.round(mDensity * 32)
                    : LayoutParams.WRAP_CONTENT));
            params.leftMargin = xc - Math.round(mDensity * (hasImage ? 32 : 64)) / 2;
            params.topMargin = yc - Math.round(mDensity * (hasImage ? 36 : 40)) / 2;

            layout.addView(btn, params);
        }

        return true;
    }

    public void removeButtonLayout() {
        if (mButtonLayout != null) {
            final ViewGroup f = (ViewGroup) mView.getParent();
            f.removeView(mButtonLayout);
            mButtonLayout = null;
        }
    }

    private boolean setButtonBackground(Context context, Button button, int action) {
        if (mImageIDs != null && action < mImageIDs.length && action > 0 && mImageIDs[action] != 0) {
            button.setBackgroundResource(mImageIDs[action]);
            return true;
        }
        if (mExtImageIDs != null && action - EXTRA_ACTION_BEGIN >= 0
                && action - EXTRA_ACTION_BEGIN < mExtImageIDs.length
                && mExtImageIDs[action - EXTRA_ACTION_BEGIN] != 0) {
            button.setBackgroundResource(mExtImageIDs[action - EXTRA_ACTION_BEGIN]);
            return true;
        }
        if (mCaptions == null && mCaptionsID != 0) {
            mCaptions = context.getResources().getStringArray(mCaptionsID);
        }
        if (mCaptions != null && action > 0 && action < mCaptions.length) {
            button.setText(mCaptions[action]);
        }
        return false;
    }

    public static int[] getButtonImages() {
        return mImageIDs;
    }

    public static void setButtonImages(int[] ids) {
        mImageIDs = ids;
    }

    public static void setExtraButtonImages(int[] ids) {
        mExtImageIDs = ids;
    }

    public static void setButtonCaptionsID(int captionsID) {
        mCaptionsID = captionsID;
    }
}
