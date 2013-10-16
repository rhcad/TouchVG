//! \file ImageCache.h
//! \brief 定义图像对象缓存类 ImageCache
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 图像对象缓存类
/*! \ingroup GROUP_IOS
 */
@interface ImageCache : NSObject {
    NSString            *_path;
    NSMutableDictionary *_images;
}

- (void)clearCachedData;                            //!< 释放临时数据内存

- (NSString *)getName:(UIImage *)image;             //!< 得到图像对应的标识名称
- (UIImage *)getImage:(NSString *)name;             //!< 根据名称查找图像对象
- (UIImage *)loadImage:(NSString *)name;            //!< 根据名称查找或加载图像对象
- (UIImage *)addPNGFromResource:(NSString *)name;   //!< 在默认位置插入一个程序资源中的图片(name.png)
- (UIImage *)addImageFromFile:(NSString *)filename; //!< 在默认位置插入一个PNG、JPEG或SVG等文件的图像
- (void)setImagePath:(NSString *)path;              //!< 设置图像文件的默认路径(可以没有末尾的分隔符)，自动加载时用

@end
