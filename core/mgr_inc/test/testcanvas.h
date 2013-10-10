//! \file testcanvas.h
//! \brief Define the testing class: TestCanvas.
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_TESTCANVAS_H
#define TOUCHVG_TESTCANVAS_H

class GiCanvas;

//! The testing class for GiCanvas.
/*! \ingroup GRAPH_INTERFACE
 */
struct TestCanvas {
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
};

#endif // TOUCHVG_TESTCANVAS_H
