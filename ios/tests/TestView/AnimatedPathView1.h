//! \file AnimatedPathView1.h
//! \brief 定义动画路径测试视图类 AnimatedPathView1
// Copyright (c) 2013, https://github.com/rhcad/touchvg
// 参考了开源项目 https://github.com/ole/Animated-Paths

#import <UIKit/UIKit.h>
#import <QuartzCore/QuartzCore.h>

//! 动画路径测试视图类
@interface AnimatedPathView1 : UIView {
    CAShapeLayer *_pathLayer;
}

@property (nonatomic, retain) CALayer *penLayer;
@property (nonatomic, retain) CALayer *drawingLayer;
@property (nonatomic)   NSInteger shapeIndex;

- (BOOL)setupDrawingLayer:(CALayer *)drawingLayer;
- (BOOL)startAnimation;
- (BOOL)startAnimation:(NSInteger)shapeIndex;

@end
