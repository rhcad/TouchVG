//! \file GiViewHelper.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <QuartzCore/QuartzCore.h>
#import "GiViewHelper.h"
#import "GiGraphView.h"
#import "ImageCache.h"
#include "GiShapeAdapter.h"
#include "gicoreview.h"

#define IOSLIBVERSION     50

GiColor CGColorToGiColor(CGColorRef color) {
    size_t num = CGColorGetNumberOfComponents(color);
    CGColorSpaceModel space = CGColorSpaceGetModel(CGColorGetColorSpace(color));
    const CGFloat *rgba = CGColorGetComponents(color);
    
    if (space == kCGColorSpaceModelMonochrome && num >= 2) {
        int c = (int)lroundf(rgba[0] * 255.f);
        return GiColor(c, c, c, (int)lroundf(rgba[1] * 255.f));
    }
    if (num < 3) {
        return GiColor::Invalid();
    }
    
    return GiColor((int)lroundf(rgba[0] * 255.f),
                   (int)lroundf(rgba[1] * 255.f),
                   (int)lroundf(rgba[2] * 255.f),
                   (int)lroundf(CGColorGetAlpha(color) * 255.f));
}

@implementation GiViewHelper

@synthesize shapeCount, selectedCount, selectedType, selectedShapeID, content, changeCount;
@synthesize command, lineWidth, strokeWidth, lineColor, lineAlpha;
@synthesize lineStyle, fillColor, fillAlpha;

+ (NSString *)version {
    return [NSString stringWithFormat:@"1.0.%d.%d", IOSLIBVERSION, GiCoreView::getVersion()];
}

- (id)initWithView:(GiGraphView *)view {
    self = [super init];
    _view = view ? view : [GiGraphView activeView];
    return self;
}

+ (id)instance:(GiGraphView *)view {
    return [[GiViewHelper alloc]initWithView:view];
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

- (long)cmdViewHandle {
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
    return [_view coreView]->calcPenWidth([_view viewAdapter], ctx.getLineWidth());
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
    [_view coreView]->getContext(true).setLineAlpha((int)lroundf(value * 255.f));
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
    [_view coreView]->getContext(true).setFillAlpha((int)lroundf(value * 255.f));
    [_view coreView]->setContext(kContextFillAlpha);
}

- (void)setContextEditing:(BOOL)editing {
    [_view coreView]->setContextEditing(editing);
}

- (NSString *)content {
    long hDoc = [_view coreView]->acquireFrontDoc();
    const char* str = [_view coreView]->getContent(hDoc);
    NSString * ret = [NSString stringWithCString:str encoding:NSUTF8StringEncoding];
    [_view coreView]->freeContent();
    [_view coreView]->releaseDoc(hDoc);
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

- (int)selectedShapeID {
    return [_view coreView]->getSelectedShapeID();
}

- (long)changeCount {
    return [_view coreView]->getChangeCount();
}

- (NSString *)addExtension:(NSString *)filename :(NSString *)ext {
    if (filename && ![filename hasSuffix:ext]) {
        filename = [[filename stringByDeletingPathExtension]
                    stringByAppendingPathExtension:ext];
    }
    return filename;
}

- (BOOL)loadFromFile:(NSString *)vgfile readOnly:(BOOL)r {
    return [_view coreView]->loadFromFile([[self addExtension:vgfile :@"vg"] UTF8String], r); 
}

- (BOOL)loadFromFile:(NSString *)vgfile {
    return [_view coreView]->loadFromFile([[self addExtension:vgfile :@"vg"] UTF8String]);
}

- (BOOL)saveToFile:(NSString *)vgfile {
    BOOL ret = NO;
    NSFileManager *fm = [NSFileManager defaultManager];
    
    vgfile = [self addExtension:vgfile :@"vg"];
    if ([_view coreView]->getShapeCount() > 0) {
        if (![fm fileExistsAtPath:vgfile]) {
            [fm createFileAtPath:vgfile contents:[NSData data] attributes:nil];
        }
        long hDoc = [_view coreView]->acquireFrontDoc();
        ret = [_view coreView]->saveToFile(hDoc, [vgfile UTF8String]);
        [_view coreView]->releaseDoc(hDoc);
    } else {
        ret = [fm removeItemAtPath:vgfile error:nil];
    }
    
    return ret;
}

- (void)clearShapes {
    [_view coreView]->clear();
}

- (UIImage *)snapshot {
    return [_view snapshot];
}

- (BOOL)savePng:(NSString *)filename {
    return [_view savePng:[self addExtension:filename :@".png"]];
}

- (BOOL)exportSVG:(NSString *)filename {
    return NO;//[_view coreView]->exportSVG([[self addExtension:filename :@".svg"] UTF8String]);
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

- (void)clearCachedData {
    [_view clearCachedData];
}

- (int)insertPNGFromResource:(NSString *)name {
    CGSize size = [_view.imageCache addPNGFromResource:name :&name];
    return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
}

- (int)insertPNGFromResource:(NSString *)name center:(CGPoint)pt {
    CGSize size = [_view.imageCache addPNGFromResource:name :&name];
    return [_view coreView]->addImageShape([name UTF8String], pt.x, pt.y, size.width, size.height);
}

- (int)insertSVGFromResource:(NSString *)name {
    CGSize size = [_view.imageCache addSVGFromResource:name :&name];
    return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
}

- (int)insertSVGFromResource:(NSString *)name center:(CGPoint)pt {
    CGSize size = [_view.imageCache addSVGFromResource:name :&name];
    return [_view coreView]->addImageShape([name UTF8String], pt.x, pt.y, size.width, size.height);
}

+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size {
    return [ImageCache getImageFromSVGFile:filename maxSize:size];
}

- (int)insertImageFromFile:(NSString *)filename {
    NSString *name = nil;
    CGSize size = [_view.imageCache addImageFromFile:filename :&name];
    return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
}

- (void)setImagePath:(NSString *)path {
    [_view.imageCache setImagePath:path];
}

- (CALayer *)exportLayerTree:(BOOL)hidden {
    CALayer *rootLayer = [CALayer layer];
    rootLayer.frame = _view.bounds;
    
    GiShapeCallback shapeCallback(rootLayer, hidden);
    GiShapeAdapter adapter(&shapeCallback);
    long hDoc = [_view coreView]->acquireFrontDoc();
    long hGs = [_view coreView]->acquireGraphics([_view viewAdapter]);
    
    [_view coreView]->drawAll(hDoc, hGs, &adapter);
    [_view coreView]->releaseDoc(hDoc);
    [_view coreView]->releaseGraphics([_view viewAdapter], hGs);
    
    return rootLayer;
}

@end
