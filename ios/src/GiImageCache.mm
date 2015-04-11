//! \file GiImageCache.mm
//! \brief 实现图像对象缓存类 GiImageCache
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiImageCache.h"
#ifdef USE_SVGKIT
#import "SVGKImage.h"
#endif

@implementation GiImageCache

@synthesize imagePath;
@synthesize playPath;

- (id)init {
    self = [super init];
    if (self) {
        _images = [[NSMutableDictionary alloc]init];
    }
    return self;
}

- (void)dealloc {
    [_images RELEASEOBJ];
    [_spirits RELEASEOBJ];
    [super DEALLOC];
}

- (void)clearCachedData {
    [_images removeAllObjects];
}

- (void)setCurrentImage:(NSString *)spiritName newName:(NSString *)name {
    if (!_spirits) {
        _spirits = [[NSMutableDictionary alloc]init];
    }
    if (name) {
        if ([name rangeOfString:@":"].location == NSNotFound) {
            name = [@"png:" stringByAppendingString:name];
        }
        if (![name isEqualToString:[_spirits objectForKey:name]]) {
            [_spirits setObject:name forKey:spiritName];
        }
    } else {
        [_spirits removeObjectForKey:spiritName];
    }
}

- (UIImage *)loadImage:(NSString *)name {
    if (name && [name rangeOfString:@"%d."].location != NSNotFound) {   // tag$png:prefix%d.png
        name = [_spirits objectForKey:name];
    }
    
    UIImage *image = name ? [_images objectForKey:name] : nil;
    
    if (!image && name && [name length] > 1) {
        image = [UIImage imageNamed:name];
        if (image) {
            [_images setObject:image forKey:name];
            return image;
        }
        
        if ([name hasPrefix:@"png:"]) {
            [self addPNGFromResource:[name substringFromIndex:4] :&name];
        }
        else if ([name hasPrefix:@"svg:"]) {
            [self addSVGFromResource:[name substringFromIndex:4] :&name];
        }
        else {
            if ([self addImageFromPath:self.playPath :&name].width < 1) {
                [self addImageFromPath:self.imagePath :&name];
            }
        }
        image = [_images objectForKey:name];
    }
    
    return image;
}

- (CGSize)addImageFromPath:(NSString *)path :(NSString**)name {
    path = path ? [path stringByAppendingPathComponent:*name] : path;
    return path ? [self addImageFromFile:path :name] : CGSizeZero;
}

- (CGSize)getImageSize:(NSString *)name {
    UIImage *image = name ? [_images objectForKey:name] : nil;
    return image ? image.size : CGSizeZero;
}

- (CGSize)addPNGFromResource:(NSString*)name :(NSString **)key {
    name = [name stringByDeletingPathExtension];
    *key = [@"png:" stringByAppendingString:name];
    CGSize size = [self getImageSize:*key];
    
    if (size.width < 1 && *key && [name length] > 1) {
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

#ifdef USE_SVGKIT
+ (UIImage *)renderSVG:(SVGKImage*)svgimg {
    if (!svgimg || ![svgimg hasSize])
        return nil;
    
    UIGraphicsBeginImageContextWithOptions(svgimg.size, NO, 0);
    [svgimg.CALayerTree renderInContext:UIGraphicsGetCurrentContext()];
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}
#endif

- (CGSize)addSVGFromResource:(NSString *)name :(NSString **)key {
    name = [name stringByDeletingPathExtension];
    *key = [@"svg:" stringByAppendingString:name];
    CGSize size = [self getImageSize:*key];
    
    if (size.width < 1 && *key && [name length] > 1) {
#ifdef USE_SVGKIT
        NSString *resname = [name stringByAppendingString:@".svg"];
        @try {
            UIImage *image = [GiImageCache renderSVG:[SVGKImage imageNamed:resname]];
            
            if (image && image.size.width > 1) {
                [_images setObject:image forKey:*key];
                size = image.size;
            } else {
                NSLog(@"Fail to load image resource: %@", resname);
            }
        }
        @catch (NSException *e) {
            NSLog(@"Fail to parse %@ due to %@", resname, [e name]);
        }
#endif
    }
    
    return size;
}

+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size {
    UIImage *image = nil;
#ifdef USE_SVGKIT
    SVGKImage* svgimg = nil;
    
    @try {
        svgimg = [[SVGKImage alloc]initWithContentsOfFile:filename];
        image = [GiImageCache renderSVG:svgimg];
    }
    @catch (NSException *e) {
        NSLog(@"Fail to parse SVG file due to %@: %@", [e name], filename);
    }
    
    if (image && (svgimg.size.width > size.width || svgimg.size.height > size.height)) {
        [svgimg scaleToFitInside:size];
        
        UIGraphicsBeginImageContextWithOptions(svgimg.size, NO, image.scale);
        [image drawInRect:CGRectMake(0, 0, svgimg.size.width, svgimg.size.height)];
        image = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
    }
    [svgimg RELEASEOBJ];
#endif
    
    return image;
}

// TODO: 暂时直接从SVGKImage得到UIImage，应当在GiCanvasAdapter中从SVGKImage获取CALayer渲染
- (CGSize)addImageFromFile:(NSString *)filename :(NSString**)name {
    *name = [[filename lastPathComponent]lowercaseString]; // 无路径的小写文件名
    CGSize size = [self getImageSize:*name];
    
    if (size.width < 1 && *name && [filename length] > 1) {
        UIImage *image = nil;
        
        if ([*name hasPrefix:@"svg:"]) {
#ifdef USE_SVGKIT
            SVGKImage* svgimg = nil;
            @try {
                svgimg = [[SVGKImage alloc]initWithContentsOfFile:filename];
                image = [GiImageCache renderSVG:svgimg];
            }
            @catch (NSException *e) {
                NSLog(@"Fail to parse SVG file due to %@: %@", [e name], filename);
            }
            if (image && image.size.width > 1) {
                [_images setObject:image forKey:*name];
                size = image.size;
            }
            [svgimg RELEASEOBJ];
#endif
        }
        else {
            image = [[UIImage alloc]initWithContentsOfFile:filename];
            if (image && image.size.width > 1) {
                [_images setObject:image forKey:*name];
                [image RELEASEOBJ];
                size = image.size;
            }
        }
    }
    if (size.width < 1) {
        NSLog(@"Fail to load image file: %@", filename);
    }
    
    return size;
}

@end
