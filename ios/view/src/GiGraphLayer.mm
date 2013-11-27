//! \file GiGraphLayer.mm
//! \brief 实现iOS图形层绘制类 GiGraphLayer
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#import "GiGraphViewImpl.h"
#include "GiCanvasAdapter.h"
#include "gilock.h"
#include <mach/mach_time.h>

@implementation GiGraphLayer

@synthesize frontLayer = _frontLayer;

- (void)freeLayers {
    for (int i = 0; i < sizeof(_layers)/sizeof(_layers[0]); i++) {
        _layers[i] = nil;
    }
}

- (id)initWithAdapter:(GiViewAdapter *)adapter {
    self = [super init];
    if (self) {
        _adapter = adapter;
        _view = _adapter->mainView();
    }
    return self;
}

- (BOOL)inRegenning {
    int i = sizeof(_layers)/sizeof(_layers[0]);
    while (--i >= 0 && _used[i] == 0) ;
    return i >= 0;
}

- (int)pickLayer {
    int i = _adapter->coreView()->isDrawing(_adapter) ? 0 : sizeof(_layers)/sizeof(_layers[0]);
    
    while (--i >= 0) {
        if (_used[i] == 0 && (!_frontLayer || _layers[i] != _frontLayer)) {
            if (giInterlockedIncrement(&_used[i]) != 1) {
                giInterlockedDecrement(&_used[i]);
            }
            else {
                if (!_layers[i]) {
                    _layers[i] = [[CALayer alloc]init];
                    _layers[i].delegate = self;
                }
                _layers[i].frame = _view.bounds;
                _layers[i].position = CGPointMake(_view.bounds.size.width/2, _view.bounds.size.height/2);
                break;
            }
        }
    }
    _needRegen = i < 0 ? _needRegen + 1 : 0;
    if (i < 0) {
        NSLog(@"pickLayer fail");
    }
    
    return i;
}

- (void)regenAll {
    dispatch_async(dispatch_get_global_queue(0, 0), ^{
        int index = [self pickLayer];
        if (index >= 0) {
            _needsClear[index] = YES;
            [_layers[index] setNeedsDisplay];
            [_layers[index] display];
        }
    });
}

- (void)regenAppend {
    if (_frontLayer && ![self inRegenning]) {
        int index = [self pickLayer];
        if (index >= 0) {
            _needsClear[index] = NO;
            [_layers[index] setNeedsDisplay];
            [_layers[index] display];
        }
    } else {
        dispatch_async(dispatch_get_global_queue(0, 0), ^{
            int index = [self pickLayer];
            if (index >= 0) {
                _needsClear[index] = NO;
                [_layers[index] setNeedsDisplay];
                [_layers[index] display];
            }
        });
    }
}

- (void)drawFrontLayer:(CGContextRef)ctx {
    if (_frontLayer) {
        [_frontLayer renderInContext:ctx];
    } else {
        [self regenAll];
    }
}

- (int)machToMs:(uint64_t)start {
    uint64_t elapsedTime = mach_absolute_time() - start;
    static double ticksToNanoseconds = -1.0;
    
    if (ticksToNanoseconds < 0) {
        mach_timebase_info_data_t timebase;
        mach_timebase_info(&timebase);
        ticksToNanoseconds = (double)timebase.numer / timebase.denom * 1e-6;
    }
    
    return (int)(elapsedTime * ticksToNanoseconds);
}

- (void)endPaint:(GiCanvasAdapter*)canvas :(int)n :(uint64_t)start {
    static int order = 0;
    NSLog(@"Regen #%d: n=%d, %d ms", ++order, n, [self machToMs:start]);
    canvas->endPaint();
}

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
    uint64_t start = mach_absolute_time();
    GiCoreView *coreView = _adapter->coreView();
    GiCanvasAdapter canvas(_adapter->imageCache());
    int n = -1;
    
    int index = sizeof(_layers)/sizeof(_layers[0]);
    while (--index >= 0 && _layers[index] != layer) ;
    
    if (_view.window && canvas.beginPaint(ctx)) {
        if (_needsClear[index] || !_frontLayer) {
            CGContextClearRect(ctx, layer.bounds);
            coreView->onSize(_adapter, layer.bounds.size.width, layer.bounds.size.height);
            n = coreView->drawAll(_adapter, &canvas);
        }
        else {
            [_frontLayer renderInContext:ctx];
            n = coreView->drawAppend(_adapter, &canvas);
        }
        [self endPaint:&canvas :n :start];
    }
    
    NSAssert(index >= 0, @"Invalid layer index");
    if (n >= 0) {
        [self swapLayer:layer index:index];
    }
    else if (_needRegen) {
        _needRegen = 0;
        [self performSelector:@selector(regenAll) withObject:nil afterDelay:0.1];
    }
    giInterlockedDecrement(&_used[index]);
}

- (void)swapLayer:(CALayer *)layer index:(int)index {
    giInterlockedIncrement(&_used[index]);
    dispatch_async(dispatch_get_main_queue(), ^{
        _frontLayer = layer;
        [_view setNeedsDisplay];
        [_adapter->getDynView() setNeedsDisplay];
        giInterlockedDecrement(&_used[index]);
        if (_needRegen) {
            _needRegen = 0;
            [self performSelector:@selector(regenAll) withObject:nil afterDelay:0.1];
        }
    });
}

@end
