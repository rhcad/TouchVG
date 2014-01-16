// Copyright (c) 2014, https://github.com/rhcad/touchvg

package touchvg.demo1;

import android.graphics.Color;
import android.view.ViewGroup;

public class ExampleActivitySF extends ExampleActivity1 {

    @Override
    protected void createGraphView() {
        setContentView(R.layout.activity_example1);
        final ViewGroup layout = (ViewGroup)this.findViewById(R.id.frame);
        mHelper.createSurfaceView(this, layout).setBackgroundColor(Color.GRAY);
    }
}
