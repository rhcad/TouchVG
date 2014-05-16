//
//  NSString+Drawing.m
//
//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
//  Copyright (c) 2014 Zhang Yungui <github.com/rhcad>
//

#ifndef __IPHONE_7_0
#import "NSString+Drawing.h"

@implementation NSString (NSStringDrawing)

- (CGSize)sizeWithAttributes:(NSDictionary *)attrs {
    return [self sizeWithFont:attrs[NSFontAttributeName]];
}

- (void)drawAtPoint:(CGPoint)point withAttributes:(NSDictionary *)attrs {
    [self drawAtPoint:point withFont:attrs[NSFontAttributeName]];
}

- (void)drawInRect:(CGRect)rect withAttributes:(NSDictionary *)attrs {
    NSParagraphStyle *paraStyle = attrs[NSParagraphStyleAttributeName];
    
    [self drawInRect:rect withFont:attrs[NSFontAttributeName]
       lineBreakMode:paraStyle.lineBreakMode
           alignment:paraStyle.alignment];
    [self boundingRectWithSize:options:attributes:context:];
}

- (CGRect)boundingRectWithSize:(CGSize)size
                       options:(NSStringDrawingOptions)options
                    attributes:(NSDictionary *)attributes
                       context:(NSStringDrawingContext *)context {
    size = [self sizeWithFont:attrs[NSFontAttributeName] constrainedToSize:size];
    return CGRectMake(0, 0, size.width, size.height);
}

@end
#endif
