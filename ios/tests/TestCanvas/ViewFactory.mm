// ViewFactory.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GraphView3.h"
#import "LargeView3.h"
#import "BasicAnimationView.h"
#include "testcanvas.h"

static UIViewController *_tmpController = nil;

static void addView(NSMutableArray *arr, NSString* title, UIView* view)
{
    if (arr) {
        [arr addObject:title];
    }
    else if (view) {
        _tmpController = [[UIViewController alloc] init];
        _tmpController.title = title;
        _tmpController.view = view;
    }
}

static void addView3(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                     NSString* title, int flags, CGRect frame)
{
    GraphView3 *view = nil;
    
    if (!arr && index == i++) {
        view = [[GraphView3 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
}

static void addView2(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                     NSString* title, int flags, CGRect frame)
{
    GraphView4 *view = nil;
    
    if (!arr && index == i++) {
        view = [[GraphView4 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
}

static void addLargeView3(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                         NSString* title, int flags, CGRect frame)
{
    LargeView3 *view = nil;
    
    if (!arr && index == i++) {
        view = [[LargeView3 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
}
/*
static void addGraphView(NSMutableArray *arr, NSUInteger &i, NSUInteger index,
                         NSString* title, CGRect frame)
{
    UIView *wrapview = nil;
    
    if (!arr && index == i++) {
        wrapview = [[UIView alloc]initWithFrame:frame];
        wrapview.opaque = NO;
    }
    addView(arr, title, wrapview);
    [wrapview RELEASE];
    if (wrapview) {
        GiGraphView3 *v = [[GiGraphView3 alloc]initWithFrame:wrapview.bounds];
        [wrapview addSubview:v];
    }
}*/

static void gatherTestView(NSMutableArray *arr, NSUInteger index, CGRect frame)
{
    NSUInteger i = 0;
    
    //addGraphView(arr, i, index, @"GiGraphView3", frame);
    
    addView3(arr, i, index, @"testRect", TestCanvas::kRect, frame);
    addView3(arr, i, index, @"testLine", TestCanvas::kLine, frame);
    addView3(arr, i, index, @"testTextAt", TestCanvas::kTextAt, frame);
    addView3(arr, i, index, @"testEllipse", TestCanvas::kEllipse, frame);
    addView3(arr, i, index, @"testQuadBezier", TestCanvas::kQuadBezier, frame);
    addView3(arr, i, index, @"testCubicBezier", TestCanvas::kCubicBezier, frame);
    addView3(arr, i, index, @"testPolygon", TestCanvas::kPolygon, frame);
    addView3(arr, i, index, @"testClearRect", TestCanvas::kClearPolygon, frame);
    addView3(arr, i, index, @"testClipPath", TestCanvas::kClipPath, frame);
    addView3(arr, i, index, @"testHandle", TestCanvas::kHandle, frame);
    addView3(arr, i, index, @"testDynCurves", TestCanvas::kDynCurves, frame);
    addView3(arr, i, index, @"testBeziers with dynview",
             TestCanvas::kCubicBezier|kWithDynView, frame);
    
    addView2(arr, i, index, @"testDynCurves with thread",
             TestCanvas::kDynCurves|kWithThread, frame);
    addView2(arr, i, index, @"testCubicBezier with thread",
             TestCanvas::kCubicBezier|kWithThread, frame);
    addView2(arr, i, index, @"testEllipse with thread",
             TestCanvas::kEllipse|kWithThread, frame);
    
    addLargeView3(arr, i, index, @"testTextAt in large view",
                  TestCanvas::kTextAt|kWithLargeView, frame);
    addLargeView3(arr, i, index, @"testCubicBezier in large view",
                  TestCanvas::kCubicBezier|kWithLargeView, frame);
    addLargeView3(arr, i, index, @"testHandle in large view",
                  TestCanvas::kHandle|kWithLargeView, frame);
    addLargeView3(arr, i, index, @"testDynCurves in large view",
                  TestCanvas::kDynCurves|kWithLargeView, frame);
    
    BasicAnimationView *baview = nil;
    if (!arr && index == i++) {
        baview = [[BasicAnimationView alloc]initWithFrame:frame];
    }
    addView(arr, @"CABasicAnimation", baview);
}

void getTestViewTitles(NSMutableArray *arr)
{
    gatherTestView(arr, 0, CGRectNull);
}

UIViewController *createTestView(NSUInteger index, CGRect frame)
{
    _tmpController = nil;
    gatherTestView(nil, index, frame);
    return _tmpController;
}
