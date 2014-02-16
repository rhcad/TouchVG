//! \file GiViewHelper.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiViewHelper.h"
#import "GiViewImpl.h"
#import "ImageCache.h"
#include "GiShapeAdapter.h"
#include "gicoreview.h"

#define IOSLIBVERSION     3
extern NSString* EXTIMAGENAMES[];

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

@interface GiViewHelper ()
@property (nonatomic, WEAK) GiPaintView *view;
@end

@implementation GiViewHelper

@synthesize shapeCount, selectedCount, selectedType, selectedShapeID, content;
@synthesize changeCount, drawCount, displayExtent, boundingBox;
@synthesize command, lineWidth, strokeWidth, lineColor, lineAlpha;
@synthesize lineStyle, fillColor, fillAlpha;

static GiViewHelper *_sharedInstance = nil;

+ (NSString *)version {
    return [NSString stringWithFormat:@"1.1.%d.%d", IOSLIBVERSION, GiCoreView::getVersion()];
}

+ (void)initialize {
	if (!_sharedInstance) {
		_sharedInstance = [[GiViewHelper alloc] init];
	}
}

+ (GiViewHelper *)sharedInstance {
    if (!_sharedInstance.view) {
        _sharedInstance.view = [GiPaintView activeView];
    }
	return _sharedInstance;
}

+ (GiViewHelper *)sharedInstance:(GiPaintView *)view {
    _sharedInstance.view = view;
    return _sharedInstance;
}

- (id)init {
    return _sharedInstance ? nil : [super init];
}

+ (GiPaintView *)activeView {
    return [GiPaintView activeView];
}

- (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    _view = [GiPaintView createGraphView:frame :parentView];
    return _view;
}

- (GiPaintView *)createMagnifierView:(CGRect)frame
                             refView:(GiPaintView *)refView
                          parentView:(UIView *)parentView
{
    refView = refView ? refView : [GiPaintView activeView];
    _view = [GiPaintView createMagnifierView:frame refView:refView parentView:parentView];
    return _view;
}

- (long)cmdViewHandle {
    return [_view coreView]->viewAdapterHandle();
}

- (NSString *)command {
    return [NSString stringWithCString:[_view coreView]->getCommand() encoding:NSUTF8StringEncoding];
}

- (void)setCommand:(NSString *)name {
    [_view coreView]->setCommand([name UTF8String]);
}

- (BOOL)setCommand:(NSString *)name withParam:(NSString *)param {
    return [_view coreView]->setCommand([name UTF8String], [param UTF8String]);
}

+ (void)setExtraContextImages:(NSArray *)names {
    NSUInteger i = 0;
    for (; i < 40 && i < [names count]; i++) {
        EXTIMAGENAMES[i] = [names objectAtIndex:i];
    }
    EXTIMAGENAMES[i] = nil;
}

- (float)lineWidth {
    float w = [_view coreView]->getContext(false).getLineWidth();
    return w > 1e-6f ? 100.f * w : w;
}

- (void)setLineWidth:(float)value {
    [_view coreView]->getContext(true).setLineWidth(value > 1e-6f ? value / 100.f : value, true);
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
    __block long hDoc;
    if ([_view viewAdapter2]->isMainThread()) {
        hDoc = [_view coreView]->acquireFrontDoc();
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^{
            hDoc = [_view coreView]->acquireFrontDoc();
        });
    }
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

- (long)drawCount {
    return [_view coreView]->getDrawCount();
}

- (CGRect)displayExtent {
    mgvector<float> box(4);
    if ([_view coreView]->getDisplayExtent(box)) {
        float w = box.get(2) - box.get(0);
        float h = box.get(3) - box.get(1);
        return CGRectMake(box.get(0), box.get(1), w, h);
    }
    return CGRectNull;
}

- (CGRect)boundingBox {
    mgvector<float> box(4);
    if ([_view coreView]->getBoundingBox(box)) {
        float w = box.get(2) - box.get(0);
        float h = box.get(3) - box.get(1);
        return CGRectMake(box.get(0), box.get(1), w, h);
    }
    return CGRectNull;
}

+ (NSString *)addExtension:(NSString *)filename :(NSString *)ext {
    if (filename && ![filename hasSuffix:ext]) {
        filename = [[filename stringByDeletingPathExtension]
                    stringByAppendingPathExtension:[ext substringFromIndex:1]];
    }
    return filename;
}

- (BOOL)loadFromFile:(NSString *)vgfile readOnly:(BOOL)r {
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    return [_view coreView]->loadFromFile([vgfile UTF8String], r);
}

- (BOOL)loadFromFile:(NSString *)vgfile {
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    return [_view coreView]->loadFromFile([vgfile UTF8String]);
}

- (BOOL)saveToFile:(NSString *)vgfile {
    BOOL ret = NO;
    NSFileManager *fm = [NSFileManager defaultManager];
    __block long hDoc;
    
    if ([_view viewAdapter2]->isMainThread()) {
        hDoc = [_view coreView]->acquireFrontDoc();
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^{
            hDoc = [_view coreView]->acquireFrontDoc();
        });
    }
    
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    if ([_view coreView]->getShapeCount(hDoc) > 0) {
        if (![fm fileExistsAtPath:vgfile]) {
            [fm createFileAtPath:vgfile contents:[NSData data] attributes:nil];
        }
        ret = [_view coreView]->saveToFile(hDoc, [vgfile UTF8String]);
        NSLog(@"GiViewHelper saveToFile: %@, %d", vgfile, ret);
    } else {
        ret = [fm removeItemAtPath:vgfile error:nil];
        NSLog(@"GiViewHelper removeItemAtPath: %@, %d", vgfile, ret);
    }
    [_view coreView]->releaseDoc(hDoc);
    
    return ret;
}

- (void)clearShapes {
    [_view coreView]->clear();
}

- (UIImage *)snapshot {
    return [_view snapshot];
}

- (BOOL)savePng:(NSString *)filename {
    return [_view savePng:[GiViewHelper addExtension:filename :@".png"]];
}

- (BOOL)exportSVG:(NSString *)filename {
    filename = [GiViewHelper addExtension:filename :@".svg"];
    int ret = [_view coreView]->exportSVG([_view viewAdapter], [filename UTF8String]);
    NSLog(@"GiViewHelper exportSVG: %@, %d", filename, ret);
    return ret >= 0;
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
    __block long hDoc, hGs;
    
    if ([_view viewAdapter2]->isMainThread()) {
        hDoc = [_view coreView]->acquireFrontDoc();
        hGs = [_view coreView]->acquireGraphics([_view viewAdapter]);
    } else {
        dispatch_sync(dispatch_get_main_queue(), ^{
            hDoc = [_view coreView]->acquireFrontDoc();
            hGs = [_view coreView]->acquireGraphics([_view viewAdapter]);
        });
    }
    [_view coreView]->drawAll(hDoc, hGs, &adapter);
    GiCoreView::releaseDoc(hDoc);
    [_view coreView]->releaseGraphics(hGs);
    
    return rootLayer;
}

- (BOOL)recreateDirectory:(NSString *)path {
    NSFileManager *fm = [NSFileManager defaultManager];
    [fm removeItemAtPath:path error:nil];
    if (![fm createDirectoryAtPath:path
       withIntermediateDirectories:YES attributes:nil error:nil]) {
        NSLog(@"Fail to create directory: %@", path);
        return NO;
    }
    
    return YES;
}

- (BOOL)startUndoRecord:(NSString *)path {
    if ([_view coreView]->isUndoRecording()
        || !path || ![self recreateDirectory:path]) {
        return NO;
    }
    return [_view viewAdapter2]->startRecord(path, GiViewAdapter::kUndo);
}

- (void)stopUndoRecord {
    [_view viewAdapter2]->stopRecord(true);
}

- (BOOL)canUndo {
    return [_view coreView]->canUndo();
}

- (BOOL)canRedo {
    return [_view coreView]->canRedo();
}

- (void)undo {
    [_view viewAdapter2]->undo();
}

- (void)redo {
    [_view viewAdapter2]->redo();
}

- (BOOL)isRecording {
    return [_view coreView]->isRecording();
}

- (BOOL)startRecord:(NSString *)path {
    if ([_view coreView]->isRecording()
        || !path || ![self recreateDirectory:path]) {
        return NO;
    }
    return [_view viewAdapter2]->startRecord(path, GiViewAdapter::kRecord);
}

- (void)stopRecord {
    [_view viewAdapter2]->stopRecord(false);
}

- (BOOL)isPlaying {
    return [_view coreView]->isPlaying();
}

- (BOOL)startPlay:(NSString *)path {
    if ([_view coreView]->isPlaying() || !path) {
        return NO;
    }
    if (![[NSFileManager defaultManager] fileExistsAtPath:path]) {
        NSLog(@"No recorded files in %@", path);
        return NO;
    }
    return [_view viewAdapter2]->startRecord(path, GiViewAdapter::kPlay);
}

- (void)stopPlay {
    [_view viewAdapter2]->stopRecord(false);
}

@end
