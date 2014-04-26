// Copyright (c) 2013, https://github.com/rhcad/touchvg

package touchvg.demo1;

import rhcad.touchvg.IGraphView;
import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.ViewFactory;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;

public class ExampleActivity1 extends Activity implements IGraphView.OnFirstRegenListener {
    protected IViewHelper hlp = ViewFactory.createHelper();
    protected static final String PATH = "mnt/sdcard/TouchVG/";
    protected static final String VGFILE = PATH + "demo.vg";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.createGraphView(savedInstanceState);
        this.initButtons();
        hlp.setCommand("splines");
        hlp.setStrokeWidth(5);

        if (savedInstanceState == null) {
            hlp.getGraphView().setOnFirstRegenListener(this);
        }
    }

    protected void createGraphView(Bundle savedInstanceState) {
        setContentView(R.layout.activity_example1);
        final ViewGroup layout = (ViewGroup) this.findViewById(R.id.frame);
        hlp.createGraphView(this, layout);
        hlp.getView().setBackgroundColor(Color.GRAY);
    }

    @Override
    public void onDestroy() {
        hlp.onDestroy();
        super.onDestroy();
    }

    @Override
    public void onPause() {
        hlp.onPause();
        super.onPause();
    }

    @Override
    public void onResume() {
        hlp.onResume();
        super.onResume();
    }

    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        hlp.onSaveInstanceState(outState, PATH);
    }

    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        hlp.onRestoreInstanceState(savedInstanceState);
    }

    public void onFirstRegen(IGraphView view) {
        hlp.startUndoRecord(PATH + "undo");
    }

    protected void initButtons() {
        this.findViewById(R.id.selectBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setCommand("select");
            }
        });
        this.findViewById(R.id.splinesBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setCommand("splines");
            }
        });
        this.findViewById(R.id.lineBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setCommand("line");
            }
        });
        this.findViewById(R.id.redPen).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setLineColor(Color.RED);
            }
        });
        this.findViewById(R.id.bluePen).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setLineColor(Color.BLUE);
            }
        });
        this.findViewById(R.id.lineStyleBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.setLineStyle((hlp.getLineStyle() + 1) % IViewHelper.MAX_LINESTYLE);
            }
        });
        this.findViewById(R.id.saveBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.saveToFile(VGFILE);
            }
        });
        this.findViewById(R.id.loadBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.loadFromFile(VGFILE);
            }
        });
        this.findViewById(R.id.addSVGFile).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.insertSVGFromResource(R.raw.text);
            }
        });
        this.findViewById(R.id.undoBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.undo();
            }
        });
        this.findViewById(R.id.redoBtn).setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                hlp.redo();
            }
        });
    }
}
