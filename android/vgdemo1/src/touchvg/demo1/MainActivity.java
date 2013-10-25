package touchvg.demo1;

import touchvg.view.ViewHelper;
import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.FrameLayout;

public class MainActivity extends Activity {
    private ViewHelper mHelper = new ViewHelper();
    private static final String VGFILE = "mnt/sdcard/TouchVG/demo.vg";
    private static final String RESUME_FILE = "mnt/sdcard/TouchVG/resume.vg";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        final FrameLayout layout = (FrameLayout)this.findViewById(R.id.frame);
        mHelper.createGraphViewInLayout(this, layout).setBackgroundColor(Color.WHITE);
        this.initButtons();
    }
    
    @Override
    protected void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
        
        if (mHelper.saveToFile(RESUME_FILE)) {
            outState.putString("file", RESUME_FILE);
            outState.putString("cmd", mHelper.getCommand());
        }
    }
    
    @Override
    protected void onRestoreInstanceState(Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);

        final String filename = savedInstanceState.getString("file");
        if (filename != null && mHelper.loadFromFile(filename)) {
            mHelper.setCommand(savedInstanceState.getString("cmd"));
        }
    }
    
    private void initButtons() {
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
    }

}
