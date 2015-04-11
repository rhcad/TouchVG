//! \file testcanvas.h
//! \brief Define the testing class: TestCanvas.
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_TESTCANVAS_H
#define TOUCHVG_TESTCANVAS_H

class GiCanvas;

//! The testing class for GiCanvas.
/*! \ingroup GRAPH_INTERFACE
 */
struct TestCanvas {
    enum {
        kRect           = 0x01,
        kLine           = 0x02,
        kTextAt         = 0x04,
        kEllipse        = 0x08,
        kQuadBezier     = 0x10,
        kCubicBezier    = 0x20,
        kPolygon        = 0x40,
        kClearRect      = 0x80,
        kClipPath       = 0x100,
        kHandle         = 0x200,
        kDynCurves      = 0x400,
        kRotateText     = 0x800,
        kClearPolygon   = 0x80|0x40|0x02,
    };
    static void initRand();
    static int randInt(int minv, int maxv);
    static float randFloat(float minv, float maxv);
    
    static void test(GiCanvas* canvas, int bits, int n = 100, bool randStyle = true);
    
    static void testRect(GiCanvas* canvas, int n);
    static void testLine(GiCanvas* canvas, int n);
    static void testEllipse(GiCanvas* canvas, int n);
    static void testQuadBezier(GiCanvas* canvas, int n);
    static void testCubicBezier(GiCanvas* canvas, int n);
    static void testPolygon(GiCanvas* canvas, int n);
    static void testClipPath(GiCanvas* canvas, int n);
    static void testHandle(GiCanvas* canvas, int n);
    static void testDynCurves(GiCanvas* canvas);
    static void testTextAt(GiCanvas* canvas, int n);
    static void testRotateText(GiCanvas* canvas, int n);
};

#endif // TOUCHVG_TESTCANVAS_H
