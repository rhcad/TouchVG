//! \file GiGraphView.h
//! \brief 定义iOS绘图视图类 GiGraphView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

#ifdef __cplusplus
class GiCoreView;
class GiView;
class MgView;
#endif

//! 绘图消息的观察者协议
/*! \ingroup GROUP_IOS
    \see GiGraphView
 */
@protocol GiGraphViewDelegate <NSObject>
@optional

- (void)onCommandChanged:(id)view;      //!< 当前命令改变的通知
- (void)onSelectionChanged:(id)view;    //!< 图形选择集改变的通知
- (void)onContentChanged:(id)view;      //!< 图形数据改变的通知

@end

//! iOS绘图视图类
/*! \ingroup GROUP_IOS
    \see GiGraphViewDelegate
 */
@interface GiGraphView : UIView

@property(nonatomic, readonly) UIPanGestureRecognizer *panRecognizer;           //!< 拖动手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *tapRecognizer;           //!< 单指点击手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *twoTapsRecognizer;       //!< 单指双击手势识别器
@property(nonatomic, readonly) UILongPressGestureRecognizer *pressRecognizer;   //!< 单指长按手势识别器
@property(nonatomic, readonly) UIPinchGestureRecognizer *pinchRecognizer;       //!< 双指放缩手势识别器
@property(nonatomic, readonly) UIRotationGestureRecognizer *rotationRecognizer; //!< 双指旋转手势识别器
@property(nonatomic)           BOOL gestureEnabled;     //!< 是否允许触摸交互

//! 创建普通图形视图，并添加到父视图、设置为当前视图，不需要额外释放
+ (GiGraphView *)createGraphView:(CGRect)frame :(UIView *)parentView;

//! 创建放大镜视图并添加到父视图，不需要额外释放
+ (GiGraphView *)createMagnifierView:(CGRect)frame
                             refView:(GiGraphView *)refView
                          parentView:(UIView *)parentView;

+ (GiGraphView *)activeView;                //!< 得到当前激活的绘图视图
- (void)activiteView;                       //!< 设置为当前活动视图，触摸时自动调用

#ifdef __cplusplus
- (GiView *)viewAdapter;                    //!< 得到视图适配器对象, GiViewAdapter
- (GiCoreView *)coreView;                   //!< 得到跨平台内核视图
- (MgView *)cmdView;                        //!< 返回内核命令视图
#endif
- (int)cmdViewHandle;                       //!< 返回内核视图的句柄, MgView 指针

- (UIImage *)snapshot;                      //!< 得到静态图形的快照，自动释放
- (BOOL)savePng:(NSString *)filename;       //!< 保存静态图形的快照到PNG文件
- (void)clearCachedData;                    //!< 释放临时数据内存

- (void)addDelegate:(id<GiGraphViewDelegate>)d;     //!< 增加绘图消息观察者
- (void)removeDelegate:(id<GiGraphViewDelegate>)d;  //!< 去掉绘图消息观察者

@end
