//! \file ShapeView.java
//! \brief 定义每个图形都是一个子视图的绘图视图类  ShapeView
// Copyright (c) 2013, https://github.com/rhcad/touchvg

package rhcad.touchvg.view;

import rhcad.touchvg.view.internal.ImageCache;
import rhcad.touchvg.view.internal.PictureAdapter;
import rhcad.touchvg.view.internal.PictureAdapter.PictureCreated;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Picture;
import android.graphics.drawable.PictureDrawable;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

//! 每个图形都是一个子视图的绘图视图类
/*! \ingroup GROUP_ANDROID
 */
public class ShapeView extends StdGraphView {
    private RelativeLayout mShapeLayout;
    private PictureAdapter mRegenAdapter;

    public ShapeView(Context context) {
        super(context);
    }
    
    @Override
    protected void createAdapter(Context context) {
        mImageCache = new ImageCache();
        mCanvasAdapter = new CanvasAdapter(this, mImageCache);
        mViewAdapter = new ShapeViewAdapter();
        mRegenAdapter = new PictureAdapter(this, mImageCache, new PictureCreated() {
            @Override
            public void onPictureCreated(Picture p, float x, float y, int id) {
                final ImageView v = new ImageView(getContext());
                v.setLayerType(View.LAYER_TYPE_SOFTWARE, null); // for drawPicture
                v.setImageDrawable(new PictureDrawable(p));
                v.setId(id);
                
                final LayoutParams params = new LayoutParams(
                        LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
                params.leftMargin = Math.round(x);
                params.topMargin = Math.round(y);
                mShapeLayout.addView(v, params);
            }
        });
    }

    @Override
    public void clearCachedData() {
        super.clearCachedData();
    }
    
    @Override
    protected void onDetachedFromWindow() {
        mShapeLayout = null;
        if (mRegenAdapter != null) {
            mRegenAdapter.delete();
            mRegenAdapter = null;
        }
        super.onDetachedFromWindow();
    }
    
    @Override
    protected void onDraw(Canvas canvas) {
        mCoreView.onSize(mViewAdapter, getWidth(), getHeight());
        if (mCanvasAdapter.beginPaint(canvas)) {
            mCoreView.dynDraw(mViewAdapter, mCanvasAdapter);
            mCanvasAdapter.endPaint();
        }
    }
    
    //! 查找指定ID的图形视图
    public View findShapeView(int id) {
        return mShapeLayout != null ? mShapeLayout.findViewById(id) : null;
    }

    //! 视图回调适配器
    protected class ShapeViewAdapter extends StdViewAdapter {
        @Override
        public void regenAll(boolean changed) {
            synchronized (mCoreView) {
                if (changed)
                    mCoreView.submitBackDoc(mViewAdapter);
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
            final ViewGroup f = (ViewGroup) getParent();
            if (mShapeLayout != null) {
                f.removeView(mShapeLayout);
                mShapeLayout = null;
            }
            mShapeLayout = new RelativeLayout(getContext());
            mCoreView.drawAll(mViewAdapter, mRegenAdapter);
            f.addView(mShapeLayout);
        }
        
        @Override
        public void regenAppend(int sid) {
            regenAll(true);
        }
        
        @Override
        public void redraw() {
            synchronized (mCoreView) {
                mCoreView.submitDynamicShapes(mViewAdapter);
            }
        }
    }
}
