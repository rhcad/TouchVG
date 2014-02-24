// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.app;

import java.lang.reflect.Constructor;

import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.ViewHelper;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;

public class DummyActivity extends Activity {
    private static final String FILEPATH = "mnt/sdcard/TouchVG";

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Bundle bundle = getIntent().getExtras();
        int flags = bundle.getInt("flags");
        View view = null;

        try {
            final Class<?> c = Class.forName(bundle.getString("className"));
            try {
                final Constructor<?> c2 = c.getDeclaredConstructor(
                        new Class[] { Context.class, Bundle.class });
                c2.setAccessible(true);
                view = (View) c2.newInstance(new Object[] { this, savedInstanceState });
            } catch (Exception e) {
                final Constructor<?> c1 = c.getDeclaredConstructor(new Class[] { Context.class });
                c1.setAccessible(true);
                view = (View) c1.newInstance(new Object[] { this });
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        if ((flags & 0x100000) != 0) { // 有底部SurfaceView
            this.setContentView(view);
        } else {
            // LinearLayout layout = new LinearLayout(this);
            // layout.setOrientation(LinearLayout.VERTICAL);
            final FrameLayout layout = new FrameLayout(this);
            layout.setBackgroundResource(R.drawable.vg_translucent_bg); // 测试图片平铺
            this.setContentView(layout);

            if (view != null) {
                final LayoutParams params = new LayoutParams(LayoutParams.MATCH_PARENT,
                        LayoutParams.MATCH_PARENT);
                layout.addView(view, params);

                if (view instanceof GraphView) {
                    final View dynview = ((GraphView) view).createDynamicShapeView(this);
                    if (dynview != null) {
                        layout.addView(dynview, params);
                    }
                }
            }
        }
        this.setTitle(bundle.getString("title"));
    }

    @Override
    public void onDestroy() {
        new ViewHelper().onActivityDestroy();
        super.onDestroy();
    }

    @Override
    public void onPause() {
        new ViewHelper().onActivityPause();
        super.onPause();
    }

    @Override
    public void onResume() {
        new ViewHelper().onActivityResume();
        super.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        new ViewHelper().onSaveInstanceState(outState, FILEPATH);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        new ViewHelper().onRestoreInstanceState(savedInstanceState);
    }
}
