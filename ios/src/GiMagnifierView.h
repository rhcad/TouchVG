// GiMagnifierView.h
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License


#import <UIKit/UIKit.h>
#include "ARCMacro.h"

//! 放大镜图形视图类
@interface GiMagnifierView : UIView {
    CGFloat _topMargin;
}

@property (nonatomic, WEAK) UIView *viewToMagnify;
@property (nonatomic) CGPoint touchPoint;
@property (nonatomic) BOOL followFinger;
@property (nonatomic) CGFloat scale;

- (void)show;
- (void)hide;

@end
