//! \file GiImageCache.h
//! \brief 定义图像对象缓存类 GiImageCache
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import <UIKit/UIKit.h>
#include "ARCMacro.h"

//! 图像对象缓存类
@interface GiImageCache : NSObject {
    NSMutableDictionary *_images;
    NSMutableDictionary *_spirits;
}

@property (nonatomic, STRONG) NSString  *imagePath; //!< 图像文件的默认路径(可以没有末尾的分隔符)
@property (nonatomic, STRONG) NSString  *playPath;  //!< 播放路径

- (void)clearCachedData;                            //!< 释放临时数据内存
+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size;  //!< 得到SVG文件的图像

- (UIImage *)loadImage:(NSString *)name;            //!< 根据名称查找或加载图像对象
- (CGSize)getImageSize:(NSString *)name;            //!< 根据名称查找图像大小
- (CGSize)addPNGFromResource:(NSString *)name :(NSString **)key; //!< 插入一个程序资源中的图片(name.png)
- (CGSize)addSVGFromResource:(NSString *)name :(NSString **)key; //!< 插入一个程序资源中的图片(name.svg)

- (CGSize)addImageFromPath:(NSString *)path :(NSString**)name;          //!< 在默认位置插入一个图像文件
- (CGSize)addImageFromFile:(NSString *)filename :(NSString **)name;     //!< 在默认位置插入一个图像文件

//! Convert tag$png:prefix%d.png to png:prefixnum.png
- (void)setCurrentImage:(NSString *)spiritName newName:(NSString *)name;

@end
