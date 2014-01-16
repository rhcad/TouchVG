// Copyright (c) 2013, https://github.com/rhcad/touchvg

package touchvg.demo1;

import rhcad.touchvg.view.GraphView;
import rhcad.touchvg.view.GraphView.SelectionChangedListener;
import rhcad.touchvg.view.ShapeView;
import android.animation.ObjectAnimator;
import android.graphics.Color;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;

public class AnimationActivity1 extends ExampleActivity1 {

    @Override
    protected void createGraphView() {
        setContentView(R.layout.activity_animation1);
        
        final ViewGroup layout = (ViewGroup)this.findViewById(R.id.frame);
        mHelper.createShapeView(this, layout).setBackgroundColor(Color.WHITE);
        
        mHelper.getView().setContextActionEnabled(false);
        mHelper.getView().setOnSelectionChangedListener(new SelectionChangedListener() {
            @Override
            public void onSelectionChanged(GraphView view) {
                int id = mHelper.getSelectedShapeID();
                final View v = ((ShapeView)view).findShapeView(id);
                if (v != null) {
                    ObjectAnimator animator = ObjectAnimator.ofFloat(v, "alpha", 0, 1);
                    animator.setRepeatCount(10);
                    animator.start();
                }
            }
        });
    }
    
    @Override
    protected void initButtons() {
        this.findViewById(R.id.loadBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setImagePath(PATH);
                mHelper.loadFromFile(VGFILE, true);
                mHelper.setCommand("select");
            }
        });
    }
}
