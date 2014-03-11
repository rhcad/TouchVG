//! \file GiGraphView2.mm
//! \brief 实现iOS绘图视图类 GiGraphView2
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphView1.h"
#import "GiViewHelper.h"
#import "GiPlayProvider.h"
#include "RandomShape.h"
#include "gicoreview.h"

static char _lastVgFile[256] = { 0 };

@interface GiGraphView2()<GiPlayProvider>
@end

@implementation GiGraphView2

- (BOOL)exportPNG:(NSString *)filename
{
    NSString *vgfile = [[filename stringByDeletingPathExtension]
                        stringByAppendingPathExtension:@"vg"];
    [[GiViewHelper sharedInstance:self] saveToFile:vgfile];
    [[GiViewHelper sharedInstance] exportSVG:vgfile];
    strncpy(_lastVgFile, [vgfile UTF8String], sizeof(_lastVgFile));
    return [super exportPNG:filename];
}

+ (NSString *)lastFileName
{
    if (_lastVgFile[0] == 0) {
        NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                              NSUserDomainMask, YES) objectAtIndex:0];
        return [path stringByAppendingPathComponent:@"page0.png"];
    }
    
    return [NSString stringWithUTF8String:_lastVgFile];
}

- (id)initWithFrame:(CGRect)frame withType:(int)type {
    self = [super initWithFrame:frame];
    if (self) {
        _testType = type;
    }
    return self;
}

- (void)onFirstRegen:(id)view {
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    if (_testType == kPlayShapes) {
        _pauseBtn = [[UIButton alloc]initWithFrame:CGRectMake(0, 50, 100, 32)];
        _pauseBtn.showsTouchWhenHighlighted = YES;
        _pauseBtn.backgroundColor = [UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:0.8];
        [_pauseBtn setTitleColor:[UIColor blackColor] forState: UIControlStateNormal];
        [_pauseBtn addTarget:self action:@selector(onPause) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_pauseBtn];
        [_pauseBtn RELEASE];
        
        [[GiViewHelper sharedInstance] startPlay:[path stringByAppendingPathComponent:@"record"]];
    }
    else if (_testType & kRecord) {
        [[GiViewHelper sharedInstance] startRecord:[path stringByAppendingPathComponent:@"record"]];
    }
    [[GiViewHelper sharedInstance] startUndoRecord:[path stringByAppendingPathComponent:@"undo"]];
    
    if (_testType & kProvider) {
        //[[GiViewHelper sharedInstance] addPlayProvider:self tag:0];
        [[GiViewHelper sharedInstance] addPlayProvider:self tag:1];
    }
}

- (void)onPlayFrame:(id)view {
    long ticks = [[GiViewHelper sharedInstance] getPlayTicks];
    NSString *text = [NSString stringWithFormat:@"%02ld:%02ld.%03ld",
                      ticks / 60000, ticks / 1000 % 60, ticks % 1000];
    [_pauseBtn setTitle:text forState: UIControlStateNormal];
}

- (void)onPause {
    if ([[GiViewHelper sharedInstance] isPaused]) {
        [[GiViewHelper sharedInstance] playResume];
    } else {
        [[GiViewHelper sharedInstance] playPause];
    }
}

- (BOOL)initProvider:(GiFrame *)frame {
    MgShapes* shapes = MgShapes::fromHandle(frame->shapes);
    BOOL ret = YES;
    
    if (frame->tag > 0) {
        GiCoreView* cv = GiCoreView::createView(NULL, 0);
        NSString* f = [GiViewHelper addExtension:[GiGraphView2 lastFileName] :@".vg"];
        
        ret = cv->loadFromFile([f UTF8String]);
        shapes->copyShapes(MgShapes::fromHandle(cv->backShapes()), false);
        cv->release();
        
        if (_recognizer) {
            Point2d oldpt(shapes->getExtent().center());
            CGPoint newpt = [_recognizer locationInView:self];
            newpt = [[GiViewHelper sharedInstance] displayToModel:newpt];
            shapes->transform(Matrix2d::translation(Point2d(newpt.x, newpt.y) - oldpt));
        }
        Point2d pt(shapes->getExtent().center());
        frame->extra = [NSValue valueWithCGPoint:CGPointMake(pt.x, pt.y)];
    }
    
    return ret;
}

- (int)provideFrame:(GiFrame)frame {
    MgShapes* shapes = MgShapes::fromHandle(frame.shapes);
    int ret = 0;
    
    if (frame.tag == 0) {
        if (frame.tick > frame.lastTick + 1000) {
            shapes->clear();
            RandomParam(frame.tick / 100 % 100).addShapes(shapes);
            ret = 1;
        }
    } else {
        if (frame.tick > frame.lastTick + 60 - frame.tag * 10) {
            CGPoint pt = [frame.extra CGPointValue];
            shapes->transform(Matrix2d::rotation(2.5f * _M_D2R, Point2d(pt.x, pt.y)));
            ret = 1;
        }
    }
    
    return ret;
}

- (void)onProvideEnded:(GiFrame)frame {
    NSLog(@"onPlayEnded tag=%d", frame.tag);
}

- (BOOL)pressHandler:(UILongPressGestureRecognizer *)sender {
    if ([[GiViewHelper sharedInstance] playProviderCount] == 0) {
        return [super pressHandler:sender];
    }
    if (sender.state == UIGestureRecognizerStateBegan) {
        _recognizer = sender;
        GiViewHelper *helper = [GiViewHelper sharedInstance];
        [helper addPlayProvider:self tag:[helper playProviderCount] + 1];
        _recognizer = nil;
    }
    
    return YES;
}

@end
