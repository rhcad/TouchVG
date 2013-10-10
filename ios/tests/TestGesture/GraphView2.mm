// GraphView2.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GraphView2.h"
#include <list>

@interface GraphView2() {
    std::list<CGPoint> _points;
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender;

@end

@implementation GraphView2
@synthesize panGestureRecognizer;

- (id)initWithFrame:(CGRect)frame withFlags:(int)t
{
    self = [super initWithFrame:frame];
    if (self) {
        self.contentMode = UIViewContentModeRedraw; // 不缓存图像，每次重画
        self.backgroundColor = [UIColor whiteColor];
        _flags = t;
        _points.clear();
    }
    if (self && _flags != 0) {
        panGestureRecognizer =
        [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(oneFingerPan:)];
        panGestureRecognizer.maximumNumberOfTouches = 1;
        [self addGestureRecognizer:panGestureRecognizer];
        [panGestureRecognizer release];
    }
    return self;
}

- (void)drawRect:(CGRect)rect
{
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    std::list<CGPoint>::const_iterator it;
    
    CGContextSetRGBFillColor(ctx, 0, 0, 1, 0.2f);
    for (it = _points.begin(); it != _points.end(); ++it) {
        if (it->x < -100 && it->y < -100) {
            continue;
        }
        CGContextFillEllipseInRect(ctx, CGRectMake(it->x - 1.f, it->y - 1.f, 2, 2));
        CGContextFillEllipseInRect(ctx, CGRectMake(it->x - 4.f, it->y - 4.f, 8, 8));
    }
    
    CGContextBeginPath(ctx);
    
    for (it = _points.begin(); it != _points.end(); ++it) {
        if (it->x < -100 && it->y < -100) {
            ++it;
            CGContextMoveToPoint(ctx, it->x, it->y);
        }
        else {
            CGContextAddLineToPoint(ctx, it->x, it->y);
        }
    }
    CGContextSetRGBStrokeColor(ctx, 0, 1, 0, 0.2f);
    CGContextDrawPath(ctx, kCGPathStroke);
    
    CGContextStrokeEllipseInRect(ctx, CGRectMake(_lastpt.x - 50, _lastpt.y - 50, 100, 100));
}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesBegan:touches withEvent:event];
    
    if (_flags < 2) {
        UITouch *touch = [touches anyObject];
        _lastpt = [touch locationInView:touch.view];
        _points.push_back(CGPointMake(-1000, -1000));
        _points.push_back(_lastpt);
        [self setNeedsDisplay];
    }
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesMoved:touches withEvent:event];
    
    if (_flags < 2) {
        UITouch *touch = [touches anyObject];
        _lastpt = [touch locationInView:touch.view];
        _points.push_back(_lastpt);
        [self setNeedsDisplay];
    }
}

- (void)oneFingerPan:(UIPanGestureRecognizer *)sender
{
    if (sender.state == UIGestureRecognizerStateBegan && _flags == 1) {
        _lastpt = [sender locationInView:sender.view];
        _points.push_back(CGPointMake(-1000, -1000));
        _points.push_back(_lastpt);
        [self setNeedsDisplay];
    }
    else if (sender.state == UIGestureRecognizerStateChanged) {
        _lastpt = [sender locationInView:sender.view];
        _points.push_back(_lastpt);
        [self setNeedsDisplay];
    }
}

@end
