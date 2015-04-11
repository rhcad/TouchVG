//  GiMagnifierView.m
//  Fix CTM by Zhang Yungui <https://github.com/rhcad/touchvg> based on SimplerMaskTest:
//  http://stackoverflow.com/questions/13330975/how-to-add-a-magnifier-to-custom-control
//  Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiMagnifierView.h"
#import <QuartzCore/QuartzCore.h>

@implementation GiMagnifierView
@synthesize viewToMagnify, touchPoint, followFinger, scale;

- (void)dealloc {
    [super DEALLOC];
}

- (id)initWithFrame:(CGRect)frame {
    CGFloat w = 118;    // =imageWidth:126-(outerRadius:3 + 1)*2
    if (self = [super initWithFrame:CGRectMake(0, 0, w, w)]) {
        self.layer.borderColor = [UIColor clearColor].CGColor;
        self.layer.borderWidth = 3;
        self.layer.cornerRadius = w / 2;
        self.layer.masksToBounds = YES;
        self.opaque = NO;
        self.scale = 1.5f;
    }
    return self;
}

- (void)setTouchPoint:(CGPoint)pt {
    touchPoint = pt;
    if (self.window) {
        CGPoint pt = [self calcCenter];
        
        if (!CGPointEqualToPoint(self.center, pt) && !self.followFinger) {
            [UIView beginAnimations:nil context:nil];
            [UIView setAnimationCurve:UIViewAnimationCurveLinear];
            [UIView setAnimationDuration:0.2];
            self.center = pt;
            [UIView commitAnimations];
        } else {
            self.center = pt;
        }
        [self setNeedsDisplay];
    }
}

- (void)show {
    [self performSelector:@selector(showDelay) withObject:nil afterDelay:0.5];
}

- (void)showDelay {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self.viewToMagnify.superview addSubview:self];
    NSAssert(self.window, @"Fail to add magnifier view");
    self.center = [self calcCenter];
}

- (void)hide {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    [self removeFromSuperview];
}

- (void)drawRect:(CGRect)rect {
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGImageRef mask = [UIImage imageNamed: @"TouchVG.bundle/magnifier-mask.png"].CGImage;
    UIImage *glass = [UIImage imageNamed: @"TouchVG.bundle/magnifier.png"];
    
    CGContextSaveGState(context);
    CGContextClipToMask(context, self.bounds, mask);
    
    CGContextTranslateCTM(context, self.frame.size.width / 2, self.frame.size.height / 2);
    CGContextScaleCTM(context, self.scale, self.scale);
    CGContextTranslateCTM(context, -touchPoint.x, -touchPoint.y);
    [self.viewToMagnify.layer renderInContext:context];
    
    CGContextRestoreGState(context);
    [glass drawInRect:self.bounds];
}

- (float)distanceBetween:(CGPoint)p1 and:(CGPoint)p2 {
    return sqrtf(powf(p2.x-p1.x, 2) + powf(p2.y-p1.y, 2));
}

- (CGPoint)calcCenter {
    if (_topMargin < 1) {
        UIViewController *controller = (UIViewController *)self.viewToMagnify.nextResponder;
        
        if ([controller isKindOfClass:[UIViewController class]]) {
            UINavigationBar *bar = controller.navigationController.navigationBar;
            CGRect rect = [bar convertRect:bar.bounds toView:self.viewToMagnify];
            _topMargin = CGRectGetMaxY(rect);
        }
        _topMargin++;
    }
    
    CGFloat margin = self.frame.size.width / 2 + 5;
    CGRect rect = CGRectInset(self.viewToMagnify.bounds, margin, margin + _topMargin);
    CGPoint pt;
    
    if (self.followFinger) {
        pt = CGPointMake(touchPoint.x, touchPoint.y - 100);
        
        if (pt.x < rect.origin.x) {
            pt.x = rect.origin.x;
        } else if (pt.x > CGRectGetMaxX(rect)) {
            pt.x = CGRectGetMaxX(rect);
        }
        
        if (pt.y < rect.origin.y) {
            pt.y = rect.origin.y;
            if (pt.x < CGRectGetMidX(rect)) {
                pt.x += self.frame.size.width;
            }
            else {
                pt.x -= self.frame.size.width;
            }
        }
    } else {
        pt = [self.superview convertPoint:self.center toView:self.viewToMagnify];
        
        if (!CGRectContainsPoint(CGRectInset(rect, -1, -1), pt)) {
            pt = rect.origin;
        }
        
        CGFloat dist = [self distanceBetween:pt and:touchPoint];
        
        if (dist < margin + 20) {
            if (pt.x < CGRectGetMidX(rect)) {
                pt.x = CGRectGetMaxX(rect);
                if ([self distanceBetween:pt and:touchPoint] < dist) {
                    pt.x = CGRectGetMinX(rect);
                }
            }
            else {
                pt.x = CGRectGetMinX(rect);
                if ([self distanceBetween:pt and:touchPoint] < dist) {
                    pt.x = CGRectGetMaxX(rect);
                }
            }
        }
    }
    
    return [self.superview convertPoint:pt fromView:self.viewToMagnify];
}

@end
