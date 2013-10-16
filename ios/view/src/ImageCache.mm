//! \file ImageCache.mm
//! \brief 实现图像对象缓存类 ImageCache
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "ImageCache.h"

@implementation ImageCache

- (void)dealloc {
    [_images release];
    [_path release];
    [super dealloc];
}

- (id)init {
    self = [super init];
    if (self) {
        _images = [[NSMutableDictionary alloc]init];
    }
    return self;
}

- (void)clearCachedData {
    [_images removeAllObjects];
}

- (NSString *)getName:(UIImage *)image {
    NSArray *arr = [_images allKeysForObject:image];
    return [arr count] > 0 ? [arr objectAtIndex:0] : nil;
}

- (UIImage *)getImage:(NSString *)name {
    return name ? [_images objectForKey:name] : nil;
}

- (UIImage *)loadImage:(NSString *)name {
    UIImage *image = [self getImage:name];
    
    if (!image && name && [name length] > 1) {
        if ([name hasPrefix:@"png:"]) {
            image = [self addPNGFromResource:[name substringFromIndex:4]];
        } else if (_path) {
            image = [self addImageFromFile:[_path stringByAppendingPathComponent:name]];
        }
    }
    
    return image;
}

- (UIImage *)addPNGFromResource:(NSString *)name {
    UIImage *image = [self getImage:name];
    
    if (!image && name && [name length] > 1) {
        NSString *imgname = [name stringByAppendingString:@".png"];
        image = [UIImage imageNamed:imgname];
        if (image && image.size.width > 1) {
            [_images setObject:image forKey:[@"png:" stringByAppendingString:name]];
        } else {
            NSLog(@"Fail to load image resource: %@", imgname);
        }
    }
    
    return image;
}

- (UIImage *)addImageFromFile:(NSString *)filename {
    NSString *name = [[filename lastPathComponent]lowercaseString]; // 无路径的小写文件名
    UIImage *image = [self getImage:name];
    
    if (!image && name && [name length] > 1) {
        image = [[UIImage alloc]initWithContentsOfFile:filename];
        if (image && image.size.width > 1) {
            [_images setObject:image forKey:name];
            [image release];
        } else {
            [image release];
            image = nil;
            NSLog(@"Fail to load image file: %@", filename);
        }
    }
    
    return image;
}

- (void)setImagePath:(NSString *)path {
    [_path release];
    _path = [path retain];
}

@end
