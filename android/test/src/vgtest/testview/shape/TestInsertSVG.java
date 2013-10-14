// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.shape;

import touchvg.view.ViewHelper;
import android.content.Context;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

//! 测试插入SVG图形的视图类
public class TestInsertSVG extends LinearLayout {
    private ViewHelper mHelper = new ViewHelper();

    public TestInsertSVG(Context context) {
        super(context);
        
        this.setOrientation(LinearLayout.VERTICAL);
        createButtons(context);
        
        addView(mHelper.createGraphView(context), 
                new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
    }
    
    private void createButtons(Context context) {
        final LinearLayout layout = new LinearLayout(context);
        final LayoutParams param = new LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        
        layout.setOrientation(LinearLayout.HORIZONTAL);
        this.addView(layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
        
        final Button tigerBtn = new Button(context);
        tigerBtn.setText("tiger");
        layout.addView(tigerBtn, param);
        tigerBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.insertSVGFromResource("tiger", v.getWidth() / 2, v.getHeight() / 2);
            }
        });
        
        final Button mapBtn = new Button(context);
        mapBtn.setText("map");
        layout.addView(mapBtn, param);
        mapBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.insertSVGFromResource("map", v.getWidth() / 2, v.getHeight() / 2);
            }
        });
        
        final Button fontsBtn = new Button(context);
        fontsBtn.setText("fonts");
        layout.addView(fontsBtn, param);
        fontsBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.insertSVGFromResource("fonts", v.getWidth() / 2, v.getHeight() / 2);
            }
        });
        
        final Button picBtn = new Button(context);
        picBtn.setText("bitmap");
        layout.addView(picBtn, param);
        picBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.insertBitmapFromResource("vg_lock", v.getWidth() / 2, v.getHeight() / 2);
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
        
        final Button splinesBtn = new Button(context);
        splinesBtn.setText("Splines");
        layout.addView(splinesBtn, param);
        splinesBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.setCommand("splines");
            }
        });
    }
}
