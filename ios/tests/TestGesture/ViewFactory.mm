// ViewFactory.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GraphView2.h"
#import "LargeView2.h"

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

static void addView2(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                     NSString* title, int flags, CGRect frame)
{
    GraphView2 *view = nil;
    
    if (!arr && index == i++) {
        view = [[GraphView2 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
    [view release];
}

static void addLargeView2(NSMutableArray *arr, NSUInteger &i, NSUInteger index, 
                          NSString* title, int flags, CGRect frame)
{
    LargeView2 *view = nil;
    
    if (!arr && index == i++) {
        view = [[LargeView2 alloc]initWithFrame:frame withFlags:flags];
    }
    addView(arr, title, view);
    [view release];
}

static void gatherTestView(NSMutableArray *arr, NSUInteger index, CGRect frame)
{
    NSUInteger i = 0;
    
    addView2(arr, i, index, @"testTouch", 0, frame);
    addView2(arr, i, index, @"testGesture", 1, frame);
    addView2(arr, i, index, @"testGestureTouches", 2, frame);
    
    addLargeView2(arr, i, index, @"testTouch in large view", 0, frame);
    addLargeView2(arr, i, index, @"testGesture in large view", 1, frame);
    addLargeView2(arr, i, index, @"testGestureTouches in large view", 2, frame);
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
