// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview.shape;

import java.util.ArrayList;
import java.util.List;

import rhcad.touchvg.IViewHelper;
import rhcad.touchvg.ViewFactory;
import rhcad.touchvg.core.CmdObserverDefault;
import rhcad.touchvg.core.ConstShapes;
import rhcad.touchvg.core.GiContext;
import rhcad.touchvg.core.GiGraphics;
import rhcad.touchvg.core.Ints;
import rhcad.touchvg.core.Matrix2d;
import rhcad.touchvg.core.MgBaseShape;
import rhcad.touchvg.core.MgCmdManager;
import rhcad.touchvg.core.MgCommand;
import rhcad.touchvg.core.MgCommandDraw;
import rhcad.touchvg.core.MgHitResult;
import rhcad.touchvg.core.MgMotion;
import rhcad.touchvg.core.MgObject;
import rhcad.touchvg.core.MgShape;
import rhcad.touchvg.core.MgShapeFactory;
import rhcad.touchvg.core.MgSplines;
import rhcad.touchvg.core.MgStorage;
import rhcad.touchvg.core.Point2d;
import vgtest.app.R;
import android.content.Context;
import android.graphics.Color;
import android.view.View;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.LinearLayout;

//! 测试自定义图形类的测试视图类
public class ViewSinShape extends LinearLayout {
    protected IViewHelper mHelper = ViewFactory.createHelper();
    private MyCmdObserver mObserver = new MyCmdObserver();
    private static final int[] MYIMAGEIDS = { R.drawable.demo_switch };

    public ViewSinShape(Context context) {
        super(context);

        this.setOrientation(LinearLayout.VERTICAL);
        createButtons(context);

        final FrameLayout layout = new FrameLayout(context);
        addView(mHelper.createGraphView(context, layout), new LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));

        mHelper.setExtraContextImages(context, MYIMAGEIDS);
        ViewFactory.registerCmdObserver(mHelper, mObserver);
    }

    protected void createButtons(Context context) {
        final LinearLayout layout = new LinearLayout(context);
        final LayoutParams param = new LayoutParams(LayoutParams.WRAP_CONTENT,
                LayoutParams.WRAP_CONTENT);

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
                final ConstShapes shapes = new ConstShapes();
                sender.getView().getSelection().getSelection(sender.getView(), shapes);

                for (int i = shapes.count() - 1; i >= 0; i--) {
                    final MgShape shape = shapes.get(i).cloneShape();
                    final SinShape sp = castSinShape(shape.shape());

                    if (sp != null) {
                        sp.switchValue();
                        sp.update();
                        sender.getView().shapes().updateShape(shape);
                    } else {
                        shape.release();
                    }
                }
                shapes.delete();
                sender.getView().regenAll(true);
            }
            return super.doAction(sender, action);
        }
    }

    private List<MgCommand> mCmdCache = new ArrayList<MgCommand>();

    protected class DrawSinShape extends MgCommandDraw {
        public static final String NAME = "sin";

        public DrawSinShape() {
            super(NAME);
            mCmdCache.add(this);
        }

        @Override
        public void release() {
            mCmdCache.remove(this);
            delete();
        }

        @Override
        public int getShapeType() {
            return SinShape.TYPE;
        }

        @Override
        public boolean click(MgMotion sender) {
            super.click(sender); // 看能否点中已有图形
            if (sender.getView().isCommand(NAME)) { // 没退出命令
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
            return sender.getView().toSelectCommand(); // 画完一个就退出
        }
    }

    private static List<SinShape> mShapeCache;

    public SinShape castSinShape(MgObject obj) {
        return mShapeCache.isEmpty() ? null : mShapeCache.iterator().next().castShape(obj);
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
                mCurve.addPoint(new Point2d(i * 0.5f, (float) y));
            }
        }

        public SinShape castShape(MgObject obj) {
            for (SinShape sp : mShapeCache) {
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
                mCurve.addPoint(new Point2d(i * 0.5f, (float) y));
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
            // mCurve.clear();
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
