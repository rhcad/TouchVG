//! \file GiViewHelper.h
//! \brief 定义iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@class GiGraphView;

//! iOS绘图视图辅助类
/*! \ingroup GROUP_IOS
 */
@interface GiViewHelper : NSObject {
    GiGraphView *_view;
}

+ (id)instance:(GiGraphView *)view;                 //!< 创建一个自动释放的本类对象

+ (GiGraphView *)activeView;                        //!< 得到当前激活的绘图视图
- (GiGraphView *)createGraphView:(CGRect)frame :(UIView *)parentView;   //!< 创建普通图形视图，并记到本类
- (GiGraphView *)createMagnifierView:(CGRect)frame refView:(GiGraphView *)refView
                           parentView:(UIView *)parentView;  //!< 创建放大镜视图(不需要额外释放)，并记到本类
- (int)cmdViewHandle;                               //!< 返回内核视图的句柄, MgView 指针

@property(nonatomic, assign) NSString   *command;   //!< 当前命令名称
@property (nonatomic)         float     lineWidth;  //!< 线宽，正数表示毫米单位，零表示1像素宽，负数表示像素单位
@property (nonatomic)         float     strokeWidth; //!< 像素单位的线宽，总是为正数
@property (nonatomic)         int       lineStyle;  //!< 线型, 0-5:实线,虚线,点线,点划线,双点划线,空线
@property (nonatomic,assign)  UIColor   *lineColor; //!< 线条颜色，忽略透明度，clearColor或nil表示不画线条
@property (nonatomic)         float     lineAlpha;  //!< 线条透明度, 0-1
@property (nonatomic,assign)  UIColor   *fillColor; //!< 填充颜色，忽略透明度，clearColor或nil表示不填充
@property (nonatomic)         float     fillAlpha;  //!< 填充透明度, 0-1
- (void)setContextEditing:(BOOL)editing;            //!< 线条属性是否正在动态修改

@property(nonatomic, readonly) int shapeCount;      //!< 图形总数
@property(nonatomic, readonly) int selectedCount;   //!< 选中的图形个数
@property(nonatomic, readonly) int selectedType;    //!< 选中的图形的类型, MgShapeType
@property(nonatomic, readonly) int changeCount;     //!< 图形改变次数，可用于检查是否需要保存
@property(nonatomic, assign) NSString *content;     //!< 图形的JSON内容

- (BOOL)loadFromFile:(NSString *)vgfile;    //!< 从JSON文件中加载图形，自动改后缀名为.vg
- (BOOL)saveToFile:(NSString *)vgfile;      //!< 保存图形到JSON文件，自动改后缀名为.vg
- (UIImage *)snapshot;                      //!< 得到静态图形的快照，自动释放
- (BOOL)savePng:(NSString *)filename;       //!< 保存静态图形的快照到PNG文件，自动改后缀名为.png
- (BOOL)exportSVG:(NSString *)filename;     //!< 导出静态图形到SVG文件，自动改后缀名为.svg

- (BOOL)zoomToExtent;                       //!< 放缩显示全部内容
- (BOOL)zoomToModel:(CGRect)rect;           //!< 放缩显示指定范围到视图区域
- (int)addShapesForTest;                    //!< 添加测试图形
- (void)clearCachedData;                    //!< 释放临时数据内存

//! 在默认位置插入一个程序资源中的PNG图片(name.png)
- (int)insertPNGFromResource:(NSString *)name;

//! 插入一个程序资源中的PNG图片(name.png)，并指定图片的中心位置
- (int)insertPNGFromResource:(NSString *)name center:(CGPoint)pt;

//! 在默认位置插入一个程序资源中的SVG图片(name.png)
- (int)insertSVGFromResource:(NSString *)name;

//! 插入一个程序资源中的SVG图片(name.svg)，并指定图片的中心位置
- (int)insertSVGFromResource:(NSString *)name center:(CGPoint)pt;

//! 得到SVG文件的图像
+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size;

//! 在默认位置插入一个PNG、JPEG或SVG等文件的图像
- (int)insertImageFromFile:(NSString *)filename;

//! 设置图像文件的默认路径(可以没有末尾的分隔符)，自动加载时用
- (void)setImagePath:(NSString *)path;

@end
