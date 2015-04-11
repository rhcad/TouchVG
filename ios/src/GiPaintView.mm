//! \file GiPaintView.mm
//! \brief 实现iOS绘图视图类 GiPaintView
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiViewImpl.h"
#import "GiImageCache.h"
#import "GiMagnifierView.h"
#include <map>

#pragma mark - GiDynDrawView

@implementation GiDynDrawView

- (void)dealloc {
    [super DEALLOC];
}

- (id)initView:(CGRect)frame :(GiViewAdapter *)adapter {
    self = [super initWithFrame:frame];
    if (self) {
        _adapter = adapter;
        self.opaque = NO;                           // 透明背景
        self.userInteractionEnabled = NO;           // 禁止交互，避免影响主视图显示
        self.contentMode = UIViewContentModeRedraw; // 避免转屏变形
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    [GiDynDrawView draw:_adapter];
}

+ (void)draw:(GiViewAdapter *)adapter {
    if (adapter->getFlags() & GIViewFlagsNotDynDraw) {
        return;
    }
    
    GiCanvasAdapter canvas(adapter->imageCache());
    GiCoreView* coreView = adapter->coreView();
    long doc, gs, playh;
    mgvector<long> shapes;
    
    @synchronized(adapter->locker()) {
        int sid = adapter->getAppendID(0, playh);
        doc = sid != 0 ? coreView->acquireFrontDoc(playh) : 0;
        gs = coreView->acquireGraphics(adapter);
        coreView->acquireDynamicShapesArray(shapes);
    }
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext(), true)) {
        for (int i = 0, sid = 0; (sid = adapter->getAppendID(i, playh)) != 0; i++) {
            coreView->drawAppend(doc, gs, &canvas, sid);
        }
        coreView->dynDraw(shapes, gs, &canvas);
        canvas.endPaint();
    }
    GiCoreView::releaseDoc(doc);
    GiCoreView::releaseShapesArray(shapes);
    coreView->releaseGraphics(gs);
    
    adapter->onDynDrawEnded();
}

@end

#pragma mark - GiLayerRender

@implementation GiLayerRender

- (id)initWithAdapter:(GiViewAdapter *)adapter {
    self = [super init];
    if (self) {
        _adapter = adapter;
        _queue = dispatch_queue_create("touchvg.render", NULL);
    }
    return self;
}

- (void)dealloc {
    [super DEALLOC];
}

- (BOOL)renderInContext:(CGContextRef)ctx {
    if (_layer && _queue) {
        dispatch_sync(_queue, ^{
            [_layer renderInContext:ctx];
        });
    }
    return _layer && _queue;
}

- (void)stopRender {
    if (_queue) {
#ifndef OS_OBJECT_USE_OBJC
        dispatch_release(_queue);
#endif
        _queue = NULL;
    }
    if (_layer) {
        _layer.delegate = nil;
        [_layer RELEASEOBJ];
        _layer = nil;
    }
}

- (void)clearCachedData {
    if (_queue && _layer) {
        dispatch_async(_queue, ^{
            _layer.delegate = nil;
            [_layer RELEASEOBJ];
            _layer = nil;
        });
    }
}

- (void)startRender:(mgvector<long>*)docs :(long)gs {
    if (_docs || _gs) {
        _adapter->coreView()->releaseGraphics(gs);
        GiCoreView::releaseDocs(*docs);
        delete docs;
        ++_drawing;     // pending
    } else {
        _docs = docs;
        _gs = gs;
        _drawing = 0;
        [self startRender_:NO];
    }
}

- (void)startRender_:(BOOL)forPending {
    if (forPending) {
        if (_adapter->getAppendCount() == 0 && _drawing == 0) {
            return;
        }
        _drawing = 0;
    }
    
    CALayer *srcLayer = _adapter->mainView().layer;
    if (++_drawing == 1 && _queue && srcLayer) {
        if (!_layer) {
            _layer = [[CALayer alloc]init];
            _layer.delegate = self;
            _layer.contentsScale = srcLayer.contentsScale;
            _layer.doubleSided = NO;
            _layer.drawsAsynchronously = YES;
        }
        dispatch_async(_queue, ^{
            _layer.frame = srcLayer.frame;
            _layer.position = srcLayer.position;
            [_layer setAffineTransform:[srcLayer affineTransform]];
            
            [_layer setNeedsDisplay];
            [_layer display];
        });
    }
}

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
    GiCanvasAdapter canvas(_adapter->imageCache());
    GiCoreView* coreView = _adapter->coreView();
    
    if (!_docs && !_gs) {
        @synchronized(_adapter->locker()) {
            _adapter->beginRender();
            _docs = new mgvector<long>;
            coreView->acquireFrontDocs(*_docs);
            _gs = coreView->acquireGraphics(_adapter);
        }
    }
    if (_docs && canvas.beginPaint(ctx)) {
        CGContextClearRect(ctx, _adapter->mainView().bounds);
        coreView->drawAll(*_docs, _gs, &canvas);
        canvas.endPaint();
    }
    if (_docs) {
        GiCoreView::releaseDocs(*_docs);
        delete _docs;
        _docs = NULL;
    }
    coreView->releaseGraphics(_gs);
    _gs = 0;
    if (_drawing > 0)
        --_drawing;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [_adapter->mainView() setNeedsDisplay];
        if (_drawing > 0)
            [self startRender_:YES];
    });
}

@end

static GiPaintView* _activePaintView = nil;
static std::map<int, dispatch_block_t> _extActions;

GiColor CGColorToGiColor(CGColorRef color);

@implementation GiPaintView

@synthesize panRecognizer = _panRecognizer;
@synthesize tapRecognizer = _tapRecognizer;
@synthesize twoTapsRecognizer = _twoTapsRecognizer;
@synthesize pressRecognizer = _pressRecognizer;
@synthesize pinchRecognizer = _pinchRecognizer;
@synthesize rotationRecognizer = _rotationRecognizer;
@synthesize gestureEnabled = _gestureEnabled;
@synthesize mainView = _mainView;
@synthesize viewToMagnify = _viewToMagnify;
@synthesize imageCache;
@synthesize delegates;
@synthesize flags;
@synthesize contextActionEnabled;

#pragma mark - Respond to low-memory warnings
+ (void)initialize {
    if (self == [GiPaintView class]) {  // Have to protect against subclasses calling this
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(didReceiveMemoryWarningNotification:)
                                                     name:UIApplicationDidReceiveMemoryWarningNotification
                                                   object:nil];
    }
}

+ (void)didReceiveMemoryWarningNotification:(NSNotification*)notification {
    [_activePaintView clearCachedData];
}

#pragma mark - createGraphView

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    if (_activePaintView == self)
        _activePaintView = nil;
    delete _adapter;
    [super DEALLOC];
}

- (void)initView {
    self.opaque = NO;                               // 透明背景
    self.multipleTouchEnabled = YES;                // 检测多个触点
    self.contentMode = UIViewContentModeRedraw;     // 避免转屏变形
    
    GiCoreView::setScreenDpi(giGetScreenDpi());
    [self setupGestureRecognizers];
    _adapter->coreView()->setPenWidthRange(_adapter, 0.5f, -1);
    
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didEnteredBackground:)
                                                 name:UIApplicationDidEnterBackgroundNotification object:nil];
    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(willEnterForeground:)
                                                 name:UIApplicationWillEnterForegroundNotification object:nil];
}

- (id)initWithCoder:(NSCoder *)aDecoder {
    self = [super initWithCoder:aDecoder];
    if (self) {
        _activePaintView = self;
        _adapter = new GiViewAdapter(self, NULL, GIViewFlagsNoBackLayer|GIViewFlagsNoDynDrawView);
        [self initView];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = 0xFF;               // 自动适应大小
        _activePaintView = self;                    // 设置为当前绘图视图
        _adapter = new GiViewAdapter(self, NULL, 0);
        _adapter->coreView()->onSize(_adapter, frame.size.width, frame.size.height);
        [self initView];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame flags:(int)f {
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = 0xFF;
        _adapter = new GiViewAdapter(self, NULL, f);
        _adapter->coreView()->onSize(_adapter, frame.size.width, frame.size.height);
        [self initView];
        if (!(f & GIViewFlagsNoCmd)) {
            _activePaintView = self;
        }
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame :(GiPaintView *)refView {
    self = [super initWithFrame:frame];
    if (self) {
        _mainView = refView;
        _adapter = new GiViewAdapter(self, [refView viewAdapter2], 0);
        _adapter->coreView()->onSize(_adapter, frame.size.width, frame.size.height);
        [self initView];
    }
    return self;
}

- (void) setFrame:(CGRect)frame {
    [super setFrame:frame];
    if (_adapter) {
        _adapter->coreView()->onSize(_adapter, frame.size.width, frame.size.height);
        if ( ! (_adapter->getFlags() & GIViewFlagsZoomExtent)
            || !_adapter->coreView()->zoomToExtent()) {
            _adapter->regenAll(false);
        }
        
        for (size_t n = _adapter->delegates.size(), i = 0; i < n; i++) {
            if ([_adapter->delegates[i] respondsToSelector:@selector(onResizeFrame:)]) {
                [_adapter->delegates[i] onResizeFrame:self];
            }
        }
    }
}

+ (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView :(int)flags {
    GiPaintView *v = [[[GiPaintView alloc]initWithFrame:frame flags:flags] AUTORELEASE];
    if (parentView) {
        [parentView addSubview:v];
    }
    return v;
}

+ (GiPaintView *)createMagnifierView:(CGRect)frame
                             refView:(GiPaintView *)refView
                          parentView:(UIView *)parentView
{
    refView = refView ? refView : [GiPaintView activeView];
    if (!refView)
        return nil;
    
    GiPaintView *v = [[[GiPaintView alloc]initWithFrame:frame :refView] AUTORELEASE];
    if (parentView) {
        [parentView addSubview:v];
    }
    
    return v;
}

- (NSInteger)flags {
    return _adapter->getFlags();
}

- (void)setFlags:(NSInteger)f {
    int old = _adapter->setFlags((int)f);
    
    if ((old & GIViewFlagsMagnifier) && !(f & GIViewFlagsMagnifier)) {
        [_magnifierView hide];
        [_magnifierView RELEASEOBJ];
        _magnifierView = nil;
    }
}

- (void)setViewToMagnify:(UIView *)v {
    _viewToMagnify = v;
    if (v)
        self.flags |= GIViewFlagsMagnifier;
    else
        self.flags &= ~GIViewFlagsMagnifier;
}

- (void)didEnteredBackground:(NSNotification*)notification {
    [self clearCachedData];
    _adapter->coreView()->onPause(getTickCount());
}

- (void)willEnterForeground:(NSNotification*)notification {
    _adapter->coreView()->onResume(getTickCount());
}

#pragma mark - GiPaintView drawRect

- (void)drawRect:(CGRect)rect {
    _adapter->coreView()->onSize(_adapter, self.bounds.size.width, self.bounds.size.height);
    UIView *dynview = _adapter->getDynView(false);
    if (dynview && dynview != self && !CGRectEqualToRect(dynview.bounds, self.bounds)) {
        [dynview setFrame:self.frame];
    }
    
    if (!_adapter->renderInContext(UIGraphicsGetCurrentContext())) {
        _adapter->regenAll(false);
    } else if (!(self.flags & GIViewFlagsNotDynDraw)) {
        [GiDynDrawView draw:_adapter];
    }
}

+ (GiPaintView *)activeView {
    return _activePaintView;
}

- (GiView *)viewAdapter {
    return _adapter;
}

- (GiViewAdapter *)viewAdapter2 {
    return _adapter;
}

- (GiCoreView *)coreView {
    return _adapter->coreView();
}

- (GiImageCache *)imageCache {
    return _adapter->imageCache();
}

- (id<NSLocking>)locker {
    return _adapter->locker();
}

- (long)acquireFrontDoc {
    return _adapter->acquireFrontDoc();
}

- (void)releaseDoc:(long)doc {
    [self coreView]->releaseDoc(doc);
}

- (UIImage *)snapshotCG {
    float scale = [UIScreen mainScreen].scale;
    CGSize size = self.bounds.size;
    
    size.width *= scale;
    size.height *= scale;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate(NULL, size.width, size.height, 8, size.width * 4,
                                             colorSpace, kCGImageAlphaPremultipliedLast);
    CGColorSpaceRelease(colorSpace);
    
    CGContextClearRect(ctx, CGRectMake(0, 0, size.width, size.height));
    
    CGContextTranslateCTM(ctx, 0, size.height);
    CGContextScaleCTM(ctx, scale, - scale);
    
    if (self.window) {
        [self.layer renderInContext:ctx];
    } else {
        _adapter->renderInContext(ctx);
    }
    
    CGImageRef cgimage = CGBitmapContextCreateImage(ctx);
    UIImage *image = [UIImage imageWithCGImage:cgimage];
    
    CGImageRelease(cgimage);
    CGContextRelease(ctx);
    
    return image;
}

- (UIImage *)snapshot {
    if (![NSThread isMainThread]) {
        return [self snapshotCG];
    }
    
    [self hideContextActions];
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, self.opaque, 0);
    
    if (self.window) {
        [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    } else {
        _adapter->renderInContext(UIGraphicsGetCurrentContext());
    }
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}

- (BOOL)exportPNG:(NSString *)filename {
    UIImage *image = [self snapshot];
    BOOL ret = [UIImagePNGRepresentation(image) writeToFile:filename atomically:NO];
    if (ret) {
        NSLog(@"exportPNG: %@, %d, %.0fx%.0f@%.0fx",
              filename, ret, image.size.width, image.size.height, image.scale);
    }
    return ret;
}

- (void)setBackgroundColor:(UIColor *)color {
    [super setBackgroundColor:color];
    
    if (!color && self.superview) {
        color = self.superview.backgroundColor;
        if (!color && self.superview.superview) {
            color = self.superview.superview.backgroundColor;
        }
    }
    if (color && _adapter) {
        [self coreView]->setBkColor(_adapter, CGColorToGiColor(color.CGColor).getARGB());
    }
}

- (void)clearCachedData {
    _adapter->clearCachedData();
    [self.imageCache clearCachedData];
}

- (void)setGestureEnabled:(BOOL)enabled {
    for (int i = 0; _recognizers[i]; i++) {
        _recognizers[i].enabled = enabled;
    }
    _gestureEnabled = enabled;
    self.userInteractionEnabled = enabled;
}

- (UIView *)dynamicShapeView:(BOOL)create {
    return _adapter->getDynView(!!create);
}

- (void)activiteView {
    if (_activePaintView != self) {
        _activePaintView = self;
    }
}

- (NSArray *)delegates {
    NSMutableArray *arr = [NSMutableArray array];
    for (size_t i = 0; i < _adapter->delegates.size(); i++) {
        [arr addObject:_adapter->delegates[i]];
    }
    return arr;
}

- (void)addDelegate:(id<GiPaintViewDelegate>)d {
    if (d) {
        [self removeDelegate:d];
        _adapter->delegates.push_back(d);
        _adapter->respondsTo.didCommandChanged |= [d respondsToSelector:@selector(onCommandChanged:)];
        _adapter->respondsTo.didSelectionChanged |= [d respondsToSelector:@selector(onSelectionChanged:)];
        _adapter->respondsTo.didContentChanged |= [d respondsToSelector:@selector(onContentChanged:)];
        _adapter->respondsTo.didDynamicChanged |= [d respondsToSelector:@selector(onDynamicChanged:)];
        _adapter->respondsTo.didZoomChanged |= [d respondsToSelector:@selector(onZoomChanged:)];
        _adapter->respondsTo.didDynDrawEnded |= [d respondsToSelector:@selector(onDynDrawEnded:)];
        _adapter->respondsTo.didShapesRecorded |= [d respondsToSelector:@selector(onShapesRecorded:)];
        _adapter->respondsTo.didShapeWillDelete |= [d respondsToSelector:@selector(onShapeWillDelete:)];
        _adapter->respondsTo.didShapeDeleted |= [d respondsToSelector:@selector(onShapeDeleted:)];
        _adapter->respondsTo.didShapeDblClick |= [d respondsToSelector:@selector(onShapeDblClick:)];
        _adapter->respondsTo.didShapeClicked |= [d respondsToSelector:@selector(onShapeClicked:)];
        _adapter->respondsTo.didGestureShouldBegin |= [d respondsToSelector:@selector(onGestureShouldBegin:)];
        _adapter->respondsTo.didGestureBegan |= [d respondsToSelector:@selector(onGestureBegan:)];
        _adapter->respondsTo.didGestureEnded |= [d respondsToSelector:@selector(onGestureEnded:)];
    }
}

- (void)removeDelegate:(id<GiPaintViewDelegate>)d {
    for (size_t i = 0; i < _adapter->delegates.size(); i++) {
        if (_adapter->delegates[i] == d) {
            _adapter->delegates.erase(_adapter->delegates.begin() + i);
            break;
        }
    }
}

- (BOOL)contextActionEnabled {
    return _adapter->getContextActionEnabled();
}

- (void)setContextActionEnabled:(BOOL)enabled {
    _adapter->setContextActionEnabled(!!enabled);
}

- (void)hideContextActions {
    _adapter->hideContextActions();
}

- (IBAction)onContextAction:(id)sender {
    UIView *btn = (UIView *)sender;
    int action = btn ? (int)btn.tag : 0;
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self
                                             selector:@selector(hideContextActions) object:nil];
    _adapter->hideContextActions();
    @synchronized(_adapter->locker()) {
        if (![self coreView]->doContextAction(action)
            && _extActions.find(action) != _extActions.end()) {
            _extActions[action]();
        }
    }
}

+ (void)addContextAction:(int)action block:(dispatch_block_t)block {
    _extActions[action] = block;
}

- (void)removeFromSuperview {
    [self tearDown];
    [super removeFromSuperview];
}

- (void)tearDown {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    _adapter->hideContextActions();
    _adapter->stopRegen();
    _adapter->stopRecord(false);
    _adapter->stopRecord(true);
    if (_magnifierView) {
        [_magnifierView hide];
        [_magnifierView RELEASEOBJ];
        _magnifierView = nil;
    }
    self.gestureEnabled = NO;
    for (int i = 0; _recognizers[i]; i++) {
        [self removeGestureRecognizer:_recognizers[i]];
        [_recognizers[i] RELEASEOBJ];
        _recognizers[i] = nil;
    }
    _mainView = nil;
    if (_activePaintView == self)
        _activePaintView = nil;
}

- (void)undo {
    _adapter->undo();
}

- (void)redo {
    _adapter->redo();
}

- (void)stopRecord:(BOOL)forUndo {
    _adapter->stopRecord(forUndo);
}

@end

#pragma mark - GestureRecognizer
@implementation GiPaintView(GestureRecognizer)

- (void)setupGestureRecognizers {
    int i = 0;
    
    _gestureEnabled = self.userInteractionEnabled;
    if (!_gestureEnabled || (_adapter->getFlags() & GIViewFlagsNoCmd)) {
        return;
    }
    
    _recognizers[i++] = _pinchRecognizer =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(moveHandler:)];
    
    _recognizers[i++] = _rotationRecognizer =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(moveHandler:)];
    
    _recognizers[i++] = _panRecognizer =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panHandler:)];
    _panRecognizer.maximumNumberOfTouches = 2;                      // 允许单指拖动变为双指拖动
    
    _recognizers[i++] = _tapRecognizer =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapHandler:)];
    [_tapRecognizer requireGestureRecognizerToFail:_panRecognizer]; // 不是拖动才算点击
    
    _recognizers[i++] = _twoTapsRecognizer =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoTapsHandler:)];
    _twoTapsRecognizer.numberOfTapsRequired = 2;
    
    _recognizers[i++] = _pressRecognizer =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(pressHandler:)];
    _pressRecognizer.minimumPressDuration = 1;
    
    _recognizers[i] = nil;
    for (i--; i >= 0; i--) {
        _recognizers[i].delegate = self;
        [self addGestureRecognizer:_recognizers[i]];
    }
}

// 某个手指接触到屏幕，先于 gestureRecognizerShouldBegin 发生
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint pt = [touch locationInView:touch.view];
    
    if (!_gestureRecognized) {
        [self activiteView];
        
        _touchCount += [touches count];                     // 累计触点
        _points.clear();                                    // 清除触摸轨迹
        if (_touchCount == 1) {                             // 是第一个触点
            _points.push_back(pt);                          // 记下起点
            _startPt = pt;
        }
    }
    
    [super touchesBegan:touches withEvent:event];
}

// 某个手指在屏幕上移动，在拖动手势生效前会触发本事件
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    
    if (!_gestureRecognized && !_points.empty()) {          // 已经记录了轨迹
        CGPoint pt = [touch locationInView:touch.view];
        _points.push_back(pt);                              // 继续记录轨迹
    }
    
    [super touchesMoved:touches withEvent:event];
}

// 手势没有生效、手指松开时会触发本事件
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    
    if (!_gestureRecognized && _gestureEnabled) {
        _touchCount -= [touches count];                     // 累计触点
        
        if (!_points.empty()) {                             // 手势未生效，模拟分发手势
            [self dispatchTapPending];                      // 分发挂起的单击手势
            
            if (_adapter->dispatchPan(kGiGestureBegan, _points[0])) {
                for (int i = 1; i < _points.size(); i++) {
                    _adapter->dispatchPan(kGiGestureMoved, _points[i]);
                }
                CGPoint pt = [touch locationInView:touch.view];
                _adapter->dispatchPan(kGiGestureEnded, pt);
            }
            _points.clear();
        }
    }
    
    [super touchesEnded:touches withEvent:event];
}

// 手势即将开始，在 touchesBegan 后发生，即将调用本类的相应手势响应函数
- (BOOL)gestureRecognizerShouldBegin:(UIGestureRecognizer *)recognizer {
    if (_points.empty()) {
        _startPt = [recognizer locationInView:recognizer.view]; // 为拖动和单击修正起点
    }
    if (recognizer != _tapRecognizer && recognizer != _twoTapsRecognizer) {   // 非点击的手势生效前
        [self dispatchTapPending];                              // 分发挂起的单击手势
    }
    
    BOOL allow = (!_buttonHandled ||
                  (recognizer != _tapRecognizer && recognizer != _pressRecognizer));
    
    _buttonHandled = NO;
    _ignorePt = CGPointMake(-1, -1);
    
    // 将状态为 UIGestureRecognizerStatePossible 的手势传递到内核，看是否允许此手势
    if (!allow) {}
    else if (recognizer == _pinchRecognizer || recognizer == _rotationRecognizer
             || recognizer == _panRecognizer) {
        allow = [self moveHandler:recognizer];
    }
    else if (recognizer == _tapRecognizer) {
        allow = [self tapHandler:(UITapGestureRecognizer *)recognizer];
    }
    else if (recognizer == _twoTapsRecognizer) {
        allow = [self twoTapsHandler:(UITapGestureRecognizer *)recognizer];
    }
    else if (recognizer == _pressRecognizer) {
        allow = [self pressHandler:(UILongPressGestureRecognizer *)recognizer];
    }
    
    return allow;
}

- (BOOL)onGestureShouldBegin_:(UIGestureRecognizer*)sender {
    size_t n = _adapter->respondsTo.didGestureShouldBegin ? _adapter->delegates.size() : 0;
    
    for (size_t i = 0; i < n; i++) {
        if ([_adapter->delegates[i] respondsToSelector:@selector(onGestureShouldBegin:)]) {
            if (![_adapter->delegates[i] onGestureShouldBegin:sender])
                return NO;
        }
    }
    if ([self respondsToSelector:@selector(onGestureShouldBegin:)]) {
        if (![self performSelector:@selector(onGestureShouldBegin:) withObject:sender])
            return NO;
    }
    
    return YES;
}

- (void)onGestureBegan_:(UIGestureRecognizer*)sender {
    size_t n = _adapter->respondsTo.didGestureBegan ? _adapter->delegates.size() : 0;
    
    for (size_t i = 0; i < n; i++) {
        if ([_adapter->delegates[i] respondsToSelector:@selector(onGestureBegan:)]) {
            [_adapter->delegates[i] onGestureBegan:sender];
        }
    }
    if ([self respondsToSelector:@selector(onGestureBegan:)]) {
        [self performSelector:@selector(onGestureBegan:) withObject:sender];
    }
}

- (void)onGestureEnded_:(UIGestureRecognizer*)sender {
    size_t n = _adapter->respondsTo.didGestureEnded ? _adapter->delegates.size() : 0;
    
    for (size_t i = 0; i < n; i++) {
        if ([_adapter->delegates[i] respondsToSelector:@selector(onGestureEnded:)]) {
            [_adapter->delegates[i] onGestureEnded:sender];
        }
    }
    if ([self respondsToSelector:@selector(onGestureEnded:)]) {
        [self performSelector:@selector(onGestureEnded:) withObject:sender];
    }
}

- (BOOL)gestureCheck:(UIGestureRecognizer*)sender {
    _gestureRecognized = (sender.state == UIGestureRecognizerStateBegan
                          || sender.state == UIGestureRecognizerStateChanged);
    
    if (sender.state == UIGestureRecognizerStatePossible
        && _gestureEnabled && ![self onGestureShouldBegin_:sender]) {
        return NO;
    }
    
    if (!self.viewToMagnify && sender.state == UIGestureRecognizerStateBegan
        && (_adapter->getFlags() & GIViewFlagsMagnifier)) {
        if (_adapter->getFlags() & GIViewFlagsNoDynDrawView)
            self.viewToMagnify = self;
        else
            self.viewToMagnify = self.superview;
    }
    if (sender.state == UIGestureRecognizerStateBegan
        && [sender numberOfTouches] == 1
        && self.viewToMagnify && !self.mainView
        && _adapter->canShowMagnifier())
    {
        if (!_magnifierView) {
            _magnifierView = [[GiMagnifierView alloc]init];
            _magnifierView.followFinger = YES;
            _magnifierView.viewToMagnify = self.viewToMagnify;
        }
        [_magnifierView show];
        _magnifierView.touchPoint = [sender locationInView:_magnifierView.viewToMagnify];
    }
    else if (sender.state == UIGestureRecognizerStateChanged
             && _magnifierView && _magnifierView.window) {
        _magnifierView.touchPoint = [sender locationInView:_magnifierView.viewToMagnify];
    }
    
    return _gestureEnabled;
}

- (BOOL)gesturePost:(UIGestureRecognizer*)sender {
    if (sender.state == UIGestureRecognizerStateBegan) {
        _points.clear();
        [self onGestureBegan_:sender];
    }
    else if (sender.state >= UIGestureRecognizerStateEnded) {
        _touchCount = 0;
        _points.clear();
        [_magnifierView hide];
        [self onGestureEnded_:sender];
    }
    
    return YES;
}

- (BOOL)panHandler:(UIPanGestureRecognizer *)sender {
    CGPoint velocity = [sender velocityInView:sender.view];
    [self coreView]->setGestureVelocity(_adapter, velocity.x, velocity.y);
    return [self moveHandler:sender];
}

- (BOOL)moveHandler:(UIGestureRecognizer *)sender {
    if (![self gestureCheck:sender]) {
        return NO;
    }
    
    CGPoint pt = [sender locationInView:sender.view];
    const int touchCount = (int)[sender numberOfTouches];
    BOOL ret = YES;
    
    if (sender.state == UIGestureRecognizerStatePossible) { // 检查手势的有效性
        _touchCount = touchCount;
        _lastPt = _startPt;
        return (touchCount > 1 ? _adapter->twoFingersMove(sender)
                : _adapter->dispatchPan(kGiGesturePossible, _startPt));
    }
    
    if (sender.state == UIGestureRecognizerStateBegan
        && touchCount == 1 && !_points.empty()) {           // 单指拖动开始，分发手势前的触摸轨迹
        ret = _adapter->dispatchPan(kGiGestureBegan, _points[0]);
        for (int i = 1; i < _points.size(); i++) {
            _adapter->dispatchPan(kGiGestureMoved, _points[i]);
        }
        _adapter->dispatchPan(kGiGestureMoved, pt);
        _lastPt = pt;
    }
    else if (sender.state == UIGestureRecognizerStateChanged
             && _touchCount != touchCount) {                // 切换单指与双指模式，就结束前一模式
        if (_touchCount > 1) {                              // 双指变为单指
            _adapter->twoFingersMove(sender, kGiGestureEnded, true);    // 结束双指模式
            _adapter->dispatchPan(kGiGestureBegan, pt, true);           // 从当前位置开始单指拖动
            _touchCount = touchCount;
            _lastPt = pt;
        }
        else if (_adapter->twoFingersMove(sender, kGiGesturePossible)) {    // 单指变为双指
            _adapter->dispatchPan(kGiGestureEnded, _lastPt, true);      // 结束单指拖动
            _adapter->twoFingersMove(sender, kGiGestureBegan, true);    // 开始双指操作
            _touchCount = touchCount;
        }
    }
    else if (_touchCount == touchCount                      // 忽略不支持的双指移动动作
             || sender.state >= UIGestureRecognizerStateEnded) {
        if (_touchCount > 1) {
            ret = _adapter->twoFingersMove(sender);
        }
        else {
            ret = _adapter->dispatchPan((GiGestureState)sender.state, touchCount == 1 ? pt : _lastPt);
            _lastPt = pt;
        }
    }
    
    return [self gesturePost:sender] && ret;
}

- (BOOL)tapHandler:(UITapGestureRecognizer *)sender {
    CGPoint pt = [sender locationInView:sender.view];
    
    if (![self gestureCheck:sender]) {
        return NO;
    }
    if (sender.state == UIGestureRecognizerStatePossible) {
        return (_tapCount == 1 ||       // 前一点击还未分发则跳过
                _adapter->dispatchGesture(kGiGestureTap, kGiGesturePossible, _startPt));
    }
    if (_tapCount == 1) {
        [self dispatchTapPending];      // 分发挂起的单击手势
        if (!_adapter->dispatchGesture(kGiGestureTap, kGiGesturePossible, pt)) { // 补发Possible
            return NO;
        }
    }
    
    _tapCount = 1;      // 标记待分发
    _tapPoint = pt;     // 记下点击松开的位置
    if (_twoTapsRecognizer && _twoTapsRecognizer.enabled) {
        [self performSelector:@selector(delayTap) withObject:nil afterDelay:0.5];   // 延时分发
    } else {
        [self delayTap];
    }
    
    return [self gesturePost:sender];
}

- (void)delayTap {
    if (_tapCount == 1) {
        _adapter->dispatchGesture(kGiGestureTap, kGiGestureEnded, _tapPoint);
        _tapCount = 0;
    }
}

- (void)dispatchTapPending {
    if (_tapCount == 1) {                           // 单击还未分发
        _tapCount = 0;
        _adapter->dispatchGesture(kGiGestureTap, kGiGestureEnded, _tapPoint);
        [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayTap) object:nil];
    }
}

- (BOOL)twoTapsHandler:(UITapGestureRecognizer *)sender {
    if (![self gestureCheck:sender]) {
        return NO;
    }
    if (sender.state == UIGestureRecognizerStatePossible) {
        return _adapter->dispatchGesture(kGiGestureDblTap, kGiGesturePossible, _startPt);
    }
    
    _tapCount = 0;          // 双击时忽略单击
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(delayTap) object:nil];
    
    return ([self gesturePost:sender]
            && _adapter->dispatchGesture(kGiGestureDblTap, kGiGestureEnded,
                                         [sender locationInView:sender.view]));
}

- (BOOL)pressHandler:(UILongPressGestureRecognizer *)sender {
    return ([self gestureCheck:sender]
            && [self gesturePost:sender]
            && _adapter->dispatchGesture(kGiGesturePress, (GiGestureState)sender.state,
                                         [sender locationInView:sender.view]));
}

@end

@implementation GiPaintView(GestureInternel)

- (void)ignoreTouch:(CGPoint)pt :(UIView *)handledButton {
    if (handledButton) {
        _buttonHandled = YES;
    }
    if (CGPointEqualToPoint(_ignorePt, pt) && _adapter->isContextActionsVisible()) {
        [self performSelector:@selector(hideContextActions) withObject:nil afterDelay:1];
    }
    _ignorePt = pt;
}

- (void)redrawForDelay:(id)changed {
    _adapter->redraw(!!changed);
}

- (void)onContextActionsDisplay:(NSMutableArray *)buttons {
    _buttonHandled = NO;
}

@end
