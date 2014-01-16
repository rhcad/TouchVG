// Copyright (c) 2013, https://github.com/rhcad/touchvg

package touchvg.demo1;

import rhcad.touchvg.view.ViewHelper;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;

public class ExampleActivity1 extends Activity {
    protected ViewHelper mHelper = new ViewHelper();
    protected static final String PATH = "mnt/sdcard/TouchVG/";
    protected static final String VGFILE = PATH + "demo.vg";
    protected static final String RESUME_FILE = PATH + "resume.vg";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        this.createGraphView();
        this.initButtons();
        mHelper.setCommand("splines");
        mHelper.setStrokeWidth(5);
    }
    
    protected void createGraphView() {
        setContentView(R.layout.activity_example1);
        final ViewGroup layout = (ViewGroup)this.findViewById(R.id.frame);
        mHelper.createGraphViewInLayout(this, layout);
        layout.setBackgroundColor(Color.WHITE);
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        
        if (mHelper.saveToFile(RESUME_FILE)) {
            outState.putString("file", RESUME_FILE);
            outState.putString("cmd", mHelper.getCommand());
            outState.putBoolean("readOnly", mHelper.cmdView().isReadOnly());
        }
    }
    
    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        final String filename = savedInstanceState.getString("file");
        boolean readOnly = savedInstanceState.getBoolean("readOnly");

        if (filename != null && mHelper.loadFromFile(filename, readOnly)) {
            mHelper.setCommand(savedInstanceState.getString("cmd"));
        }
    }
    
    protected void initButtons() {
        this.findViewById(R.id.selectBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setCommand("select");
            }
        });
        this.findViewById(R.id.splinesBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setCommand("splines");
            }
        });
        this.findViewById(R.id.lineBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setCommand("line");
            }
        });
        this.findViewById(R.id.redPen).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setLineColor(Color.RED);
            }
        });
        this.findViewById(R.id.bluePen).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setLineColor(Color.BLUE);
            }
        });
        this.findViewById(R.id.lineStyleBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.setLineStyle((mHelper.getLineStyle() + 1) % ViewHelper.MAX_LINESTYLE);
            }
        });
        this.findViewById(R.id.saveBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.saveToFile(VGFILE);
            }
        });
        this.findViewById(R.id.loadBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.loadFromFile(VGFILE);
            }
        });
        this.findViewById(R.id.addSVGFile).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                mHelper.insertSVGFromResource(R.raw.text);
            }
        });
    }

}
