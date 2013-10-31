//! \file AnimatedPathView1.mm
//! \brief 实现动画路径测试视图类 AnimatedPathView1
// Copyright (c) 2013, https://github.com/rhcad/touchvg
// 参考了开源项目 https://github.com/ole/Animated-Paths

#import "AnimatedPathView1.h"

@implementation AnimatedPathView1
@synthesize penLayer = _penLayer;
@synthesize drawingLayer = _drawingLayer;
@synthesize shapeIndex = _shapeIndex;

- (BOOL)setupDrawingLayer:(CALayer *)drawingLayer {
    if (self.drawingLayer) {
        [self.penLayer removeFromSuperlayer];
        [self.drawingLayer removeFromSuperlayer];
        self.drawingLayer = nil;
        self.penLayer = nil;
    }
    if (!drawingLayer) {
        return NO;
    }
    
    [self.layer addSublayer:drawingLayer];
    self.drawingLayer = drawingLayer;
    
    UIImage *penImage = [UIImage imageNamed:@"pencil.png"];
    CALayer *penLayer = [CALayer layer];
    penLayer.contents = (id)penImage.CGImage;
    penLayer.anchorPoint = CGPointMake(0.0f, 1.0f);
    penLayer.frame = CGRectMake(0.0f, 0.0f, penImage.size.width, penImage.size.height);
    
    [self.layer addSublayer:penLayer];
    self.penLayer = penLayer;
    
    return YES;
}

- (BOOL)startAnimation {
    if (!self.penLayer)
        return NO;
    return [self startAnimation:0];
}

- (BOOL)startAnimation:(NSInteger)shapeIndex {
    [_pathLayer removeAllAnimations];
    [self.penLayer removeAllAnimations];
    [self.penLayer removeFromSuperlayer];
    
    if (shapeIndex >= [self.drawingLayer.sublayers count]) {
        return NO;
    }
    
    CALayer *shapeLayer = [self.drawingLayer.sublayers objectAtIndex:shapeIndex];
    
    _pathLayer = [shapeLayer.sublayers objectAtIndex:0];
    [_pathLayer addSublayer:self.penLayer];
    
    CABasicAnimation *pathAnimation = [CABasicAnimation animationWithKeyPath:@"strokeEnd"];
    pathAnimation.duration = 1.0;
    pathAnimation.fromValue = [NSNumber numberWithFloat:0.0f];
    pathAnimation.toValue = [NSNumber numberWithFloat:1.0f];
    [_pathLayer addAnimation:pathAnimation forKey:@"strokeEnd"];
    
    CAKeyframeAnimation *penAnimation = [CAKeyframeAnimation animationWithKeyPath:@"position"];
    penAnimation.duration = 1.0;
    penAnimation.path = _pathLayer.path;
    penAnimation.calculationMode = kCAAnimationPaced;
    penAnimation.delegate = self;
    [self.penLayer addAnimation:penAnimation forKey:@"position"];
    
    return YES;
}

- (void)animationDidStart:(CAAnimation *)anim {
    _pathLayer.hidden = NO;
}

- (void) animationDidStop:(CAAnimation *)anim finished:(BOOL)flag {
    self.shapeIndex++;
    if (![self startAnimation:self.shapeIndex]) {
        [_pathLayer removeAllAnimations];
        [self.penLayer removeAllAnimations];
        [self.penLayer removeFromSuperlayer];
    }
}

@end
