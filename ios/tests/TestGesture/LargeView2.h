// LargeView2.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class GraphView2;

@interface LargeView2 : UIScrollView<UIScrollViewDelegate> {
    GraphView2 *_subview;
}

- (id)initWithFrame:(CGRect)frame withFlags:(int)t;

@end
