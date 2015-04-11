// NSString+Drawing.h
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import <UIKit/UIKit.h>
#include <CoreGraphics/CoreGraphics.h>

#ifdef __cplusplus
extern "C" {
#endif

CGSize sizeWithAttributes6(NSString *str, NSDictionary *attrs);
void drawAtPoint6(NSString *str, CGPoint point, NSDictionary *attrs);
void drawInRect6(NSString *str, CGRect rect, NSDictionary *attrs);
CGRect boundingRectWithSize6(NSString *str, CGSize size, NSStringDrawingOptions options,
                             NSDictionary *attrs, NSStringDrawingContext *context);

NSString *giRoundString(CGFloat num);

#ifdef __cplusplus
}
#endif
