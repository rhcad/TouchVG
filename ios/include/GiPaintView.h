//! \file GiPaintView.h
//! \brief 定义iOS绘图视图类 GiPaintView
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiPaintViewDelegate.h"

#ifdef __cplusplus
class GiCoreView;
class GiView;
class GiViewAdapter;
#endif

@class GiImageCache;

//! iOS绘图视图类
/*! \ingroup GROUP_IOS
    \see GiPaintViewDelegate, GiPaintViewXIB
 */
@interface GiPaintView : UIView<UIGestureRecognizerDelegate>

@property(nonatomic, readonly) UIPanGestureRecognizer *panRecognizer;           //!< 拖动手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *tapRecognizer;           //!< 单指点击手势识别器
@property(nonatomic, readonly) UITapGestureRecognizer *twoTapsRecognizer;       //!< 单指双击手势识别器
@property(nonatomic, readonly) UILongPressGestureRecognizer *pressRecognizer;   //!< 单指长按手势识别器
@property(nonatomic, readonly) UIPinchGestureRecognizer *pinchRecognizer;       //!< 双指放缩手势识别器
@property(nonatomic, readonly) UIRotationGestureRecognizer *rotationRecognizer; //!< 双指旋转手势识别器
@property(nonatomic)           BOOL gestureEnabled;     //!< 是否允许触摸交互

@property(nonatomic, readonly) GiImageCache         *imageCache;                //!< 图像对象缓存
@property(nonatomic, readonly) GiPaintView          *mainView;                  //!< 放大镜对应的主视图
@property(nonatomic, assign)   UIView               *viewToMagnify;             //!< 待放大的视图
@property(nonatomic, readonly) NSArray              *delegates;                 //!< GiPaintViewDelegate
@property(nonatomic, assign)   NSInteger            flags; //!< 由 GIViewFlags 按位组成的视图特性标志
@property(nonatomic, assign)   BOOL                 contextActionEnabled;       //!< 是否允许上下文操作

//! 创建普通图形视图，并添加到父视图、设置为当前视图，不需要额外释放
+ (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView :(int)flags;

//! 创建放大镜视图并添加到父视图，不需要额外释放
+ (GiPaintView *)createMagnifierView:(CGRect)frame
                             refView:(GiPaintView *)refView
                          parentView:(UIView *)parentView;

- (id)initWithFrame:(CGRect)frame flags:(int)f;

+ (GiPaintView *)activeView;                //!< 得到当前激活的绘图视图
- (void)activiteView;                       //!< 设置为当前活动视图，触摸时自动调用
- (UIView *)dynamicShapeView:(BOOL)create;  //!< 动态图形视图

- (id<NSLocking>)locker;                    //!< 返回显示锁定对象

#ifdef __cplusplus
- (GiView *)viewAdapter;                    //!< 得到视图适配器对象, GiViewAdapter
- (GiCoreView *)coreView;                   //!< 得到跨平台内核视图
- (GiViewAdapter *)viewAdapter2;            //!< 得到视图适配器对象
#endif
- (long)acquireFrontDoc;                    //!< 获取前端文档句柄
- (void)releaseDoc:(long)doc;               //!< 释放文档句柄

- (UIImage *)snapshot;                      //!< 得到静态图形的快照，自动释放
- (BOOL)exportPNG:(NSString *)filename;     //!< 保存静态图形的快照到PNG文件
- (void)clearCachedData;                    //!< 释放临时数据内存
- (void)tearDown;                           //!< 停止后台任务
- (void)stopRecord:(BOOL)forUndo;           //!< 停止录制图形

- (void)hideContextActions;                 //!< 隐藏上下文按钮
- (IBAction)onContextAction:(id)sender;     //!< 上下文按钮的响应函数

- (void)addDelegate:(id<GiPaintViewDelegate>)d;     //!< 增加绘图消息观察者
- (void)removeDelegate:(id<GiPaintViewDelegate>)d;  //!< 去掉绘图消息观察者
+ (void)addContextAction:(int)action block:(dispatch_block_t)block;

@end

/*! \category GiPaintView(GestureRecognizer)
    \brief GiPaintView 的手势响应函数部分
 */
@interface GiPaintView(GestureRecognizer)

- (void)setupGestureRecognizers;                                //!< 设置手势识别器
- (BOOL)panHandler:(UIPanGestureRecognizer *)sender;            //!< 拖动手势识别响应
- (BOOL)moveHandler:(UIGestureRecognizer *)sender;              //!< 放缩和旋转手势识别响应
- (BOOL)tapHandler:(UITapGestureRecognizer *)sender;            //!< 单指点击手势响应
- (BOOL)twoTapsHandler:(UITapGestureRecognizer *)sender;        //!< 单指双击手势响应
- (BOOL)pressHandler:(UILongPressGestureRecognizer *)sender;    //!< 单指长按手势响应
- (void)delayTap;                                               //!< 延时响应点击手势
- (void)dispatchTapPending;                                     //!< 分发挂起的单击手势

@end

int getTickCount();
