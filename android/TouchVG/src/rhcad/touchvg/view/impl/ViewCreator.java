// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.impl;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgView;
import rhcad.touchvg.view.BaseGraphView;
import rhcad.touchvg.view.SFGraphView;
import rhcad.touchvg.view.StdGraphView;
import rhcad.touchvg.view.internal.BaseViewAdapter;
import rhcad.touchvg.view.internal.ViewUtil;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class ViewCreator {
    private BaseGraphView mView;

    public void setGraphView(IGraphView view) {
        mView = (BaseGraphView)view;
    }

    public boolean isValid() {
        return mView != null;
    }

    public BaseGraphView getGraphView() {
        return mView;
    }

    public BaseViewAdapter getMainAdapter() {
        return BaseViewAdapter.getMainAdapter(mView);
    }

    public View getView() {
        return mView != null ? mView.getView() : null;
    }

    public ViewGroup getParent() {
        return mView != null ? (ViewGroup) mView.getView().getParent() : null;
    }

    public Context getContext() {
        return mView != null ? mView.getView().getContext() : null;
    }

    public GiCoreView coreView() {
        return mView != null ? mView.coreView() : null;
    }

    public int cmdViewHandle() {
        final GiCoreView v = mView != null ? mView.coreView() : null;
        return v != null ? v.viewAdapterHandle() : 0;
    }

    public MgView cmdView() {
        return MgView.fromHandle(cmdViewHandle());
    }

    public ViewGroup createSurfaceView(Context context, ViewGroup layout, Bundle savedState) {
        final ViewGroup layout1 = layout != null ? layout : new FrameLayout(context);
        final SFGraphView view = new SFGraphView(context, savedState);
        mView = view;
        layout1.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        createDynamicShapeView(context, layout1, view);
        return layout1;
    }

    public ViewGroup createSurfaceAndImageView(Context context, ViewGroup layout, Bundle savedState) {
        final ViewGroup layout1 = layout != null ? layout : new FrameLayout(context);
        final SFGraphView view = new SFGraphView(context, savedState);
        final LayoutParams params = new LayoutParams(LayoutParams.MATCH_PARENT,
                LayoutParams.MATCH_PARENT);

        mView = view;
        layout1.addView(view, params);
        createDynamicShapeView(context, layout1, view);

        ImageView imageView = getImageViewForSurface();
        if (imageView == null) {
            imageView = new ImageView(context);
            layout1.addView(imageView, params);
        } else {
            layout1.bringChildToFront(imageView);
        }
        imageView.setVisibility(View.INVISIBLE);

        return layout1;
    }

    public ImageView getImageViewForSurface() {
        if (mView == null || mView.getView() == null) {
            return null;
        }

        final ViewGroup layout = (ViewGroup) mView.getView().getParent();

        if (layout != null) {
            for (int i = layout.getChildCount() - 1; i >= 0; i--) {
                final View lastView = layout.getChildAt(i);
                if (lastView.getClass() == ImageView.class) {
                    return (ImageView) lastView;
                }
            }
        }
        return null;
    }

    public ViewGroup createGraphView(Context context, ViewGroup layout, Bundle savedState) {
        final ViewGroup layout1 = layout != null ? layout : new FrameLayout(context);
        final StdGraphView view = new StdGraphView(context, savedState);
        mView = view;
        layout1.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        return layout1;
    }

    public ViewGroup createMagnifierView(Context context, ViewGroup layout, IGraphView mainView) {
        final ViewGroup layout1 = layout != null ? layout : new FrameLayout(context);
        final SFGraphView view = new SFGraphView(context,
                (BaseGraphView) (mainView != null ? mainView : mView));
        mView = view;
        layout1.addView(view, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        createDynamicShapeView(context, layout1, view);
        return layout1;
    }

    private void createDynamicShapeView(Context context, ViewGroup layout, IGraphView view) {
        final View dynview = view.createDynamicShapeView(context);
        if (dynview != null) {
            layout.addView(dynview, new LayoutParams(LayoutParams.MATCH_PARENT,
                    LayoutParams.MATCH_PARENT));
        }
    }

    public void createDummyView(Context context, int width, int height) {
        final StdGraphView view = new StdGraphView(context, (BaseGraphView)null);
        view.layout(0, 0, width, height);
        mView = view;
        mView.coreView().onSize(BaseViewAdapter.getMainAdapter(mView), width, height);
    }

    public void close(Bitmap snapshot, final IGraphView.OnViewDetachedListener listener) {
        if (mView != null && mView.getView() != null) {
            final ViewGroup layout = (ViewGroup) mView.getView().getParent();
            final ImageView imageView = getImageViewForSurface();

            if (imageView != null) {
                imageView.setImageBitmap(snapshot);
                imageView.setVisibility(View.VISIBLE);
                layout.bringChildToFront(imageView);

                // remove SurfaceView delayed
                mView.getView().post(new Runnable() {
                    @Override
                    public void run() {
                        mView.getView().removeCallbacks(this);
                        mView.onPause();
                        mView.stop(listener);
                        layout.removeAllViews();
                        mView = null;
                    }
                });
            } else {
                mView.onPause();
                mView.stop(listener);
                if (layout != null) {
                    layout.removeAllViews();
                } else {
                    mView.tearDown();
                }
                mView = null;
            }
        }
    }

    public void onDestroy() {
        final Context context = getContext();
        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                view.stop(null);
            }
        }
    }

    public boolean onPause() {
        Log.d(FileUtil.TAG, "onPause");
        final Context context = getContext();
        boolean ret = false;

        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                ret = view.onPause() || ret;
            }
        }
        return ret;
    }

    public boolean onResume() {
        Log.d(FileUtil.TAG, "onResume");
        final Context context = getContext();
        boolean ret = false;

        for (BaseGraphView view : ViewUtil.views()) {
            if (view.getView().getContext() == context) {
                ret = view.onResume() || ret;
            }
        }
        return ret;
    }
}
