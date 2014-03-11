//! \file GiPaintView.h
//! \brief 定义iOS绘图视图类 GiPaintView
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

#ifdef __cplusplus
class GiCoreView;
class GiView;
class GiViewAdapter;
#endif

//! 绘图消息的观察者协议
/*! 也可以在 GiPaintView 派生类直接实现这些通知函数。
    \ingroup GROUP_IOS
    \see GiPaintView
 */
@protocol GiPaintViewDelegate <NSObject>
@optional

- (void)onCommandChanged:(id)view;      //!< 当前命令改变的通知
- (void)onSelectionChanged:(id)view;    //!< 图形选择集改变的通知
- (void)onContentChanged:(id)view;      //!< 图形数据改变的通知
- (void)onDynamicChanged:(id)view;      //!< 图形动态改变的通知
- (void)onFirstRegen:(id)view;          //!< 第一次后台渲染结束的通知
- (void)onPlayFrame:(id)view;           //!< 播放一帧的通知
- (void)onPlayWillEnd:(id)view;         //!< 播放完成，待用户结束播放
- (void)onPlayEnded:(id)view;           //!< 播放结束的通知

@end

@class ImageCache;

//! iOS绘图视图类
/*! \ingroup GROUP_IOS
    \see GiPaintViewDelegate
 */
@interface GiPaintView : UIView

@property(nonatomic, readonly) UIPanGestureRecognizer *panRecognizer;           //!< 拖动手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *tapRecognizer;           //!< 单指点击手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *twoTapsRecognizer;       //!< 单指双击手势识别器
@property(nonatomic, readonly) UILongPressGestureRecognizer *pressRecognizer;   //!< 单指长按手势识别器
@property(nonatomic, readonly) UIPinchGestureRecognizer *pinchRecognizer;       //!< 双指放缩手势识别器
@property(nonatomic, readonly) UIRotationGestureRecognizer *rotationRecognizer; //!< 双指旋转手势识别器
@property(nonatomic)           BOOL gestureEnabled;     //!< 是否允许触摸交互

@property(nonatomic, readonly) ImageCache           *imageCache;                //!< 图像对象缓存
@property(nonatomic, readonly) GiPaintView          *mainView;                  //!< 放大镜对应的主视图

//! 创建普通图形视图，并添加到父视图、设置为当前视图，不需要额外释放
+ (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView;

//! 创建放大镜视图并添加到父视图，不需要额外释放
+ (GiPaintView *)createMagnifierView:(CGRect)frame
                             refView:(GiPaintView *)refView
                          parentView:(UIView *)parentView;

+ (GiPaintView *)activeView;                //!< 得到当前激活的绘图视图
- (void)activiteView;                       //!< 设置为当前活动视图，触摸时自动调用
- (UIView *)dynamicShapeView;               //!< 动态图形视图

#ifdef __cplusplus
- (GiView *)viewAdapter;                    //!< 得到视图适配器对象, GiViewAdapter
- (GiCoreView *)coreView;                   //!< 得到跨平台内核视图
- (GiViewAdapter *)viewAdapter2;            //!< 得到视图适配器对象
#endif

- (UIImage *)snapshot;                      //!< 得到静态图形的快照，自动释放
- (BOOL)exportPNG:(NSString *)filename;     //!< 保存静态图形的快照到PNG文件
- (void)clearCachedData;                    //!< 释放临时数据内存
- (void)tearDown;                           //!< 停止后台任务
- (void)stopRecord:(BOOL)forUndo;           //!< 停止录制图形

- (void)setContextActionEnabled:(BOOL)en;   //!< 是否允许上下文操作
- (void)hideContextActions;                 //!< 隐藏上下文按钮
- (IBAction)onContextAction:(id)sender;     //!< 上下文按钮的响应函数

- (void)addDelegate:(id<GiPaintViewDelegate>)d;     //!< 增加绘图消息观察者
- (void)removeDelegate:(id<GiPaintViewDelegate>)d;  //!< 去掉绘图消息观察者

@end

/*! \category GiPaintView(GestureRecognizer)
    \brief GiPaintView 的手势响应函数部分
 */
@interface GiPaintView(GestureRecognizer)

- (void)setupGestureRecognizers;                                //!< 设置手势识别器
- (BOOL)panHandler:(UIGestureRecognizer *)sender;               //!< 拖动手势识别响应
- (BOOL)tapHandler:(UITapGestureRecognizer *)sender;            //!< 单指点击手势响应
- (BOOL)twoTapsHandler:(UITapGestureRecognizer *)sender;        //!< 单指双击手势响应
- (BOOL)pressHandler:(UILongPressGestureRecognizer *)sender;    //!< 单指长按手势响应
- (void)delayTap;                                               //!< 延时响应点击手势
- (void)dispatchTapPending;                                     //!< 分发挂起的单击手势

@end
