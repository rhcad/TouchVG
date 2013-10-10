//! \file ContextAction.java
//! \brief Android绘图视图类

package touchvg.view;

import touchvg.core.Floats;
import touchvg.core.GiCoreView;
import touchvg.core.Ints;
import android.content.Context;
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
    
    private GiCoreView mCoreView;
    private GraphView mView;
    private Context mContext;
    private RelativeLayout mButtonLayout;
    private static int[] mImageIDs;
    private static int[] mExtImageIDs;
    private static int mCaptionsID;
    private static String[] mCaptions;
    
    public ContextAction(Context context, GiCoreView core, GraphView view) {
        this.mContext = context;
        this.mCoreView = core;
        this.mView = view;
    }
    
    public void release() {
        this.mContext = null;
        this.mCoreView = null;
        this.mView = null;
    }
    
    public boolean isVisible() {
        return mButtonLayout != null;
    }
    
    public boolean showActions(Ints actions, Floats buttonXY) {
        removeButtonLayout();
        
        int n = actions.count();
        if (n == 0) {
            return true;
        }
        
        mButtonLayout = new RelativeLayout(mContext);
        
        for (int i = 0; i < n; i++) {
            int xc = (int)buttonXY.get(2 * i);
            int yc = (int)buttonXY.get(2 * i + 1);
            addContextAction(n, i, actions.get(i), xc, yc, mButtonLayout);
        }
        
        final ViewGroup f = (ViewGroup) mView.getParent();
        if (mButtonLayout != null) {
            f.addView(mButtonLayout);
        }
        
        return isVisible();
    }
    
    private boolean addContextAction(int n, int index, int action,
                                     int xc, int yc, final RelativeLayout layout) {
        final Button btn = new Button(mContext);
        boolean hasImage = setButtonBackground(btn, action);
        
        btn.setId(action);
        btn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                int action = v.getId();
                removeButtonLayout();
                mCoreView.doContextAction(action);
            }
        });
        btn.setOnTouchListener(new OnTouchListener() {
            public boolean onTouch(View v, MotionEvent event) {
                mView.getParent().requestDisallowInterceptTouchEvent(
                    event.getActionMasked() != MotionEvent.ACTION_UP);
                return false;
            }
        });
        
        if (layout != null) {
            final LayoutParams params = new LayoutParams(
                (hasImage ? 32 : LayoutParams.WRAP_CONTENT),
                (hasImage ? 32 : LayoutParams.WRAP_CONTENT));
            params.leftMargin = xc - (hasImage ? 32 : 64) / 2;
            params.topMargin = yc - (hasImage ? 36 : 40) / 2;
            
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
    
    private boolean setButtonBackground(Button button, int action) {
        if (mImageIDs != null && action < mImageIDs.length
            && action > 0 && mImageIDs[action] != 0) {
            button.setBackgroundResource(mImageIDs[action]);
            return true;
        }
        if (mExtImageIDs != null && action - 40 < mExtImageIDs.length
            && action >= 40 && mExtImageIDs[action - 40] != 0) {
            button.setBackgroundResource(mExtImageIDs[action - 40]);
            return true;
        }
        if (mCaptions == null && mCaptionsID != 0) {
            mCaptions = mContext.getResources().getStringArray(mCaptionsID);
        }
        if (mCaptions != null && action > 0 && action < mCaptions.length) {
            button.setText(mCaptions[action]);
        }
        return false;
    }
    
    public static void setButtonImages(int[] imageIDs, int[] extraImageIDs) {
        mImageIDs = imageIDs;
        mExtImageIDs = extraImageIDs;
    }
    
    public static void setButtonCaptionsID(int captionsID) {
        mCaptionsID = captionsID;
    }
}
