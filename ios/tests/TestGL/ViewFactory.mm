// ViewFactory.mm
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#import "GLView1.h"

@interface TestViewController : UIViewController
@end

static TestViewController *_tmpController = nil;

@implementation TestViewController

- (void)viewWillDisappear:(BOOL)animated {
    if ([self.view respondsToSelector:@selector(tearDown)]) {
        [self.view performSelector:@selector(tearDown)];
    }
    [super viewWillDisappear:animated];
}

@end

static void addView(NSMutableArray *arr, NSString* title, UIView* view)
{
    if (arr) {
        [arr addObject:title];
    }
    else if (view) {
        _tmpController = [[TestViewController alloc] init];
        _tmpController.title = title;
        _tmpController.view = view;
    }
}

static void addView1(NSMutableArray *arr, NSUInteger &i, NSUInteger index,
                     NSString* title, int flags, CGRect frame)
{
    GLView1 *view = nil;
    
    if (!arr && index == i++) {
        view = [[GLView1 alloc]initWithFrame:frame];
    }
    addView(arr, title, view);
}

static void gatherTestView(NSMutableArray *arr, NSUInteger index, CGRect frame)
{
    NSUInteger i = 0;
    
    addView1(arr, i, index, @"GLView1", 0, frame);
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
