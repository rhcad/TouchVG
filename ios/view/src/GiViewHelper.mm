//! \file GiViewHelper.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiViewHelper.h"
#import "GiGraphView.h"
#include "gicoreview.h"

GiColor CGColorToGiColor(CGColorRef color) {
    int num = CGColorGetNumberOfComponents(color);
    CGColorSpaceModel space = CGColorSpaceGetModel(CGColorGetColorSpace(color));
    const CGFloat *rgba = CGColorGetComponents(color);
    
    if (space == kCGColorSpaceModelMonochrome && num >= 2) {
        unsigned char c = lroundf(rgba[0] * 255.f);
        return GiColor(c, c, c, lroundf(rgba[1] * 255.f));
    }
    if (num < 3) {
        return GiColor::Invalid();
    }
    
    return GiColor(lroundf(rgba[0] * 255.f), lroundf(rgba[1] * 255.f),
                   lroundf(rgba[2] * 255.f), lroundf(CGColorGetAlpha(color) * 255.f));
}

@implementation GiViewHelper

@synthesize command, shapeCount, selectedCount, selectedType, content;
@synthesize lineWidth, strokeWidth, lineColor, lineAlpha;
@synthesize lineStyle, fillColor, fillAlpha;

- (id)init:(GiGraphView *)view {
    self = [super init];
    _view = view;
    return self;
}

+ (id)instance:(GiGraphView *)view {
    return [[[GiViewHelper alloc]init:view]autorelease];
}

+ (GiGraphView *)activeView {
    return [GiGraphView activeView];
}

- (GiGraphView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    _view = [GiGraphView createGraphView:frame :parentView];
    return _view;
}

- (GiGraphView *)createMagnifierView:(CGRect)frame
                              refView:(GiGraphView *)refView
                           parentView:(UIView *)parentView
{
    refView = refView ? refView : [GiGraphView activeView];
    _view = [GiGraphView createMagnifierView:frame refView:refView parentView:parentView];
    return _view;
}

- (int)cmdViewHandle {
    return [_view cmdViewHandle];
}

- (NSString *)command {
    return [NSString stringWithCString:[_view coreView]->getCommand() encoding:NSUTF8StringEncoding];
}

- (void)setCommand:(NSString *)name {
    [_view coreView]->setCommand([_view viewAdapter], [name UTF8String]);
}

- (float)lineWidth {
    float w = [_view coreView]->getContext(false).getLineWidth();
    return w > 1e-5f ? 100.f * w : w;
}

- (void)setLineWidth:(float)value {
    [_view coreView]->getContext(true).setLineWidth(value > 1e-5f ? value / 100.f : value, true);
    [_view coreView]->setContext(kContextLineWidth);
}

- (float)strokeWidth {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [_view coreView]->calcPenWidth(ctx.getLineWidth());
}

- (void)setStrokeWidth:(float)value {
    [_view coreView]->getContext(true).setLineWidth(-fabsf(value), true);
    [_view coreView]->setContext(kContextLineWidth);
}

- (int)lineStyle {
    return [_view coreView]->getContext(false).getLineStyle();
}

- (void)setLineStyle:(int)value {
    [_view coreView]->getContext(true).setLineStyle(value);
    [_view coreView]->setContext(kContextLineStyle);
}

- (UIColor *)GiColorToUIColor:(GiColor)c {
    if (c.a == 0)
        return [UIColor clearColor];
    c.a = 255;
    if (c == GiColor::White())
        return [UIColor whiteColor];
    if (c == GiColor::Black())
        return [UIColor blackColor];
    if (c == GiColor(255, 0, 0))
        return [UIColor redColor];
    if (c == GiColor(0, 255, 0))
        return [UIColor greenColor];
    if (c == GiColor(0, 0, 255))
        return [UIColor blueColor];
    if (c == GiColor(255, 255, 0))
        return [UIColor yellowColor];
    
    return [UIColor colorWithRed:(float)c.r/255.f green:(float)c.g/255.f
                            blue:(float)c.b/255.f alpha:1];
}

- (UIColor *)lineColor {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [self GiColorToUIColor:ctx.getLineColor()];
}

- (void)setLineColor:(UIColor *)value {
    GiColor c = value ? CGColorToGiColor(value.CGColor) : GiColor::Invalid();
    [_view coreView]->getContext(true).setLineColor(c);
    [_view coreView]->setContext(c.a ? kContextLineRGB : kContextLineARGB);
}

- (float)lineAlpha {
    return [_view coreView]->getContext(false).getLineAlpha();
}

- (void)setLineAlpha:(float)value {
    [_view coreView]->getContext(true).setLineAlpha(lroundf(value * 255.f));
    [_view coreView]->setContext(kContextLineAlpha);
}

- (UIColor *)fillColor {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [self GiColorToUIColor:ctx.getFillColor()];
}

- (void)setFillColor:(UIColor *)value {
    GiColor c = value ? CGColorToGiColor(value.CGColor) : GiColor::Invalid();
    [_view coreView]->getContext(true).setFillColor(c);
    [_view coreView]->setContext(c.a ? kContextFillRGB : kContextFillARGB);
}

- (float)fillAlpha {
    return [_view coreView]->getContext(false).getFillAlpha();
}

- (void)setFillAlpha:(float)value {
    [_view coreView]->getContext(true).setFillAlpha(lroundf(value * 255.f));
    [_view coreView]->setContext(kContextFillAlpha);
}

- (void)setContextEditing:(BOOL)editing {
    [_view coreView]->setContextEditing(editing);
}

- (NSString *)content {
    const char* str = [_view coreView]->getContent();
    NSString * ret = [NSString stringWithCString:str encoding:NSUTF8StringEncoding];
    [_view coreView]->freeContent();
    return ret;
}

- (void)setContent:(NSString *)value {
    [_view coreView]->setContent([value UTF8String]);
}

- (int)shapeCount {
    return [_view coreView]->getShapeCount();
}

- (int)selectedCount {
    return [_view coreView]->getSelectedShapeCount();
}

- (int)selectedType {
    return [_view coreView]->getSelectedShapeType();
}

- (BOOL)loadFromFile:(NSString *)vgfile {
    return [_view coreView]->loadFromFile([vgfile UTF8String]);
}

- (BOOL)saveToFile:(NSString *)vgfile {
    BOOL ret = NO;
    NSFileManager *fm = [NSFileManager defaultManager];
    
    if ([_view coreView]->getShapeCount() > 0) {
        if (![fm fileExistsAtPath:vgfile]) {
            [fm createFileAtPath:vgfile contents:[NSData data] attributes:nil];
        }
        ret = [_view coreView]->saveToFile([vgfile UTF8String]);
    } else {
        ret = [fm removeItemAtPath:vgfile error:nil];
    }
    
    return ret;
}

- (UIImage *)snapshot {
    return [_view snapshot];
}

- (BOOL)savePng:(NSString *)filename {
    return [_view savePng:filename];
}

- (BOOL)zoomToExtent {
    return [_view coreView]->zoomToExtent();
}

- (BOOL)zoomToModel:(CGRect)rect {
    return [_view coreView]->zoomToModel(rect.origin.x, rect.origin.y,
                                         rect.size.width, rect.size.height);
}

- (int)addShapesForTest {
    return [_view coreView]->addShapesForTest();
}

@end
