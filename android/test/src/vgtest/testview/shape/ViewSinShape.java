// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.shape;

import java.util.ArrayList;
import java.util.List;

import touchvg.core.CmdObserverDefault;
import touchvg.core.GiContext;
import touchvg.core.GiGraphics;
import touchvg.core.Ints;
import touchvg.core.Matrix2d;
import touchvg.core.MgBaseShape;
import touchvg.core.MgCmdManager;
import touchvg.core.MgCommand;
import touchvg.core.MgCommandDraw;
import touchvg.core.MgHitResult;
import touchvg.core.MgMotion;
import touchvg.core.MgObject;
import touchvg.core.MgShape;
import touchvg.core.MgShapeFactory;
import touchvg.core.MgShapesLock;
import touchvg.core.MgSplines;
import touchvg.core.MgStorage;
import touchvg.core.Point2d;
import touchvg.core.Shapes;
import touchvg.view.ViewHelper;
import vgtest.app.R;
import android.content.Context;
import android.graphics.Color;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;

//! 测试自定义图形类的测试视图类
public class ViewSinShape extends LinearLayout {
    protected ViewHelper mHelper = new ViewHelper();
    private MyCmdObserver mObserver = new MyCmdObserver();
    private static final int[] HANDLEIDS = { R.drawable.vgdot1,
            R.drawable.vgdot2, R.drawable.vgdot3 };
    private static final int[] MYIMAGEIDS = { R.drawable.demo_switch };

    public ViewSinShape(Context context) {
        super(context);
        
        this.setOrientation(LinearLayout.VERTICAL);
        createButtons(context);
        
        addView(mHelper.createGraphView(context), 
                new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        ViewHelper.setContextButtonImages(null, R.array.vg_action_captions,
                MYIMAGEIDS, HANDLEIDS);
        mHelper.registerCmdObserver(mObserver);
    }
    
    protected void createButtons(Context context) {
        final LinearLayout layout = new LinearLayout(context);
        final LayoutParams param = new LayoutParams(
                LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT);
        
        layout.setOrientation(LinearLayout.HORIZONTAL);
        this.addView(layout, new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
        
        final Button sinBtn = new Button(context);
        sinBtn.setText("Sin");
        sinBtn.setBackgroundColor(Color.GRAY);
        layout.addView(sinBtn, param);
        sinBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.setCommand(DrawSinShape.NAME);
            }
        });
        
        final Button selectBtn = new Button(context);
        selectBtn.setText("Select");
        selectBtn.setBackgroundColor(Color.GRAY);
        layout.addView(selectBtn, param);
        selectBtn.setOnClickListener(new OnClickListener() {
            public void onClick(View v) {
                mHelper.setCommand("select");
            }
        });
    }
    
    private class MyCmdObserver extends CmdObserverDefault {
        private static final int ACTION_SWITCH = 40;
        
        @Override
        public void onUnloadCommands(MgCmdManager sender) {
            if (mObserver != null) {
                mObserver.delete();
                mObserver = null;
            }
            mHelper = null;
        }
        
        @Override
        public MgBaseShape createShape(MgMotion sender, int type) {
            if (type == SinShape.TYPE)
                return new SinShape();
            return super.createShape(sender, type);
        }
        
        @Override
        public MgCommand createCommand(MgMotion sender, String name) {
            if (name.equals(DrawSinShape.NAME))
                return new DrawSinShape();
            return super.createCommand(sender, name);
        }
        
        @Override
        public int addShapeActions(MgMotion sender, Ints actions, int n, MgShape sp) {
            if (castSinShape(sp.shapec()) != null) {
                actions.set(n++, ACTION_SWITCH);
            }
            return n;
        }
        
        @Override
        public boolean doAction(MgMotion sender, int action) {
            if (action == ACTION_SWITCH) {
                final MgShapesLock locker = new MgShapesLock(sender.getView());
                if (locker.locked()) {
                    final Shapes shapes = new Shapes();
                    sender.getView().getSelection().getSelection(sender.getView(), shapes);

                    for (int i = shapes.count() - 1; i >= 0; i--) {
                        final MgShape shape = shapes.get(i);
                        final SinShape sp = castSinShape(shape.shape());

                        if (sp != null) {
                            sp.switchValue();
                            sp.update();
                        }
                    }
                    shapes.delete();
                    sender.getView().regenAll();
                }
                locker.delete();
            }
            return super.doAction(sender, action);
        }
    }
    
    private List<MgCommand> mCmdCache = new ArrayList<MgCommand>();
    
    protected class DrawSinShape extends MgCommandDraw {
        public static final String NAME = "sin";
        
        public DrawSinShape() {
            mCmdCache.add(this);
        }
        
        @Override
        public String getName() {
            return NAME;
        }
        
        @Override
        public void release() {
            mCmdCache.remove(this);
            delete();
        }
        
        @Override
        public MgShape createShape(MgShapeFactory factory) {
            return factory.createShape(SinShape.TYPE);
        }
        
        @Override
        public boolean click(MgMotion sender) {
            super.click(sender);            // 看能否点中已有图形
            if (sender.getView().getCommandName().equals(NAME)) { // 没退出命令
                dynshape().shape().setHandlePoint(0, snapPoint(sender), 0);
                addShape(sender);
            }
            return true;
        }
        
        @Override
        public boolean touchBegan(MgMotion sender) {
            setStep(1);
            return super.touchBegan(sender);
        }
        
        @Override
        public boolean touchMoved(MgMotion sender) {
            dynshape().shape().setHandlePoint(0, snapPoint(sender), 0);
            return super.touchMoved(sender);
        }
        
        @Override
        public boolean touchEnded(MgMotion sender) {
            dynshape().shape().setHandlePoint(0, snapPoint(sender), 0);
            addShape(sender);
            return sender.cancel();     // 画完一个就退出
        }
    }

    private static List<SinShape> mShapeCache;
    
    public SinShape castSinShape(MgObject obj) {
        return mShapeCache.isEmpty() ? null :
            mShapeCache.iterator().next().castShape(obj);
    }
    
    private class SinShape extends MgBaseShape {
        public static final int TYPE = 100;
        private MgSplines mCurve = new MgSplines();
        int mTestValue = 0;
        
        public SinShape() {
            if (mShapeCache == null)
                mShapeCache = new ArrayList<SinShape>();
            mShapeCache.add(this);
            
            for (int i = 0; i < 40; i++) {
                double y = 10 * Math.sin(i * Math.PI / 10);
                mCurve.addPoint(new Point2d(i * 0.5f, (float)y));
            }
        }
        
        public SinShape castShape(MgObject obj) {
            for (SinShape sp: mShapeCache) {
                if (MgObject.getCPtr(obj) == MgObject.getCPtr(sp)) {
                    return sp;
                }
            }
            return null;
        }
        
        public void switchValue() {
            mTestValue++;
            Point2d oldpt = getPoint(0);
            mCurve.clear();
            for (int i = 0; i < 40; i++) {
                double y = 10 * Math.sin((i + mTestValue * 2) * Math.PI / 10);
                mCurve.addPoint(new Point2d(i * 0.5f, (float)y));
            }
            update();
            setPoint(0, oldpt);
        }
        
        @Override
        public int getType() {
            return TYPE;
        }
        
        @Override
        public boolean isKindOf(int type) {
            return type == TYPE || super.isKindOf(type);
        }

        @Override
        public String getTypeName() {
            return this.getClass().getSimpleName();
        }
        
        @Override
        public MgObject clone() {
            SinShape obj = new SinShape();
            obj.copy(this);
            return obj;
        }
        
        @Override
        public void copy(MgObject src) {
            super.copy(src);
            SinShape s = castShape(src);
            if (s != null) {
                mCurve.copy(s.mCurve);
            }
        }
        
        @Override
        public void release() {
            mShapeCache.remove(this);
            delete();
        }
        
        @Override
        public boolean equals(MgObject src) {
            SinShape s = castShape(src);
            if (s == null || !mCurve.equals(s.mCurve)) {
                return false;
            }
            return super.equals(src);
        }

        @Override
        public void update() {
            mCurve.update();
            super.setExtent(mCurve.getExtent());
            super.update();
        }

        @Override
        public void transform(Matrix2d mat) {
            mCurve.transform(mat);
            super.transform(mat);
        }

        @Override
        public void clear() {
            //mCurve.clear();
            super.clear();
        }

        @Override
        public int getPointCount() {
            return 1;
        }

        @Override
        public Point2d getPoint(int index) {
            return mCurve.getExtent().center();
        }

        @Override
        public void setPoint(int index, Point2d pt) {
            mCurve.offset(pt.subtract(getPoint(0)), -1);
        }

        @Override
        public boolean isCurve() {
            return true;
        }

        @Override
        public float hitTest(Point2d pt, float tol, MgHitResult res) {
            return mCurve.hitTest(pt, tol, res);
        }

        @Override
        public boolean draw(int mode, GiGraphics gs, GiContext ctx, int segment) {
            boolean ret = mCurve.draw(mode, gs, ctx, segment);
            return super.draw(mode, gs, ctx, segment) || ret;
        }

        @Override
        public boolean save(MgStorage s) {
            final Point2d pt = getPoint(0);
            s.writeInt("testValue", mTestValue);
            s.writeFloat("x", pt.getX());
            s.writeFloat("y", pt.getY());
            return super.save(s);
        }

        @Override
        public boolean load(MgShapeFactory factory, MgStorage s) {
            mTestValue = s.readInt("testValue", mTestValue);
            setPoint(0, new Point2d(s.readFloat("x", 0), s.readFloat("y", 0)));
            return super.load(factory, s);
        }

        @Override
        public int getHandleCount() {
            return super.getHandleCount();
        }

        @Override
        public Point2d getHandlePoint(int index) {
            return super.getHandlePoint(index);
        }

        @Override
        public boolean setHandlePoint(int index, Point2d pt, float tol) {
            return super.setHandlePoint(index, pt, tol);
        }
        
        @Override
        public void setOwner(MgShape owner) {
            super.setOwner(owner);
        }
    }
}
