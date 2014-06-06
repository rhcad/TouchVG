//! \file GiViewHelper.h
//! \brief 定义iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class GiPaintView;
@protocol GiPaintViewDelegate;

typedef NS_ENUM(int, GILineStyle) {
    GILineStyleSolid,       //!< ----------
    GILineStyleDash,        //!< － － － －
    GILineStyleDot,         //!< ..........
    GILineStyleDashDot,     //!< _._._._._
    GILineStyleDashDotdot,  //!< _.._.._.._
    GILineStyleNull         //!< Not draw.
};

//! iOS绘图视图辅助类
/*! \ingroup GROUP_IOS
 */
@interface GiViewHelper : NSObject

+ (GiViewHelper *)sharedInstance;                           //!< 返回单实例对象
+ (GiViewHelper *)sharedInstance:(GiPaintView *)view;       //!< 指定绘图视图
+ (NSString *)version;                              //!< 返回本库的版本号, 1.1.ioslibver.corelibver

+ (GiPaintView *)activeView;                        //!< 得到当前激活的绘图视图
- (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView;   //!< 创建普通图形视图，并记到本类
- (GiPaintView *)createMagnifierView:(CGRect)frame refView:(GiPaintView *)refView
                          parentView:(UIView *)parentView;  //!< 创建放大镜视图(不需要额外释放)，并记到本类
+ (void)removeSubviews:(UIView *)owner;             //!< 关闭视图，用在拥有者的 removeFromSuperview 中
- (long)cmdViewHandle;                              //!< 返回内核视图的句柄, MgView 指针

@property(nonatomic, assign) NSString   *command;   //!< 当前命令名称
@property (nonatomic)         float     lineWidth;  //!< 线宽，正数表示毫米单位，零表示1像素宽，负数表示像素单位
@property (nonatomic)         float     strokeWidth; //!< 像素单位的线宽，总是为正数
@property (nonatomic)       GILineStyle lineStyle;  //!< 线型
@property (nonatomic, assign) UIColor   *lineColor; //!< 线条颜色，忽略透明度，clearColor或nil表示不画线条
@property (nonatomic)         float     lineAlpha;  //!< 线条透明度, 0-1
@property (nonatomic, assign) UIColor   *fillColor; //!< 填充颜色，忽略透明度，clearColor或nil表示不填充
@property (nonatomic)         float     fillAlpha;  //!< 填充透明度, 0-1
- (void)setContextEditing:(BOOL)editing;            //!< 线条属性是否正在动态修改
- (BOOL)setCommand:(NSString *)name withParam:(NSString *)param;    //!< 指定名称和JSON串参数，启动命令
- (BOOL)switchCommand;                              //!< 切换到下一命令
- (BOOL)isCommand:(const char*)name;                //!< 当前是否为指定名称的命令
+ (void)setExtraContextImages:(NSArray *)names;     //!< 设置额外的上下文操作按钮的图像名(name.png)数组

@property(nonatomic, readonly) int shapeCount;      //!< 图形总数
@property(nonatomic, readonly) int selectedCount;   //!< 选中的图形个数
@property(nonatomic, readonly) int selectedType;    //!< 选中的图形的类型, MgShapeType
@property(nonatomic, readonly) int selectedShapeID; //!< 当前选中的图形的ID，选中多个时只取第一个
@property(nonatomic, readonly) long changeCount;    //!< 图形改变次数，可用于检查是否需要保存
@property(nonatomic, readonly) long drawCount;      //!< 显示次数
@property(nonatomic, readonly) CGRect displayExtent; //!< 所有图形的显示范围，视图坐标
@property(nonatomic, readonly) CGRect boundingBox;  //!< 选择包络框，视图坐标
@property(nonatomic, assign) NSString *content;     //!< 图形的JSON内容

- (BOOL)loadFromFile:(NSString *)vgfile readOnly:(BOOL)r;   //!< 从JSON文件中只读加载图形，自动改后缀名为.vg
- (BOOL)loadFromFile:(NSString *)vgfile;    //!< 从JSON文件中加载图形，自动改后缀名为.vg
- (BOOL)saveToFile:(NSString *)vgfile;      //!< 保存图形到JSON文件，自动改后缀名为.vg
- (void)clearShapes;                        //!< 清除所有图形

- (UIImage *)snapshot;                      //!< 得到静态图形的快照，自动释放
- (UIImage *)extentSnapshot:(CGFloat)space; //!< 得到当前显示的静态图形快照，自动去掉周围空白
- (BOOL)exportExtentAsPNG:(NSString *)filename space:(CGFloat)space; //!< 保存当前显示的静态图形快照
- (BOOL)exportPNG:(NSString *)filename;     //!< 保存静态图形的快照到PNG文件，自动改后缀名为.png
- (BOOL)exportSVG:(NSString *)filename;     //!< 导出静态图形到SVG文件，自动改后缀名为.svg

- (BOOL)zoomToExtent;                       //!< 放缩显示全部内容
- (BOOL)zoomToModel:(CGRect)rect;           //!< 放缩显示指定范围到视图区域
- (void)setZoomEnabled:(BOOL)enabled;       //!< 是否允许放缩显示

- (int)addShapesForTest;                    //!< 添加测试图形
- (void)clearCachedData;                    //!< 释放临时数据内存
- (CGPoint)displayToModel:(CGPoint)point;   //!< 视图坐标转为模型坐标
- (CGRect)displayRectToModel:(CGRect)rect;  //!< 视图坐标转为模型坐标
- (CGRect)getShapeBox:(int)sid;             //!< 得到指定ID的图形的包络框显示坐标

- (BOOL)startUndoRecord:(NSString *)path;   //!< 开始Undo录制，在视图初始化或onFirstRegen中调用
- (void)stopUndoRecord;                     //!< 停止Undo录制，在主线程用
- (BOOL)canUndo;                            //!< 能否撤销
- (BOOL)canRedo;                            //!< 能否重做
- (void)undo;                               //!< 撤销
- (void)redo;                               //!< 重做

- (BOOL)isRecording;                        //!< 是否正在录屏
- (BOOL)startRecord:(NSString *)path;       //!< 开始录屏，在视图初始化或onFirstRegen中调用
- (void)stopRecord;                         //!< 停止录屏，在主线程用
- (BOOL)isPaused;                           //!< 是否已暂停
- (BOOL)isPlaying;                          //!< 是否正在播放
- (BOOL)playPause;                          //!< 暂停播放
- (BOOL)playResume;                         //!< 继续播放
- (long)getPlayTicks;                       //!< 返回已播放或录制的毫秒数

- (int)insertPNGFromResource:(NSString *)name;      //!< 在默认位置插入一个程序资源中的PNG图片(name.png)
- (int)insertPNGFromResource:(NSString *)name center:(CGPoint)pt;   //!< 插入PNG图片(name.png)，并指定其中心位置
- (int)insertSVGFromResource:(NSString *)name;      //!< 在默认位置插入一个程序资源中的SVG图片(name.svg)
- (int)insertSVGFromResource:(NSString *)name center:(CGPoint)pt;   //!< 插入一个程序资源中的SVG图片(name.svg)

+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size; //!< 得到SVG文件的图像
+ (NSString *)addExtension:(NSString *)filename :(NSString *)ext;
- (int)insertImageFromFile:(NSString *)filename;    //!< 在默认位置插入一个PNG、JPEG或SVG等文件的图像
- (int)insertImageFromFile:(NSString *)filename center:(CGPoint)pt tag:(int)tag;    //!< 插入一个图像文件

- (BOOL)hasImageShape;                      //!< 返回是否有容纳图像的图形对象
- (int)findShapeByImageID:(NSString *)name; //!< 查找指定名称的图像对应的图形对象ID
- (int)findShapeByTag:(int)tag;             //!< 查找指定Tag的图形对象ID
- (NSArray *)getImageShapes;                //!< 遍历有容纳图像的图形对象
- (void)setImagePath:(NSString *)path;      //!< 设置图像文件的默认路径(可以没有末尾的分隔符)，自动加载时用
- (NSString *)getImagePath;                 //!< 返回图像文件的默认路径

- (void)addDelegate:(id<GiPaintViewDelegate>)d;     //!< 增加绘图消息观察者
- (void)removeDelegate:(id<GiPaintViewDelegate>)d;  //!< 去掉绘图消息观察者

@end
