//! \file GiViewHelper.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper
// Copyright (c) 2012-2016, https://github.com/rhcad/vgios, BSD License

#import "GiViewHelper.h"
#import "GiViewImpl.h"
#import "GiImageCache.h"
#include "mgview.h"

#define IOSLIBVERSION     35

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

//! 遍历图像块用的回调类
struct GiImageFinderD : public MgFindImageCallback {
    GiImageCache    *cache;
    NSMutableArray  *arr;
    
    GiImageFinderD(GiImageCache *cache, NSMutableArray *arr)
        : cache(cache), arr(arr) {}
    
    virtual void onFindImage(int sid, const char* name) {
        NSString *title = @(name);
        CGRect rect = [[GiViewHelper sharedInstance] getShapeBox:sid];
        
        [arr addObject:@{@"id":@(sid),
                         @"name":title,
                         @"path":[cache.imagePath stringByAppendingPathComponent:title],
                         @"image":[cache loadImage:title],
                         @"rect":[NSValue valueWithCGRect:rect]
                         }];
    }
};

struct GiOptionCallback : public MgOptionCallback {
    NSMutableDictionary *rootDict;
    
    GiOptionCallback(NSMutableDictionary *dict) : rootDict(dict) {}
    
    virtual void onGetOptionBool(const char* name, bool value) {
        rootDict[@(name)] = @(value);
    }
    virtual void onGetOptionInt(const char* name, int value) {
        rootDict[@(name)] = @(value);
    }
    virtual void onGetOptionFloat(const char* name, float value) {
        rootDict[@(name)] = @(value);
    }
    virtual void onGetOptionString(const char* name, const char* text) {
        rootDict[@(name)] = @(text);
    }
};

@interface GiViewHelper ()
@property (nonatomic, WEAK) GiPaintView *view;
@end

@implementation GiViewHelper

@synthesize shapeCount, selectedCount, selectedType, selectedShapeID, selectedIds, content;
@synthesize changeCount, drawCount, displayExtent, boundingBox, selectedHandle;
@synthesize command, lineWidth, strokeWidth, lineColor, lineAlpha, startArrowHead, endArrowHead;
@synthesize lineStyle, fillColor, fillAlpha, options, zoomEnabled, viewBox, modelBox;
@synthesize currentPoint, currentModelPoint, viewScale, viewCenter;

static GiViewHelper *_sharedInstance = nil;

+ (NSString *)version {
    return [NSString stringWithFormat:@"1.1.%d.%d", IOSLIBVERSION, GiCoreView::getVersion()];
}

+ (GiViewHelper *)sharedInstance {
    if (!_sharedInstance) {
        _sharedInstance = [[GiViewHelper alloc] init];
    }
    if (!_sharedInstance.view) {
        _sharedInstance.view = [GiPaintView activeView];
    }
    return _sharedInstance;
}

+ (GiViewHelper *)sharedInstance:(GiPaintView *)view {
    if (!_sharedInstance) {
        _sharedInstance = [[GiViewHelper alloc] init];
    }
    if (_sharedInstance.view != view) {
        _sharedInstance.view = view;
    }
    return _sharedInstance;
}

- (id)initWithView:(GiPaintView *)view {
    self = [super init];
    if (self) {
        _view = view;
    }
    return self;
}

- (void)dealloc {
    [super DEALLOC];
}

- (GiPaintView *)view {
    return _view;
}

+ (GiPaintView *)activeView {
    return [GiPaintView activeView];
}

- (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    _view = [GiPaintView createGraphView:frame :parentView :0];
    return _view;
}

- (GiPaintView *)createGraphView:(CGRect)frame
                          inView:(UIView *)parentView
                           flags:(int)flags {
    _view = [GiPaintView createGraphView:frame :parentView :flags];
    return _view;
}

- (GiPaintView *)createDummyView:(CGSize)size {
    return [self createGraphView:CGRectMake(0, 0, size.width, size.height)
                          inView:nil flags:GIViewFlagsDummyView];
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

- (MgView *)cmdView {
    return MgView::fromHandle([_view coreView]->viewAdapterHandle());
}

- (MgShapeFactory *)shapeFactory {
    return [self cmdView]->getShapeFactory();
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

- (CGFloat)lineWidth {
    float w = [_view coreView]->getContext(false).getLineWidth();
    return w > 1e-6f ? 100.f * w : w;
}

- (void)setLineWidth:(CGFloat)value {
    [_view coreView]->getContext(true).setLineWidth(value > 1e-6f ? value / 100.f : value, true);
    [_view coreView]->setContext(GiContext::kLineWidth);
}

- (CGFloat)strokeWidth {
    GiContext& ctx = [_view coreView]->getContext(false);
    return [_view coreView]->calcPenWidth([_view viewAdapter], ctx.getLineWidth());
}

- (void)setStrokeWidth:(CGFloat)value {
    [_view coreView]->getContext(true).setLineWidth(-fabsf((float)value), true);
    [_view coreView]->setContext(GiContext::kLineWidth);
}

- (GILineStyle)lineStyle {
    return (GILineStyle)[_view coreView]->getContext(false).getLineStyle();
}

- (void)setLineStyle:(GILineStyle)value {
    [_view coreView]->getContext(true).setLineStyle(value);
    [_view coreView]->setContext(GiContext::kLineStyle);
}

- (GIArrowHead)startArrowHead {
    return (GIArrowHead)[_view coreView]->getContext(false).getStartArrayHead();
}

- (GIArrowHead)endArrowHead {
    return (GIArrowHead)[_view coreView]->getContext(false).getEndArrayHead();
}

- (void)setStartArrowHead:(GIArrowHead)value {
    [_view coreView]->getContext(true).setStartArrayHead(value);
    [_view coreView]->setContext(GiContext::kLineArrayHead);
}

- (void)setEndArrowHead:(GIArrowHead)value {
    [_view coreView]->getContext(true).setEndArrayHead(value);
    [_view coreView]->setContext(GiContext::kLineArrayHead);
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

- (CGFloat)lineAlpha {
    return (float)[_view coreView]->getContext(false).getLineAlpha() / 255.f;
}

- (void)setLineAlpha:(CGFloat)value {
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

- (CGFloat)fillAlpha {
    return (float)[_view coreView]->getContext(false).getFillAlpha() / 255.f;
}

- (void)setFillAlpha:(CGFloat)value {
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

- (int)unlockedShapeCount {
    return [_view coreView]->getUnlockedShapeCount();
}

- (int)visibleShapeCount {
    return [_view coreView]->getVisibleShapeCount();
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

- (void)setSelectedShapeID:(int)sid {
    self.command = [NSString stringWithFormat:@"select{'id':%d}", sid];
}

- (NSArray *)selectedIds {
    mgvector<int> ids;
    [_view coreView]->getSelectedShapeIDs(ids);
    
    if (ids.count() == 0 || ids.get(0) == 0)
        return nil;
    
    NSMutableArray *arr = [NSMutableArray array];
    
    for (int i = 0; i < ids.count(); i++) {
        [arr addObject:@(ids.get(i))];
    }
    return arr;
}

- (void)setSelectedIds:(NSArray *)arr {
    mgvector<int> ids((int)[arr count]);
    
    for (int i = 0; i < ids.count(); i++) {
        ids.set(i, [[arr objectAtIndex:i] intValue]);
    }
    [_view coreView]->setSelectedShapeIDs(ids);
}

- (int)selectedHandle {
    return [_view coreView]->getSelectedHandle();
}

- (long)changeCount {
    return [_view coreView]->getChangeCount();
}

- (long)drawCount {
    return [_view coreView]->getDrawCount();
}

- (CGFloat)viewScale {
    return [self cmdView]->xform()->getViewScale();
}

- (void)setViewScale:(CGFloat)value {
    if ([self cmdView]->xform()->zoomScale(value)) {
        [self cmdView]->regenAll(false);
    }
}

- (CGPoint)viewCenter {
    Point2d pt([self cmdView]->xform()->getCenterW());
    return CGPointMake(pt.x, pt.y);
}

- (void)setViewCenter:(CGPoint)value {
    if ([self cmdView]->xform()->zoomTo(Point2d(value.x, value.y))) {
        [self cmdView]->regenAll(false);
    }
}

- (CGRect)viewBox {
    mgvector<float> box(4);
    [_view coreView]->getViewModelBox(box);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

- (void)setViewBox:(CGRect)box {
    [self zoomToModel:box];
}

- (CGRect)modelBox {
    mgvector<float> box(4);
    [_view coreView]->getModelBox(box);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
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

- (CGPoint)currentPoint {
    Point2d pt([self cmdView]->motion()->point);
    return CGPointMake(pt.x, pt.y);
}

- (CGPoint)currentModelPoint {
    Point2d pt([self cmdView]->motion()->pointM);
    return CGPointMake(pt.x, pt.y);
}

- (CGRect)getShapeBox:(int)sid {
    mgvector<float> box(4);
    [_view coreView]->getBoundingBox(box, sid);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

- (CGRect)getModelBox:(int)sid {
    mgvector<float> box(4);
    [_view coreView]->getModelBox(box, sid);
    
    float w = box.get(2) - box.get(0);
    float h = box.get(3) - box.get(1);
    return CGRectMake(box.get(0), box.get(1), w, h);
}

- (CGPoint)getHandlePoint:(int)sid index:(int)i {
    mgvector<float> xy(2);
    return ([_view coreView]->getHandlePoint(xy, sid, i)
            ? CGPointMake(xy.get(0), xy.get(1)) : CGPointMake(NAN, NAN));
}

+ (NSString *)addExtension:(NSString *)filename :(NSString *)ext {
    if (filename && ![filename hasSuffix:ext] && ![filename hasSuffix:@".json"]) {
        filename = [[filename stringByDeletingPathExtension]
                    stringByAppendingPathExtension:[ext substringFromIndex:1]];
    }
    return filename;
}

- (BOOL)loadFromFile:(NSString *)vgfile readOnly:(BOOL)r {
    @synchronized([_view locker]) {
        [_view.imageCache clearCachedData];
        vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
        return [_view coreView]->loadFromFile([vgfile UTF8String], r);
    }
}

- (BOOL)loadFromFile:(NSString *)vgfile {
    @synchronized([_view locker]) {
        [_view.imageCache clearCachedData];
        vgfile = [GiViewHelper addExtension:vgfile :@".vg"];
        return [_view coreView]->loadFromFile([vgfile UTF8String]);
    }
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
    } else {
        ret = [fm removeItemAtPath:vgfile error:nil];
        NSLog(@"GiViewHelper removeItemAtPath: %@, %d", vgfile, ret);
    }
    [_view coreView]->releaseDoc(doc);
    
    return ret;
}

- (void)clearShapes {
    @synchronized([_view locker]) {
        [_view coreView]->clear();
        [_view.imageCache clearCachedData];
    }
}

- (void)clearShapes:(BOOL)showMessage {
    @synchronized([_view locker]) {
        [_view coreView]->loadShapes(NULL);
        [_view.imageCache clearCachedData];
    }
}

- (void)eraseView {
    @synchronized([_view locker]) {
        [_view coreView]->setCommand("erasewnd");
    }
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

- (UIImage *)snapshotWithShapes:(NSArray *)ids size:(CGSize)size {
    GiViewHelper *hlp = [[GiViewHelper alloc]init];
    GiPaintView *tmpview = [hlp createDummyView:size];
    MgShapes *srcs = self.cmdView->shapes();
    MgShapes *dests = hlp.cmdView->shapes();
    
    @synchronized([_view locker]) {
        if ([ids count] == 0) {
            dests->copyShapes(srcs, false);
        } else {
            for (NSNumber *sid in ids) {
                const MgShape *sp = srcs->findShape([sid intValue]);
                if (sp) {
                    MgShape* newsp = dests->addShape(*sp);
                    if (newsp) {
                        newsp->shape()->setFlag(kMgHideContent, false);
                        if (newsp->context().getLineAlpha() > 0 && newsp->context().getLineAlpha() < 20) {
                            GiContext ctx(newsp->context());
                            ctx.setLineAlpha(20);
                            newsp->setContext(ctx, GiContext::kLineAlpha);
                        }
                    }
                }
            }
        }
    }
    [hlp zoomToExtent];
    
    UIImage *image = [hlp snapshot];
    [tmpview removeFromSuperview];
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

- (int)importSVGPath:(int)sid d:(NSString *)d {
    long shapes = [_view coreView]->backShapes();
    
    sid = [_view coreView]->importSVGPath(shapes, sid, [d UTF8String]);
    if (sid) {
        [_view viewAdapter]->regenAll(true);
    }
    return sid;
}

- (NSString *)exportSVGPath:(int)sid {
    __block NSString *ret = nil;
    GiStringCallback c(^(NSString *s) {
        ret = s;
    });
    [_view coreView]->exportSVGPath2(&c, [_view coreView]->backShapes(), sid);
    return ret;
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

- (BOOL)zoomToExtent:(CGFloat)margin {
    return [_view coreView]->zoomToExtent(margin);
}

- (BOOL)zoomToModel:(CGRect)rect {
    return [_view coreView]->zoomToModel(rect.origin.x, rect.origin.y,
                                         rect.size.width, rect.size.height);
}

- (BOOL)zoomPan:(CGPoint)offPixel {
    return [_view coreView]->zoomPan(offPixel.x, offPixel.y);
}

- (BOOL)zoomEnabled {
    return [_view coreView]->isZoomEnabled([_view viewAdapter]);
}

- (void)setZoomEnabled:(BOOL)enabled {
    [_view coreView]->setZoomEnabled([_view viewAdapter], enabled);
}

- (int)addShapesForTest {
    @synchronized([_view locker]) {
        return [_view coreView]->addShapesForTest();
    }
}

- (void)showMessage:(NSString *)text {
    [_view viewAdapter]->showMessage([text UTF8String]);
}

+ (NSString *)localizedString:(NSString *)name {
    return GiLocalizedString(name);
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

- (BOOL)getImageSize:(float*)info shape:(int)sid {
    mgvector<float> arr(5);
    bool ret = [_view coreView]->getImageSize(arr, sid);
    
    for (int i = 0; i < 5; i++) {
        info[i] = arr.get(i);
    }
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

- (long)getRecordTicks {
    return [_view coreView]->getRecordTick(false, getTickCount());
}

- (void)combineRegen:(dispatch_block_t)block {
    MgRegenLocker locker([self cmdView]);
    block();
}

- (void)addDelegate:(id<GiPaintViewDelegate>)d {
    [_view addDelegate:d];
}

- (void)removeDelegate:(id<GiPaintViewDelegate>)d {
    [_view removeDelegate:d];
}

- (NSDictionary *)options {
    GiOptionCallback c([NSMutableDictionary dictionary]);
    [_view coreView]->traverseOptions(&c);
    c.onGetOptionBool("contextActionEnabled", _view.contextActionEnabled);
    c.onGetOptionBool("zoomEnabled", self.zoomEnabled);
    c.onGetOptionBool("showMagnifier", !!(_view.flags & GIViewFlagsMagnifier));
    return c.rootDict;
}

- (void)setOptions:(NSDictionary *)dict {
    GiCoreView *cv = [_view coreView];
    NSNumber *num;
    
    if (dict && dict.count > 0) {
        for (NSString *name in dict.allKeys) {
            num = dict[name];
            
            if ([num isKindOfClass:[NSString class]]) {
                cv->setOptionString([name UTF8String], [dict[name] UTF8String]);
            }
            if (![num isKindOfClass:[NSNumber class]]) {
                continue;
            }
            
            if ([name isEqualToString:@"contextActionEnabled"]) {
                _view.contextActionEnabled = [num boolValue];
                [_view hideContextActions];
            }
            else if ([name isEqualToString:@"zoomEnabled"]) {
                self.zoomEnabled = [num boolValue];
            }
            else if ([name isEqualToString:@"showMagnifier"]) {
                if ([num boolValue])
                    _view.flags |= GIViewFlagsMagnifier;
                else
                    _view.flags &= ~GIViewFlagsMagnifier;
            }
            else if (strstr(object_getClassName(num), "Boolean")) {
                cv->setOptionBool([name UTF8String], [num boolValue]);
            } else if (strcmp([num objCType], @encode(int)) == 0 ||
                       strcmp([num objCType], "q") == 0) {
                cv->setOptionInt([name UTF8String], [num intValue]);
            } else if (strcmp([num objCType], @encode(float)) == 0 ||
                       strcmp([num objCType], @encode(double)) == 0) {
                cv->setOptionFloat([name UTF8String], [num floatValue]);
            } else {
                NSLog(@"Unsupported number type: %@, %s", name, [num objCType]);
            }
        }
    } else {
        cv->setOptionBool(NULL, false);
    }
}

- (void)setOption:(id)value forKey:(NSString *)key {
    if (key) {
        self.options = @{ key : value };
    } else {
        [_view coreView]->setOptionBool(NULL, false);
    }
}

@end

#pragma mark - GiMessageHelper

//! The UILabel subclass for show message text.
@interface WDLabel : UILabel
@end

@implementation WDLabel

- (void)drawRect:(CGRect)rect
{
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    
    rect = CGRectInset(rect, 8.0f, 8.0f);
    CGContextSetShadow(ctx, CGSizeMake(0, 2), 4);
    
    [[UIColor colorWithWhite:0.0f alpha:0.5f] set];
    
    UIBezierPath *path = [UIBezierPath bezierPathWithRoundedRect:rect cornerRadius:9.0f];
    [path fill];
    
    [[UIColor whiteColor] set];
    path.lineWidth = 2;
    [path stroke];
    
    [super drawRect:rect];
}

@end

@implementation GiMessageHelper

- (void)hideMessage {
    [UIView animateWithDuration:0.2f
                     animations:^{ _label.alpha = 0.0f; }
                     completion:^(BOOL finished) {
                         [self removeLabel];
                     }];
}

- (void)removeLabel {
    if (_label) {
        [_label removeFromSuperview];
        _label = nil;
    }
}

- (void)showMessage:(NSString *)message inView:(UIView *)view {
    BOOL created = NO;
    
    [self removeLabel];
    
    if (!_label) {
        _label = [[WDLabel alloc] initWithFrame:CGRectInset(CGRectMake(0,0,100,40), -8, -8)];
        _label.textColor = [UIColor whiteColor];
        _label.font = [UIFont boldSystemFontOfSize:24.0f];
        _label.textAlignment = NSTextAlignmentCenter;
        _label.opaque = NO;
        _label.backgroundColor = nil;
        _label.alpha = 0;
        
        created = YES;
    }
    
    _label.text = message;
    [_label sizeToFit];
    
    CGRect frame = _label.frame;
    frame.size.width = MAX(frame.size.width, 80.f);
    frame = CGRectInset(frame, -20, -15);
    _label.frame = frame;
    _label.center = view.center;
    
    if (created) {
        [view addSubview:_label];
        
        [UIView animateWithDuration:0.2f animations:^{ _label.alpha = 1; }];
    }
    
    // start message dismissal timer
    [NSTimer scheduledTimerWithTimeInterval:0.7 target:self
                                   selector:@selector(hideMessage)
                                   userInfo:nil repeats:NO];
}

@end
