//! \file GiPaintViewXIB.h
//! \brief 定义可在XIB中使用的绘图视图类 GiPaintViewXIB
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiPaintView.h"

@class GiViewHelper;

//! 可在XIB和StoryBoard中指定属性的的绘图视图类
/*! \ingroup GROUP_IOS
 */
@interface GiPaintViewXIB : GiPaintView

//@property BOOL        gestureEnabled;         // 是否允许触摸交互
//@property NSInteger   flags;                  // 由 GIViewFlags 按位组成的视图特性标志
//@property BOOL        contextActionEnabled;   // 是否允许上下文操作

//! 专用于此视图的绘图辅助对象
@property(nonatomic, readonly)  GiViewHelper    *helper;

//! 当前命令名称
@property(nonatomic, assign)    NSString        *command;

//! 线宽，正数表示毫米单位，零表示1像素宽，负数表示像素单位
@property (nonatomic)           CGFloat         lineWidth;

//! 像素单位的浮点线宽，总是为正数
@property (nonatomic)           CGFloat         strokeWidth;

//! 线型: solid,dash,dot,dashdot,dashdotdot,null
@property (nonatomic, assign)   NSString        *lineStyle;

//! 线条颜色，忽略透明度，clearColor或nil表示不画线条
@property (nonatomic, assign)   UIColor         *lineColor;

//! 线条透明度, 0-1
@property (nonatomic)           CGFloat         lineAlpha;

//! 填充颜色，忽略透明度，clearColor或nil表示不填充
@property (nonatomic, assign)   UIColor         *fillColor;

//! 填充透明度, 0-1
@property (nonatomic)           CGFloat         fillAlpha;

//! 绘图命令选项
@property (nonatomic, assign)   NSDictionary    *options;

//! 图形的JSON内容
@property(nonatomic, assign)    NSString        *content;

//! SVG路径的d坐标序列
@property(nonatomic, assign)    NSString        *path;

//! 获取全部图形范围，模型坐标
@property (nonatomic, readonly) CGRect          extent;

//! 当前显示范围，模型坐标
@property(nonatomic)            CGRect          zoomExtent;

//! 显示比例
@property (nonatomic)           CGFloat         viewScale;

//! 视图中心的世界坐标，毫米单位
@property (nonatomic)           CGPoint         viewCenter;

//! 模型单位到世界坐标系的比例
@property (nonatomic)           CGSize          modelScale;

//! 是否允许放缩显示
@property(nonatomic)            BOOL            zoomEnabled;

@end
