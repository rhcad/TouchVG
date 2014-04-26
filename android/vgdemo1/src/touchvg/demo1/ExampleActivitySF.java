// Copyright (c) 2014, https://github.com/rhcad/touchvg

package touchvg.demo1;

import rhcad.touchvg.IGraphView;
import android.graphics.Color;
import android.os.Bundle;
import android.view.ViewGroup;

public class ExampleActivitySF extends ExampleActivity1 {

    @Override
    protected void createGraphView(Bundle savedInstanceState) {
        setContentView(R.layout.activity_example1);
        final ViewGroup layout = (ViewGroup) this.findViewById(R.id.frame);
        hlp.createSurfaceView(this, layout);
        hlp.getView().setBackgroundColor(Color.GRAY);
    }

    @Override
    public void onFirstRegen(IGraphView view) {
        super.onFirstRegen(view);
        hlp.startRecord(PATH + "record");
    }
}
