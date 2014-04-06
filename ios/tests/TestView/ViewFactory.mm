// ViewFactory.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphView1.h"
#import "LargeView1.h"
#import "GiViewHelper.h"
#include "DemoCmds.h"

static UIViewController *_tmpController = nil;

static void addView(NSMutableArray *arr, NSString* title, UIView* view)
{
    if (arr) {
        [arr addObject:title];
    }
    else if (view) {
        _tmpController = [[[UIViewController alloc] init] AUTORELEASE];
        _tmpController.title = title;
        _tmpController.view = view;
    }
}

static void testSVGPages(GiPaintView *v, GiViewHelper *hlp, NSString *path)
{
    NSString *files[] = { @"page0.svg", @"page1.svg", @"page2.svg", @"page3.svg", nil };
    float x = 10;
    
    for (int i = 0; files[i]; i++) {
        NSString *filename = [path stringByAppendingPathComponent:files[i]];
        UIImage *image = [GiViewHelper getImageFromSVGFile:filename maxSize:CGSizeMake(200, 200)];
        if (!image)
            break;
        
        CGSize size = image.size;
        float y = 20;
        
        UIImageView *imageView = [[UIImageView alloc]initWithImage:image];
        imageView.frame = CGRectMake(x, y, size.width, size.height);
        imageView.layer.borderColor = [UIColor redColor].CGColor;
        imageView.layer.borderWidth = 2;
        [v addSubview:imageView];
        [imageView RELEASE];
        y += size.height + 2;
        
        [hlp createGraphView:CGRectMake(x, y, size.width, size.height) :v];
        if ([hlp loadFromFile:filename]) {
            [GiViewHelper activeView].layer.borderColor = [UIColor blueColor].CGColor;
            [GiViewHelper activeView].layer.borderWidth = 2;
        }
        y += size.height + 2;
        
        image = [[UIImage alloc]initWithContentsOfFile:[GiViewHelper addExtension:filename :@".png"]];
        if (image) {
            imageView = [[UIImageView alloc]initWithImage:image];
            imageView.frame = CGRectMake(x, y, size.width, size.height);
            imageView.layer.borderColor = [UIColor greenColor].CGColor;
            imageView.layer.borderWidth = 2;
            [v addSubview:imageView];
            [imageView RELEASE];
            [image RELEASE];
        }
        y += size.height + 2;
        
        x += size.width + 2;
    }
}

static void testGraphView(GiPaintView *v, int type)
{
    GiViewHelper *hlp = [GiViewHelper sharedInstance:v];
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    if (type & kRandShapes) {
        [hlp addShapesForTest];
    }
    
    switch (type & kCmdMask) {
        case kSplinesCmd:
            hlp.command = @"splines";
            hlp.strokeWidth = 3;
            break;
            
        case kSelectCmd:
            hlp.command = @"select";
            break;
            
        case kSelectLoad:
            [hlp loadFromFile:[GiGraphView2 lastFileName]];
            hlp.command = @"select";
            break;
            
        case kLineCmd:
            hlp.command = @"line";
            hlp.lineStyle = GILineStyleDash;
            break;
            
        case kLinesCmd:
            hlp.command = @"lines";
            hlp.lineStyle = GILineStyleDot;
            hlp.strokeWidth = 5;
            break;
            
        case kHitTestCmd:
            DemoCmdsGate::registerCmds([hlp cmdViewHandle]);
            hlp.command = @"hittest";
            if (!(type & kRandShapes)) {
                [hlp loadFromFile:[GiGraphView2 lastFileName]];
            }
            break;
            
        case kAddImages:
            [hlp insertPNGFromResource:@"app72"];
            [hlp insertPNGFromResource:@"app57" center:CGPointMake(200, 100)];
            [hlp insertImageFromFile:[path stringByAppendingPathComponent:@"page0.png"]];
            break;
            
        case kLoadImages:
            [hlp setImagePath:path];
            [hlp loadFromFile:[GiGraphView2 lastFileName]];
            hlp.command = @"select";
            break;
            
        case kSVGImages:
            [hlp insertSVGFromResource:@"fonts" center:CGPointMake(200, 100)];
            [hlp insertImageFromFile:[path stringByAppendingPathComponent:@"test.svg"]];
            [hlp setImagePath:path];
            break;
            
        case kSVGPages:
            testSVGPages(v, hlp, path);
            break;
    }
}

static void addLargeView1(NSMutableArray *arr, NSUInteger &i, NSUInteger index,
                          NSString* title, CGRect frame, int type)
{
    LargeView1 *view = nil;
    
    if (!arr && index == i++) {
        view = [[LargeView1 alloc]initWithFrame:frame withType:type];
    }
    addView(arr, title, view);
    [view RELEASE];
    
    if (view && type != 0) {
        testGraphView(view.subview2, type);
    }
}

static UIView* addGraphView(NSMutableArray *arr, NSUInteger &i, NSUInteger index,
                            NSString* title, CGRect frame, int type)
{
    UIView *v = nil;
    
    if (!arr && index == i++ && type >= 0) {
        if (type == 0) {
            v = [[GiGraphView1 alloc]initWithFrame:frame];
        }
        else {
            GiGraphView2 *v2 = [[GiGraphView2 alloc]initWithFrame:frame withType:type];
            v = v2;
            testGraphView(v2, type);
        }
    }
    addView(arr, title, v);
    [v RELEASE];
    
    return v;
}

static void testMagnifierView(NSMutableArray *arr, NSUInteger &i, NSUInteger index,
                              NSString* title, CGRect frame, int type)
{
    UIView *wrapview = addGraphView(arr, i, index, title, frame, type);
    
    if (wrapview) {
        CGRect magframe = CGRectMake(10, 10, 200, 200);
        UIView *v = [GiPaintView createMagnifierView:magframe refView:nil parentView:wrapview];
        v.backgroundColor = [UIColor greenColor];
    }
}

static void gatherTestView(NSMutableArray *arr, NSUInteger index, CGRect frame)
{
    NSUInteger i = 0;
    
    addGraphView(arr, i, index, @"Empty view", frame, -1);
    addGraphView(arr, i, index, @"GiGraphView1", frame, 0);
    addLargeView1(arr, i, index, @"GiGraphView1 in large view", frame, 0);
    addGraphView(arr, i, index, @"GiPaintView splines", frame, kSplinesCmd);
    addGraphView(arr, i, index, @"GiPaintView randShapes splines", frame, kSplinesCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView randShapes line", frame, kLineCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView select", frame, kSelectCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView zoom", frame, kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView line", frame, kLineCmd);
    addGraphView(arr, i, index, @"GiPaintView lines", frame, kLinesCmd);
    addGraphView(arr, i, index, @"GiPaintView switch command", frame, kSwitchCmd);
    addGraphView(arr, i, index, @"GiPaintView switch hittest", frame, kSwitchCmd|kHitTestCmd);
    addGraphView(arr, i, index, @"GiPaintView record splines", frame, kRecord|kSplinesCmd);
    addGraphView(arr, i, index, @"GiPaintView record line", frame, kRecord|kLineCmd);
    addGraphView(arr, i, index, @"GiPaintView record randShapes splines",
                 frame, kRecord|kSplinesCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView record randShapes line",
                 frame, kRecord|kLineCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView hittest in democmds", frame, kHitTestCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView add images", frame, kAddImages);
    addGraphView(arr, i, index, @"GiPaintView load images", frame, kLoadImages);
    addGraphView(arr, i, index, @"GiPaintView SVG images", frame, kSVGImages);
    addGraphView(arr, i, index, @"GiPaintView SVG pages", frame, kSVGPages);
    addGraphView(arr, i, index, @"GiPaintView select randShapes", frame, kSelectCmd|kRandShapes);
    addGraphView(arr, i, index, @"GiPaintView select loadShapes", frame, kSelectLoad);
    addLargeView1(arr, i, index, @"GiPaintView in large view", frame, kSplinesCmd);
    addLargeView1(arr, i, index, @"GiPaintView draw in large view", frame, kLineCmd|kRandShapes);
    addLargeView1(arr, i, index, @"GiPaintView SVG pages in large view", frame, kSVGPages);
    testMagnifierView(arr, i, index, @"MagnifierView", frame, kSplinesCmd);
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
