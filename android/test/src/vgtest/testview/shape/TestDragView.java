package vgtest.testview.shape;

import android.content.ClipData;
import android.content.Context;
import android.graphics.Color;
import android.view.DragEvent;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

public class TestDragView extends ViewSinShape {

    public TestDragView(Context context) {
        super(context);
    }

    @Override
    protected void createButtons(Context context) {
        final LinearLayout layout = new LinearLayout(context);
        final LayoutParams param = new LayoutParams(LayoutParams.WRAP_CONTENT,
                LayoutParams.WRAP_CONTENT);

        layout.setOrientation(LinearLayout.HORIZONTAL);
        this.addView(layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));

        final Button sinBtn = new Button(context);
        sinBtn.setText("Press me then drag");
        sinBtn.setBackgroundColor(Color.GRAY);
        layout.addView(sinBtn, param);
        sinBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.setCommand(DrawSinShape.NAME);
            }
        });
        sinBtn.setOnLongClickListener(new OnLongClickListener() {
            public boolean onLongClick(View v) {
                ClipData dragData = ClipData.newPlainText("icon", v.getTag() + "");
                DragShadowBuilder shadow = new DragShadowBuilder(v);
                v.startDrag(dragData, shadow, null, 0);
                return true;
            }
        });
    }

    @Override
    public boolean onDragEvent(DragEvent event) {
        boolean ret = true;

        switch (event.getAction()) {
        case DragEvent.ACTION_DRAG_ENTERED:
            mHelper.setCommand(DrawSinShape.NAME);
            ret = mHelper.getGraphView().onTouchDrag(MotionEvent.ACTION_DOWN, event.getX(),
                    event.getY());
            break;

        case DragEvent.ACTION_DRAG_LOCATION:
            ret = mHelper.getGraphView().onTouchDrag(MotionEvent.ACTION_MOVE,
                    event.getX(), event.getY());
            break;

        case DragEvent.ACTION_DROP:
            ret = mHelper.getGraphView().onTouchDrag(MotionEvent.ACTION_UP,
                    event.getX(), event.getY());
            break;

        default:
            break;
        }

        return ret;
    }
}
