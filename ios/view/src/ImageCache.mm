//! \file ImageCache.mm
//! \brief 实现图像对象缓存类 ImageCache
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "ImageCache.h"
#import "SVGKImage.h"

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

- (UIImage *)loadImage:(NSString *)name {
    UIImage *image = name ? [_images objectForKey:name] : nil;
    
    if (!image && name && [name length] > 1) {
        if ([name hasPrefix:@"png:"]) {
            [self addPNGFromResource:[name substringFromIndex:4] :&name];
        }
        else if ([name hasPrefix:@"svg:"]) {
            [self addSVGFromResource:[name substringFromIndex:4] :&name];
        }
        else if (_path) {
            [self addImageFromFile:[_path stringByAppendingPathComponent:name] :&name];
        }
        image = [_images objectForKey:name];
    }
    
    return image;
}

- (CGSize)getImageSize:(NSString *)name {
    UIImage *image = name ? [_images objectForKey:name] : nil;
    return image ? image.size : CGSizeZero;
}

- (CGSize)addPNGFromResource:(NSString*)name :(NSString **)key {
    name = [name stringByDeletingPathExtension];
    *key = [@"png:" stringByAppendingString:name];
    CGSize size = [self getImageSize:*key];
    
    if (size.width < 1 && name && [name length] > 1) {
        NSString *resname = [name stringByAppendingString:@".png"];
        UIImage *image = [UIImage imageNamed:resname];
        if (image && image.size.width > 1) {
            [_images setObject:image forKey:*key];
            size = image.size;
        } else {
            NSLog(@"Fail to load image resource: %@", resname);
        }
    }
    
    return size;
}

// TODO: 暂时直接从SVGKImage得到UIImage，应当在GiCanvasAdapter中从SVGKImage获取CALayer渲染
- (CGSize)addSVGFromResource:(NSString *)name :(NSString **)key {
    name = [name stringByDeletingPathExtension];
    *key = [@"svg:" stringByAppendingString:name];
    CGSize size = [self getImageSize:*key];
    
    if (size.width < 1 && name && [name length] > 1) {
        NSString *resname = [name stringByAppendingString:@".svg"];
        UIImage *image = [[SVGKImage imageNamed:resname] UIImage];
        if (image && image.size.width > 1) {
            [_images setObject:image forKey:*key];
            size = image.size;
        } else {
            NSLog(@"Fail to load image resource: %@", resname);
        }
    }
    
    return size;
}

// TODO: 暂时直接从SVGKImage得到UIImage，应当在GiCanvasAdapter中从SVGKImage获取CALayer渲染
- (CGSize)addImageFromFile:(NSString *)filename :(NSString**)name {
    *name = [[filename lastPathComponent]lowercaseString]; // 无路径的小写文件名
    CGSize size = [self getImageSize:*name];
    
    if (size.width < 1 && filename && [filename length] > 1) {
        UIImage *image;
        
        if ([*name hasPrefix:@"svg:"]) {
            image = [[SVGKImage imageWithContentsOfFile:filename] UIImage];
        }
        else {
            image = [[UIImage alloc]initWithContentsOfFile:filename];
        }
        if (image && image.size.width > 1) {
            [_images setObject:image forKey:*name];
            [image release];
            size = image.size;
        } else {
            [image release];
            NSLog(@"Fail to load image file: %@", filename);
        }
    }
    
    return size;
}

- (void)setImagePath:(NSString *)path {
    [_path release];
    _path = [path retain];
}

@end
