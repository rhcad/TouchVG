//! \file ViewFactory.java
//! \brief 基于普通View类的绘图测试视图类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package vgtest.testview;

import java.util.ArrayList;
import java.util.List;

import rhcad.touchvg.core.TestCanvas;

//! 测试视图的构造列表类
public class ViewFactory {

    public static class DummyItem {

        public String id;
        public int flags;
        public String title;

        public DummyItem(String id, int flags, String title) {
            this.id = id;
            this.flags = flags;
            this.title = title;
        }

        @Override
        public String toString() {
            return title;
        }
    }

    public static List<DummyItem> ITEMS = new ArrayList<DummyItem>();

    static {
        addItem("vgtest.testview.shape.ViewSinShape", 0, "ViewSinShape");
        addItem("vgtest.testview.shape.TestDragView", 0, "TestDragView");
        addItem("vgtest.testview.shape.TestInsertSVG", 0, "TestInsertSVG");

        addItem("vgtest.testview.view.SFGraphView1", TestFlags.SPLINES_CMD, "SFGraphView splines");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RAND_SPLINES, "SFGraphView randShapes splines");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RAND_LINE, "SFGraphView randShapes line");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.LINE_CMD, "SFGraphView line");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.LINES_CMD, "SFGraphView lines");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.SWITCH_CMD, "SFGraphView switch command");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.SWITCH_CMD|TestFlags.HITTEST_CMD, "SFGraphView switch hittest in democmds");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RECORD_SPLINES, "SFGraphView record splines");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RECORD_LINE, "SFGraphView record line");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RECORD_SPLINES_RAND, "SFGraphView record randShapes splines");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RECORD_LINE_RAND, "SFGraphView record randShapes line");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RECORD | TestFlags.RAND_SHAPES, "SFGraphView randShapes play");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.RAND_SELECT, "SFGraphView select");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.HAS_BACKDRAWABLE | TestFlags.RAND_SELECT, "SFGraphView select transparent");
        addItem("vgtest.testview.view.SFGraphView1", TestFlags.HAS_BACKDRAWABLE | TestFlags.RAND_SPLINES, "SFGraphView draw transparent");

        addItem("vgtest.testview.view.GraphView1", TestFlags.SPLINES_CMD, "StdGraphView splines");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RAND_SPLINES, "StdGraphView randShapes splines");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RAND_LINE, "StdGraphView randShapes line");
        addItem("vgtest.testview.view.GraphView1", TestFlags.LINE_CMD, "StdGraphView line");
        addItem("vgtest.testview.view.GraphView1", TestFlags.LINES_CMD, "StdGraphView lines");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RECORD_SPLINES, "StdGraphView record splines");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RECORD_LINE, "StdGraphView record line");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RECORD_SPLINES_RAND, "StdGraphView record randShapes splines");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RECORD_LINE_RAND, "StdGraphView record randShapes line");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RECORD | TestFlags.RAND_SHAPES, "StdGraphView randShapes play");
        addItem("vgtest.testview.view.GraphView1", TestFlags.SWITCH_CMD, "StdGraphView switch command");
        addItem("vgtest.testview.view.GraphView1", TestFlags.SWITCH_CMD|TestFlags.HITTEST_CMD, "StdGraphView switch hittest in democmds");
        addItem("vgtest.testview.view.GraphView1", TestFlags.RAND_SELECT, "StdGraphView select");

        addItem("vgtest.testview.view.TestMagnifier1", TestFlags.SPLINES_CMD, "TestMagnifier");
        addItem("vgtest.testview.view.TestMagnifier1", TestFlags.TWO_MAGVIEWS | TestFlags.SPLINES_CMD, "TestMagnifier, 2 views");
        addItem("vgtest.testview.view.LargeView1", TestFlags.SPLINES_CMD, "Scroll GraphView splines");
        addItem("vgtest.testview.view.LargeView1", TestFlags.LINE_CMD, "Scroll GraphView line");
        addItem("vgtest.testview.view.LargeView1", TestFlags.RAND_SELECT, "Scroll GraphView select");

        addItem("vgtest.testview.view.GraphView", 0, "TestOneView");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 0, "TestOneSurfaceView");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 0 | TestFlags.MODEL_SURFACE, "TestOneSurfaceView, back");
        addItem("vgtest.testview.view.GraphViewCached", 0, "GraphViewCached");

        addItem("vgtest.testview.view.TestDoubleViews", 0 | 2, "Test2Views, std view+view");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 2, "Test2Views, top surface+view");
        addItem("vgtest.testview.view.TestDoubleViews", 0 | 4, "Test2Views, std view+surface");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 4, "Test2Views, top surface+surface");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 2 | TestFlags.MODEL_SURFACE, "Test2Views, back surface+view");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 4 | TestFlags.MODEL_SURFACE, "Test2Views, back surface+surface");
        addItem("vgtest.testview.view.TestDoubleViews", 8 | 4, "Test2Views, cachedview+surface");
        addItem("vgtest.testview.view.TestDoubleViews", 8 | 2, "Test2Views, cachedview+view");

        addItem("vgtest.testview.view.LargeView2", 0 | 2, "Test2Views, scroll view+view");
        addItem("vgtest.testview.view.LargeView2", 1 | 2, "Test2Views, scroll surface+view");
        addItem("vgtest.testview.view.LargeView2", 0 | 4, "Test2Views, scroll view+surface");
        addItem("vgtest.testview.view.LargeView2", 1 | 4, "Test2Views, scroll surface+surface");
        addItem("vgtest.testview.view.LargeView2", 0 | 0, "TestOneView, scroll");
        addItem("vgtest.testview.view.LargeView2", 1 | 0, "TestOneSurfaceView, scroll");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 2 | TestFlags.MODEL_SURFACE, "Test2Views, scroll back surface+view");
        addItem("vgtest.testview.view.TestDoubleViews", 1 | 4 | TestFlags.MODEL_SURFACE, "Test2Views, scroll back surface+surface");
        addItem("vgtest.testview.view.LargeView2", 8 | 4, "Test2Views, scroll cachedview+surface");
        addItem("vgtest.testview.view.LargeView2", 8 | 2, "Test2Views, scroll cachedview+surface");

        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kRect, "testRect");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kLine, "testLine");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kTextAt, "testTextAt");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kEllipse, "testEllipse");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kQuadBezier, "testQuadBezier");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kCubicBezier, "testCubicBezier");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kPolygon, "testPolygon");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kClearRect | TestCanvas.kTextAt | TestCanvas.kLine, "testClearRect");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kClipPath, "testClipPath");
        addItem("vgtest.testview.canvas.GraphView1", TestCanvas.kHandle, "testHandle");
        addItem("vgtest.testview.canvas.GraphView2", TestCanvas.kDynCurves, "testDynCurves");

        addItem("vgtest.testview.canvas.SurfaceView1", TestCanvas.kCubicBezier, "testCubicBezier in SurfaceView");
        addItem("vgtest.testview.canvas.SurfaceView1", TestCanvas.kClearPolygon, "testClearRect in SurfaceView");
        addItem("vgtest.testview.canvas.SurfaceView2", TestCanvas.kCubicBezier, "testCubicBezier in SurfaceView with thread");
        addItem("vgtest.testview.canvas.SurfaceView2", TestCanvas.kLine, "testLine in SurfaceView with thread");
        addItem("vgtest.testview.canvas.SurfaceView2", TestCanvas.kDynCurves, "testDynCurves in SurfaceView with touch");
        addItem("vgtest.testview.canvas.SurfaceView3", TestCanvas.kDynCurves, "testDynCurves in SurfaceView with thread");
        addItem("vgtest.testview.canvas.SurfaceView3", TestCanvas.kDynCurves | TestFlags.OPAQUE_VIEW, "testDynCurves(OPAQUE) with thread");

        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kTextAt | TestFlags.LARGE_VIEW, "testTextAt in large view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kTextAt | TestFlags.LARGE_SURFACEVIEW, "testTextAt in large surface view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kCubicBezier | TestFlags.LARGE_VIEW, "testCubicBezier in large view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kCubicBezier | TestFlags.LARGE_SURFACEVIEW, "testCubicBezier in large surface view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kHandle | TestFlags.LARGE_VIEW, "testHandle in large view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kHandle | TestFlags.LARGE_SURFACEVIEW, "testHandle in large surface view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kDynCurves | TestFlags.LARGE_VIEW, "testDynCurves in large view");
        addItem("vgtest.testview.canvas.LargeView1", TestCanvas.kDynCurves | TestFlags.LARGE_SURFACEVIEW, "testDynCurves in large surface view");
    }

    private static void addItem(String id, int flags, String title) {
        ITEMS.add(new DummyItem(id, flags, title));
    }
}
