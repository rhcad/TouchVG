// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.shape;

import touchvg.view.ViewHelper;
import vgtest.app.R;
import android.content.Context;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

//! 测试插入SVG图形的视图类
public class TestInsertSVG extends LinearLayout {
    private ViewHelper mHelper = new ViewHelper();
    private static final int[] HANDLEIDS = { R.drawable.vgdot1,
            R.drawable.vgdot2, R.drawable.vgdot3 };

    public TestInsertSVG(Context context) {
        super(context);
        
        this.setOrientation(LinearLayout.VERTICAL);
        createButtons(context);
        
        addView(mHelper.createGraphView(context), 
                new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        ViewHelper.setContextButtonImages(null, R.array.vg_action_captions, null, HANDLEIDS);
    }
    
    private void createButtons(Context context) {
        final LinearLayout layout = new LinearLayout(context);
        final LayoutParams param = new LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        
        layout.setOrientation(LinearLayout.HORIZONTAL);
        this.addView(layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
        
        final Button assetsBtn = new Button(context);
        assetsBtn.setText("Insert from assets");
        layout.addView(assetsBtn, param);
        
        assetsBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
            	mHelper.insertSVGFromResource(R.raw.tiger, v.getWidth() / 2, v.getHeight() / 2);
            }
        });
        
        final Button saveBtn = new Button(context);
        saveBtn.setText("Save PNG");
        layout.addView(saveBtn, param);
        saveBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.savePng("mnt/sdcard/TouchVG/testsvg.png");
            }
        });
        
        final Button lineBtn = new Button(context);
        lineBtn.setText("Line");
        layout.addView(lineBtn, param);
        lineBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.setCommand("line");
            }
        });
    }
}
