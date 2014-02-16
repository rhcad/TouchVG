//! \file GiPaintView.mm
//! \brief 实现iOS绘图视图类 GiPaintView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiViewImpl.h"
#import "ImageCache.h"

#pragma mark - IosTempView
@implementation IosTempView

- (id)initView:(CGRect)frame :(GiViewAdapter *)adapter {
    self = [super initWithFrame:frame];
    if (self) {
        _adapter = adapter;
        self.opaque = NO;                           // 透明背景
        self.userInteractionEnabled = NO;           // 禁止交互，避免影响主视图显示
    }
    return self;
}

- (void)drawRect:(CGRect)rect {
    GiCanvasAdapter canvas(_adapter->imageCache());
    GiCoreView* coreView = _adapter->coreView();
    long hDoc = _adapter->getAppendCount() > 0 ? coreView->acquireFrontDoc() : 0;
    long hShapes = coreView->acquireDynamicShapes();
    long hGs = coreView->acquireGraphics(_adapter);
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext())) {
        for (int i = 0, sid = 0; (sid = _adapter->getAppendID(i)) != 0; i++) {
            coreView->drawAppend(hDoc, hGs, &canvas, sid);
        }
        coreView->dynDraw(hShapes, hGs, &canvas);
        canvas.endPaint();
    }
    GiCoreView::releaseShapes(hShapes);
    coreView->releaseGraphics(hGs);
}

@end

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
    dispatch_release(_queue);
    [_layer RELEASE];
    [super DEALLOC];
}

- (CALayer *)getLayer {
    return _layer;
}

- (void)startRender:(long)doc :(long)gs {
    _doc = doc;
    _gs = gs;
    [self startRender_:NO];
}

- (void)startRenderForPending {
    [self startRender_:YES];
}

- (void)startRender_:(BOOL)forPending {
    if (forPending) {
        if (_adapter->getAppendCount() == 0 && _drawing == 0) {
            return;
        }
        _drawing = 0;
    }
    if (++_drawing == 1) {
        dispatch_async(_queue, ^{
            CALayer *srcLayer = _adapter->mainView().layer;
            
            if (!_layer) {
                _layer = [[CALayer alloc]init];
                _layer.delegate = self;
                _layer.contentsScale = srcLayer.contentsScale;
            }
            
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
    
    if (!_doc) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            _adapter->beginRender();
            _doc = coreView->acquireFrontDoc();
            _gs = coreView->acquireGraphics(_adapter);
        });
    }
    if (canvas.beginPaint(ctx)) {
        CGContextClearRect(ctx, _adapter->mainView().bounds);
        coreView->drawAll(_doc, _gs, &canvas);
        canvas.endPaint();
    }
    GiCoreView::releaseDoc(_doc);
    coreView->releaseGraphics(_gs);
    _doc = 0;
    _gs = 0;
    
    dispatch_async(dispatch_get_main_queue(), ^{
        [_adapter->mainView() setNeedsDisplay];
        --_drawing;
    });
}

@end

static GiPaintView* _activePaintView = nil;
GiColor CGColorToGiColor(CGColorRef color);

@implementation GiPaintView

@synthesize panRecognizer = _panRecognizer;
@synthesize tapRecognizer = _tapRecognizer;
@synthesize twoTapsRecognizer = _twoTapsRecognizer;
@synthesize pressRecognizer = _pressRecognizer;
@synthesize pinchRecognizer = _pinchRecognizer;
@synthesize rotationRecognizer = _rotationRecognizer;
@synthesize gestureEnabled = _gestureEnabled;
@synthesize imageCache;

#pragma mark - Respond to low-memory warnings
+ (void)initialize {
	if (self == [GiPaintView class]) {  // Have to protect against subclasses calling this
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(didReceiveMemoryWarningNotification:) name:UIApplicationDidReceiveMemoryWarningNotification object:nil];
	}
}

+ (void)didReceiveMemoryWarningNotification:(NSNotification*)notification {
	[_activePaintView clearCachedData];
}

#pragma mark - createGraphView

- (void)dealloc {
    if (_activePaintView == self)
        _activePaintView = nil;
    delete _adapter;
    [super DEALLOC];
}

- (void)initView:(GiView*)mainView :(GiCoreView*)coreView {
    self.opaque = NO;                               // 透明背景
    self.multipleTouchEnabled = YES;                // 检测多个触点
    
    GiCoreView::setScreenDpi(giGetScreenDpi());
    [self setupGestureRecognizers];
    
    if (mainView && coreView) {
        _adapter = new GiViewAdapter(self, coreView);
        coreView->createMagnifierView(_adapter, mainView);
    }
    else {
        _adapter = new GiViewAdapter(self, NULL);
        _adapter->coreView()->createView(_adapter);
    }
    _adapter->coreView()->setPenWidthRange(_adapter, 0.5f, -1);
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = 0xFF;               // 自动适应大小
        _activePaintView = self;                    // 设置为当前绘图视图
        [self initView:NULL :NULL];
        [self coreView]->onSize(_adapter, frame.size.width, frame.size.height);
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame :(GiPaintView *)refView {
    self = [super initWithFrame:frame];
    if (self) {
        [self initView:[refView viewAdapter] :[refView coreView]];
    }
    return self;
}

+ (GiPaintView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    GiPaintView *v = [[GiPaintView alloc]initWithFrame:frame];
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
    
    GiPaintView *v = [[GiPaintView alloc]initWithFrame:frame :refView];
    if (parentView) {
        [parentView addSubview:v];
    }
    
    return v;
}

#pragma mark - GiPaintView drawRect

- (void)drawRect:(CGRect)rect {
    _adapter->coreView()->onSize(_adapter, self.bounds.size.width, self.bounds.size.height);
    if (!_adapter->renderInContext(UIGraphicsGetCurrentContext())) {
        _adapter->regenAll(true);
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

- (ImageCache *)imageCache {
    return _adapter->imageCache();
}

- (UIImage *)snapshot {
    [self hideContextActions];
    
    UIGraphicsBeginImageContextWithOptions(self.bounds.size, self.opaque, 0);
    [self.layer renderInContext:UIGraphicsGetCurrentContext()];
    
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return image;
}

- (BOOL)savePng:(NSString *)filename {
    UIImage *image = [self snapshot];
    BOOL ret = [UIImagePNGRepresentation(image) writeToFile:filename atomically:NO];
    if (ret) {
        NSLog(@"savePng: %@, %d, %.0fx%.0f@%.0fx",
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
    if (color) {
        [self coreView]->setBkColor(_adapter, CGColorToGiColor(color.CGColor).getARGB());
    }
}

- (void)clearCachedData {
    _adapter->clearCachedData();
    [self.imageCache clearCachedData];
}

- (void)setGestureEnabled:(BOOL)enabled {
    UIGestureRecognizer *recognizers[] = {
        _pinchRecognizer, _rotationRecognizer, _panRecognizer,
        _tapRecognizer, _twoTapsRecognizer, _pressRecognizer, nil
    };
    for (int i = 0; recognizers[i]; i++) {
        recognizers[i].enabled = enabled;
    }
    _gestureEnabled = enabled;
    self.userInteractionEnabled = enabled;
}

- (UIView *)dynamicShapeView {
    return _adapter->getDynView();
}

- (void)activiteView {
    if (_activePaintView != self) {
        _activePaintView = self;
    }
}

- (void)addDelegate:(id<GiPaintViewDelegate>)d {
    if (d) {
        [self removeDelegate:d];
        _adapter->delegates.push_back(d);
        _adapter->respondsTo.didCommandChanged |= [d respondsToSelector:@selector(onCommandChanged:)];
        _adapter->respondsTo.didSelectionChanged |= [d respondsToSelector:@selector(onSelectionChanged:)];
        _adapter->respondsTo.didContentChanged |= [d respondsToSelector:@selector(onContentChanged:)];
        _adapter->respondsTo.didDynamicChanged |= [d respondsToSelector:@selector(onDynamicChanged:)];
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
    [self coreView]->doContextAction(action);
}

- (void)removeFromSuperview {
    [self tearDown];
    [super removeFromSuperview];
}

- (void)tearDown {
    [NSObject cancelPreviousPerformRequestsWithTarget:self];
    _adapter->stopRegen();
    _adapter->stopRecord(false);
    _adapter->stopRecord(true);
    self.gestureEnabled = NO;
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
    UIGestureRecognizer *recognizers[7];
    int i = 0;
    
    _gestureEnabled = self.userInteractionEnabled;
    if (!_gestureEnabled)
        return;
    
    recognizers[i++] = _pinchRecognizer =
    [[UIPinchGestureRecognizer alloc] initWithTarget:self action:@selector(panHandler:)];
    
    recognizers[i++] = _rotationRecognizer =
    [[UIRotationGestureRecognizer alloc] initWithTarget:self action:@selector(panHandler:)];
    
    recognizers[i++] = _panRecognizer =
    [[UIPanGestureRecognizer alloc] initWithTarget:self action:@selector(panHandler:)];
    _panRecognizer.maximumNumberOfTouches = 2;                      // 允许单指拖动变为双指拖动
    
    recognizers[i++] = _tapRecognizer =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(tapHandler:)];
    [_tapRecognizer requireGestureRecognizerToFail:_panRecognizer]; // 不是拖动才算点击
    
    recognizers[i++] = _twoTapsRecognizer =
    [[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(twoTapsHandler:)];
    _twoTapsRecognizer.numberOfTapsRequired = 2;
    
    recognizers[i++] = _pressRecognizer =
    [[UILongPressGestureRecognizer alloc] initWithTarget:self action:@selector(pressHandler:)];
    _pressRecognizer.minimumPressDuration = 1;
    
    for (i--; i >= 0; i--) {
        recognizers[i].delegate = self;
        [self addGestureRecognizer:recognizers[i]];
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

- (void)ignoreTouch:(CGPoint)pt :(UIView *)handledButton {
    if (handledButton) {
        _buttonHandled = YES;
    }
    if (CGPointEqualToPoint(_ignorePt, pt) && _adapter->isContextActionsVisible()) {
        [self performSelector:@selector(hideContextActions) withObject:nil afterDelay:1];
    }
    _ignorePt = pt;
}

- (void)redrawForDelay {
    _adapter->redraw();
}

- (void)onContextActionsDisplay:(NSMutableArray *)buttons {
    _buttonHandled = NO;
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
        allow = [self panHandler:recognizer];
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

- (BOOL)gestureCheck:(UIGestureRecognizer*)sender {
    _gestureRecognized = (sender.state == UIGestureRecognizerStateBegan
                          || sender.state == UIGestureRecognizerStateChanged);
    return _gestureEnabled;
}

- (BOOL)gesturePost:(UIGestureRecognizer*)sender {
    if (sender.state == UIGestureRecognizerStateBegan) {
        _points.clear();
    }
    else if (sender.state >= UIGestureRecognizerStateEnded) {
        _touchCount = 0;
        _points.clear();
    }
    
    return YES;
}

- (BOOL)panHandler:(UIGestureRecognizer *)sender {
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
    [self performSelector:@selector(delayTap) withObject:nil afterDelay:0.5];   // 延时分发
    
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
