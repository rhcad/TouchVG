// LargeView1.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class GiGraphView1;
@class GiPaintView;

//! 测试内嵌一个绘图视图的长幅滚动视图类
@interface LargeView1 : UIScrollView<UIScrollViewDelegate> {
    GiGraphView1    *_subview1;
    GiPaintView    *_subview2;
}

@property(nonatomic, readonly) GiPaintView  *subview2;

- (id)initWithFrame:(CGRect)frame withType:(int)type;
- (BOOL)exportPNG:(NSString *)filename;

@end
