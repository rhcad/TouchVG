// BasicAnimationView.m
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "BasicAnimationView.h"
#import <QuartzCore/QuartzCore.h>

@implementation BasicAnimationView

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    [super touchesBegan:touches withEvent:event];
    
    UITouch *touch = [touches anyObject];
    CGPoint pt = [touch locationInView:touch.view];
    
    float radius = 50.f;
    float newRadius = 100.f;
    
    if (!shapeLayer) {
        shapeLayer = [[CAShapeLayer alloc]init];
        
        shapeLayer.fillColor = nil;
        shapeLayer.strokeColor = [UIColor blackColor].CGColor;
        shapeLayer.lineWidth = 7;
        shapeLayer.bounds = CGRectMake(0, 0, 2 * radius, 2 * radius);
        shapeLayer.path = [UIBezierPath bezierPathWithRect:shapeLayer.bounds].CGPath;
        shapeLayer.position = pt;
        
        [self.layer addSublayer:shapeLayer];
    }
    else {
        CGRect newBounds = CGRectMake(0, 0, 2 * newRadius, 2 * newRadius);
        CGPathRef newPath = NULL;
        CGColorRef fillColor = nil;
        BOOL needReleasePath = NO;
        
        if ((++step) % 3 == 1) {
            newPath = [UIBezierPath bezierPathWithOvalInRect:newBounds].CGPath;
            fillColor = [UIColor clearColor].CGColor;
        }
        else if (step % 3 == 2) {
            newBounds = CGRectMake(pt.x - 2 * radius, pt.y - 2 * radius, 
                                   2 * radius, 2 * radius);
            newPath = [UIBezierPath bezierPathWithOvalInRect:newBounds].CGPath;
            fillColor = [UIColor blueColor].CGColor;
        }
        else {
            CGMutablePathRef path = CGPathCreateMutable();
            CGPathMoveToPoint(path, NULL, 0, 100);
            CGPathAddLineToPoint(path, NULL, 200, 0);
            CGPathAddLineToPoint(path, NULL, 200,200);
            CGPathAddLineToPoint(path, NULL, 0, 100);
            CGPathCloseSubpath(path);
            newPath = path;
            needReleasePath = YES;
            fillColor = [UIColor greenColor].CGColor;
        }
        
        CABasicAnimation* pathAnim = [CABasicAnimation animationWithKeyPath: @"path"];
        pathAnim.toValue = (__bridge id)newPath;
        if (needReleasePath) {
            CGPathRelease(newPath);
            newPath = nil;
        }
        
        CABasicAnimation* boundsAnim = [CABasicAnimation animationWithKeyPath: @"bounds"];
        boundsAnim.toValue = [NSValue valueWithCGRect:newBounds];
        
        CABasicAnimation* fillAnim = [CABasicAnimation animationWithKeyPath: @"fillColor"];
        fillAnim.toValue = (__bridge id)fillColor;
        
        CABasicAnimation* positionAnim = [CABasicAnimation animationWithKeyPath: @"position"];
        positionAnim.toValue = [NSValue valueWithCGPoint:pt];
        
        CAAnimationGroup *anims = [CAAnimationGroup animation];
        anims.animations = [NSArray arrayWithObjects:pathAnim, boundsAnim, fillAnim, positionAnim, nil];
        anims.removedOnCompletion = NO;
        anims.duration = 0.3f;
        anims.fillMode  = kCAFillModeForwards;
        
        [shapeLayer addAnimation:anims forKey:nil];
    }
}

@end
