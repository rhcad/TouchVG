// NSString+Drawing.m
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "NSString+Drawing.h"

#if !defined(__IPHONE_7_0) || __IPHONE_OS_VERSION_MIN_REQUIRED < __IPHONE_7_0
static bool isIos7(NSString *str) {
    static bool ios7 = false;
#ifdef __IPHONE_7_0
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        ios7 = [str respondsToSelector:@selector(sizeWithAttributes:)];
    });
#endif
    return ios7;
}
#endif

CGSize sizeWithAttributes6(NSString *str, NSDictionary *attrs) {
#if defined(__IPHONE_7_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_7_0
    return [str sizeWithAttributes:attrs];
#else
#ifdef __IPHONE_7_0
    if (isIos7(str)) return [str sizeWithAttributes:attrs];
#endif
    return [str sizeWithFont:attrs[NSFontAttributeName]];
#endif
}

void drawAtPoint6(NSString *str, CGPoint point, NSDictionary *attrs) {
#if defined(__IPHONE_7_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_7_0
    [str drawAtPoint:point withAttributes:attrs];
#else
#ifdef __IPHONE_7_0
    if (isIos7(str)) [str drawAtPoint:point withAttributes:attrs]; else
#endif
        [str drawAtPoint:point withFont:attrs[NSFontAttributeName]];
#endif
}

void drawInRect6(NSString *str, CGRect rect, NSDictionary *attrs) {
#if defined(__IPHONE_7_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_7_0
    [str drawInRect:rect withAttributes:attrs];
#else
#ifdef __IPHONE_7_0
    if (isIos7(str)) { [str drawInRect:rect withAttributes:attrs]; return; }
#endif
    NSParagraphStyle *paraStyle = attrs[NSParagraphStyleAttributeName];
    
    [str drawInRect:rect withFont:attrs[NSFontAttributeName]
      lineBreakMode:paraStyle.lineBreakMode
          alignment:paraStyle.alignment];
#endif
}

CGRect boundingRectWithSize6(NSString *str, CGSize size, NSStringDrawingOptions options,
                             NSDictionary *attrs, NSStringDrawingContext *context) {
#if defined(__IPHONE_7_0) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_7_0
    return [str boundingRectWithSize:size options:options
                          attributes:attrs context:context];
#else
#ifdef __IPHONE_7_0
    if (isIos7(str)) return [str boundingRectWithSize:size options:options
                                           attributes:attrs context:context];
#endif
    size = [str sizeWithFont:attrs[NSFontAttributeName] constrainedToSize:size];
    return CGRectMake(0, 0, size.width, size.height);
#endif
}

NSString *giRoundString(CGFloat num) {
    NSString *str = [NSString stringWithFormat:@"%.5f", num];
    int i = (int)str.length;
    
    while (--i > 0 && [str characterAtIndex:i] == '0') ;
    return [str substringToIndex:[str characterAtIndex:i] == '.' ? i : i+1];
}
