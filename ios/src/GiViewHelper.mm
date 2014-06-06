//! \file GiViewHelper.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

#import "GiViewHelper.h"
#import "GiViewImpl.h"
#import "GiImageCache.h"

#define IOSLIBVERSION     11
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

struct GiImageFinderD : public MgFindImageCallback {
    GiImageCache    *cache;
    NSMutableArray  *arr;
    
    GiImageFinderD(GiImageCache *cache, NSMutableArray *arr)
        : cache(cache), arr(arr) {}
    
    virtual void onFindImage(int sid, const char* name) {
        NSString *title = [NSString stringWithUTF8String:name];
        CGRect rect = [[GiViewHelper sharedInstance] getShapeBox:sid];
        
        [arr addObject:@{@"id":[NSNumber numberWithInt:sid],
                         @"name":title,
                         @"path":[cache.imagePath stringByAppendingPathComponent:title],
                         @"image":[cache loadImage:title],
                         @"rect":[NSValue valueWithCGRect:rect]
                         }];
    }
};

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
    if (_sharedInstance.view != view) {
        _sharedInstance.view = view;
    }
    return _sharedInstance;
}

- (id)init {
    return _sharedInstance ? nil : [super init];
}

- (void)dealloc {
    [super DEALLOC];
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

+ (void)removeSubviews:(UIView *)owner {
    for (UIView *view : [owner subviews]) {
        [GiViewHelper removeSubviews:view];
        if ([view respondsToSelector:@selector(tearDown)]) {
            [view performSelector:@selector(tearDown)];
        }
    }
    while ([[owner subviews] count] > 0) {
        [[[owner subviews] objectAtIndex:[[owner subviews] count] - 1] removeFromSuperview];
    }
}

- (long)cmdViewHandle {
    return [_view coreView]->viewAdapterHandle();
}

- (NSString *)command {
    return [NSString stringWithCString:[_view coreView]->getCommand()
                              encoding:NSUTF8StringEncoding];
}

- (void)setCommand:(NSString *)name {
    [_view coreView]->setCommand([name UTF8String]);
}

- (BOOL)setCommand:(NSString *)name withParam:(NSString *)param {
    return [_view coreView]->setCommand([name UTF8String], [param UTF8String]);
}

- (BOOL)switchCommand {
    return [_view coreView]->switchCommand();
}

- (BOOL)isCommand:(const char*)name {
    return [_view coreView]->isCommand(name);
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
    [_view coreView]->setContext(GiContext::kLineWidth);
}

- (float)strokeWidth {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [_view coreView]->calcPenWidth([_view viewAdapter], ctx.getLineWidth());
}

- (void)setStrokeWidth:(float)value {
    [_view coreView]->getContext(true).setLineWidth(-fabsf(value), true);
    [_view coreView]->setContext(GiContext::kLineWidth);
}

- (GILineStyle)lineStyle {
    return (GILineStyle)[_view coreView]->getContext(false).getLineStyle();
}

- (void)setLineStyle:(GILineStyle)value {
    [_view coreView]->getContext(true).setLineStyle(value);
    [_view coreView]->setContext(GiContext::kLineStyle);
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
    [_view coreView]->setContext(c.a ? GiContext::kLineRGB : GiContext::kLineARGB);
}

- (float)lineAlpha {
    return (float)[_view coreView]->getContext(false).getLineAlpha() / 255.f;
}

- (void)setLineAlpha:(float)value {
    [_view coreView]->getContext(true).setLineAlpha((int)lroundf(value * 255.f));
    [_view coreView]->setContext(GiContext::kLineAlpha);
}

- (UIColor *)fillColor {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [self GiColorToUIColor:ctx.getFillColor()];
}

- (void)setFillColor:(UIColor *)value {
    GiColor c = value ? CGColorToGiColor(value.CGColor) : GiColor::Invalid();
    [_view coreView]->getContext(true).setFillColor(c);
    [_view coreView]->setContext(c.a ? GiContext::kFillRGB : GiContext::kFillARGB);
}

- (float)fillAlpha {
    return (float)[_view coreView]->getContext(false).getFillAlpha() / 255.f;
}

- (void)setFillAlpha:(float)value {
    [_view coreView]->getContext(true).setFillAlpha((int)lroundf(value * 255.f));
    [_view coreView]->setContext(GiContext::kFillAlpha);
}

- (void)setContextEditing:(BOOL)editing {
    [_view coreView]->setContextEditing(editing);
}

- (long)acquireFrontDoc {
    return [_view viewAdapter2]->acquireFrontDoc();
}

- (NSString *)content {
    long doc = [self acquireFrontDoc];
    const char* str = [_view coreView]->getContent(doc);
    NSString * ret = [NSString stringWithCString:str encoding:NSUTF8StringEncoding];
    [_view coreView]->freeContent();
    [_view coreView]->releaseDoc(doc);
    return ret;
}

- (void)setContent:(NSString *)value {
    [_view.imageCache clearCachedData];
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
    [_view coreView]->getDisplayExtent(box);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

- (CGRect)boundingBox {
    mgvector<float> box(4);
    [_view coreView]->getBoundingBox(box);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

- (CGRect)getShapeBox:(int)sid {
    mgvector<float> box(4);
    [_view coreView]->getBoundingBox(box, sid);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

+ (NSString *)addExtension:(NSString *)filename :(NSString *)ext {
    if (filename && ![filename hasSuffix:ext]) {
        filename = [[filename stringByDeletingPathExtension]
                    stringByAppendingPathExtension:[ext substringFromIndex:1]];
    }
    return filename;
}

- (BOOL)loadFromFile:(NSString *)vgfile readOnly:(BOOL)r {
    [_view.imageCache clearCachedData];
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    return [_view coreView]->loadFromFile([vgfile UTF8String], r);
}

- (BOOL)loadFromFile:(NSString *)vgfile {
    [_view.imageCache clearCachedData];
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    return [_view coreView]->loadFromFile([vgfile UTF8String]);
}

- (BOOL)saveToFile:(NSString *)vgfile {
    BOOL ret = NO;
    NSFileManager *fm = [NSFileManager defaultManager];
    long doc = [self acquireFrontDoc];
    
    vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
    if ([_view coreView]->getShapeCount(doc) > 0) {
        if (![fm fileExistsAtPath:vgfile]) {
            [fm createFileAtPath:vgfile contents:[NSData data] attributes:nil];
        }
        ret = [_view coreView]->saveToFile(doc, [vgfile UTF8String]);
        NSLog(@"GiViewHelper saveToFile: %@, %d", vgfile, ret);
    } else {
        ret = [fm removeItemAtPath:vgfile error:nil];
        NSLog(@"GiViewHelper removeItemAtPath: %@, %d", vgfile, ret);
    }
    [_view coreView]->releaseDoc(doc);
    
    return ret;
}

- (void)clearShapes {
    [_view coreView]->clear();
    [_view.imageCache clearCachedData];
}

- (UIImage *)snapshot {
    return [_view snapshot];
}

- (UIImage *)extentSnapshot:(CGFloat)space {
    [_view hideContextActions];
    
    CGRect extent = [self displayExtent];
    if (CGRectIsEmpty(extent))
        return nil;
    
    extent = CGRectIntersection(_view.bounds, CGRectInset(extent, -space, -space));
    if (CGRectIsEmpty(extent))
        return nil;
    
    UIGraphicsBeginImageContextWithOptions(extent.size, _view.opaque, 0);
    CGContextTranslateCTM(UIGraphicsGetCurrentContext(), -extent.origin.x, -extent.origin.y);
    [_view.layer renderInContext:UIGraphicsGetCurrentContext()];
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}

- (BOOL)exportExtentAsPNG:(NSString *)filename space:(CGFloat)space {
    UIImage *image = [self extentSnapshot:space];
    BOOL ret = [UIImagePNGRepresentation(image) writeToFile:filename atomically:NO];
    if (ret) {
        NSLog(@"exportExtentAsPNG: %@, %d, %.0fx%.0f@%.0fx",
              filename, ret, image.size.width, image.size.height, image.scale);
    }
    return ret;
}

- (BOOL)exportPNG:(NSString *)filename {
    return [_view exportPNG:[GiViewHelper addExtension:filename :@".png"]];
}

- (BOOL)exportSVG:(NSString *)filename {
    filename = [GiViewHelper addExtension:filename :@".svg"];
    int ret = [_view coreView]->exportSVG([_view viewAdapter], [filename UTF8String]);
    NSLog(@"GiViewHelper exportSVG: %@, %d", filename, ret);
    return ret >= 0;
}

- (CGPoint)displayToModel:(CGPoint)point {
    mgvector<float> pt(point.x, point.y);
    if ([_view coreView]->displayToModel(pt))
        point = CGPointMake(pt.get(0), pt.get(1));
    return point;
}

- (CGRect)displayRectToModel:(CGRect)rect {
    mgvector<float> rc(CGRectGetMinX(rect), CGRectGetMinY(rect),
                       CGRectGetMaxX(rect), CGRectGetMaxY(rect));
    if ([_view coreView]->displayToModel(rc)) {
        rect = CGRectMake(rc.get(0), rc.get(1),
                          rc.get(2) - rc.get(0), rc.get(3) - rc.get(1));
    }
    return rect;
}

- (BOOL)zoomToExtent {
    return [_view coreView]->zoomToExtent();
}

- (BOOL)zoomToModel:(CGRect)rect {
    return [_view coreView]->zoomToModel(rect.origin.x, rect.origin.y,
                                         rect.size.width, rect.size.height);
}

- (void)setZoomEnabled:(BOOL)enabled {
    [_view coreView]->setZoomEnabled([_view viewAdapter], enabled);
}

- (int)addShapesForTest {
    @synchronized([_view locker]) {
        return [_view coreView]->addShapesForTest();
    }
}

- (void)clearCachedData {
    [_view clearCachedData];
}

- (int)insertPNGFromResource:(NSString *)name {
    CGSize size = [_view.imageCache addPNGFromResource:name :&name];
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
    }
}

- (int)insertPNGFromResource:(NSString *)name center:(CGPoint)pt {
    CGSize size = [_view.imageCache addPNGFromResource:name :&name];
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], pt.x, pt.y,
                                               size.width, size.height, 0);
    }
}

- (int)insertSVGFromResource:(NSString *)name {
    CGSize size = [_view.imageCache addSVGFromResource:name :&name];
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
    }
}

- (int)insertSVGFromResource:(NSString *)name center:(CGPoint)pt {
    CGSize size = [_view.imageCache addSVGFromResource:name :&name];
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], pt.x, pt.y,
                                               size.width, size.height, 0);
    }
}

+ (UIImage *)getImageFromSVGFile:(NSString *)filename maxSize:(CGSize)size {
    return [GiImageCache getImageFromSVGFile:filename maxSize:size];
}

- (CGSize)insertImageFromFile_:(NSString *)filename :(NSString **)name {
    CGSize size = [_view.imageCache addImageFromFile:filename :name];
    
    if (size.width > 0.5f) {
        NSString *dest = [_view.imageCache.playPath stringByAppendingPathComponent:*name];
        NSFileManager *fm = [NSFileManager defaultManager];
        
        if (dest && ![fm fileExistsAtPath:dest]) {
            [fm copyItemAtPath:filename toPath:dest error:nil];
        }
    }
    
    return size;
}

- (int)insertImageFromFile:(NSString *)filename {
    NSString *name = nil;
    CGSize size = [self insertImageFromFile_:filename :&name];
    
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], size.width, size.height);
    }
}
    
- (int)insertImageFromFile:(NSString *)filename center:(CGPoint)pt tag:(int)tag {
    NSString *name = nil;
    CGSize size = [self insertImageFromFile_:filename :&name];
    
    @synchronized([_view locker]) {
        return [_view coreView]->addImageShape([name UTF8String], pt.x, pt.y,
                                               size.width, size.height, tag);
    }
}

- (BOOL)hasImageShape {
    long doc = [self acquireFrontDoc];
    bool ret = [_view coreView]->hasImageShape(doc);
    [_view coreView]->releaseDoc(doc);
    return ret;
}

- (int)findShapeByImageID:(NSString *)name {
    long doc = [self acquireFrontDoc];
    int ret = [_view coreView]->findShapeByImageID(doc, [name UTF8String]);
    [_view coreView]->releaseDoc(doc);
    return ret;
}

- (int)findShapeByTag:(int)tag {
    long doc = [self acquireFrontDoc];
    int ret = [_view coreView]->findShapeByTag(doc, tag);
    [_view coreView]->releaseDoc(doc);
    return ret;
}

- (NSArray *)getImageShapes {
    NSMutableArray *arr = [[NSMutableArray alloc]init];
    long doc = [self acquireFrontDoc];
    GiImageFinderD f(_view.imageCache, arr);
    
    [_view coreView]->traverseImageShapes(doc, &f);
    [_view coreView]->releaseDoc(doc);
    
    return [arr AUTORELEASE];
}

- (void)setImagePath:(NSString *)path {
    [_view.imageCache setImagePath:path];
}

- (NSString *)getImagePath {
    return _view.imageCache.imagePath;
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

- (GiViewAdapter *)internalAdapter {
    return [(_view.mainView ? _view.mainView : _view) viewAdapter2];
}

- (BOOL)startUndoRecord:(NSString *)path {
    if ([_view coreView]->isUndoRecording()
        || !path || ![self recreateDirectory:path]) {
        return NO;
    }
    return [self internalAdapter]->startRecord(path, true);
}

- (void)stopUndoRecord {
    [self internalAdapter]->stopRecord(true);
}

- (BOOL)canUndo {
    return [_view coreView]->canUndo();
}

- (BOOL)canRedo {
    return [_view coreView]->canRedo();
}

- (void)undo {
    [self internalAdapter]->undo();
}

- (void)redo {
    [self internalAdapter]->redo();
}

- (BOOL)isRecording {
    return [_view coreView]->isRecording();
}

- (BOOL)startRecord:(NSString *)path {
    if ([_view coreView]->isRecording()
        || !path || ![self recreateDirectory:path]) {
        return NO;
    }
    return [self internalAdapter]->startRecord(path, false);
}

- (void)stopRecord {
    [self internalAdapter]->stopRecord(false);
}

- (BOOL)isPaused {
    return [_view coreView]->isPaused();
}

- (BOOL)isPlaying {
    return [_view coreView]->isPlaying();
}

- (BOOL)playPause {
    return [_view coreView]->onPause(getTickCount());
}

- (BOOL)playResume {
    return [_view coreView]->onResume(getTickCount());
}

- (long)getPlayTicks {
    return [_view coreView]->getRecordTick(false, getTickCount());
}

- (void)addDelegate:(id<GiPaintViewDelegate>)d {
    [_view addDelegate:d];
}

- (void)removeDelegate:(id<GiPaintViewDelegate>)d {
    [_view removeDelegate:d];
}

@end
