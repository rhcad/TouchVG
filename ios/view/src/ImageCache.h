//! \file ImageCache.h
//! \brief 定义图像对象缓存类 ImageCache
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 图像对象缓存类
@interface ImageCache : NSObject {
    NSString            *_path;
    NSMutableDictionary *_images;
}

- (void)clearCachedData;                            //!< 释放临时数据内存
- (UIImage *)loadImage:(NSString *)name;            //!< 根据名称查找或加载图像对象
- (CGSize)getImageSize:(NSString *)name;            //!< 根据名称查找图像大小
- (CGSize)addPNGFromResource:(NSString *)name :(NSString **)key; //!< 插入一个程序资源中的图片(name.png)
- (CGSize)addSVGFromResource:(NSString *)name :(NSString **)key; //!< 插入一个程序资源中的图片(name.svg)
- (CGSize)addImageFromFile:(NSString *)filename :(NSString **)name;     //!< 在默认位置插入一个图像文件
- (void)setImagePath:(NSString *)path;              //!< 设置图像文件的默认路径(可以没有末尾的分隔符)，自动加载时用

@end
