//! \file GiGraphView.mm
//! \brief 实现iOS绘图视图类 GiGraphView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphViewImpl.h"

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
    GiCanvasAdapter canvas;
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext())) {
        _adapter->coreView()->dynDraw(_adapter, &canvas);
        canvas.endPaint();
    }
}

@end

static GiGraphView* _activeGraphView = nil;
GiColor CGColorToGiColor(CGColorRef color);

@implementation GiGraphView

@synthesize panRecognizer = _panRecognizer;
@synthesize tapRecognizer = _tapRecognizer;
@synthesize twoTapsRecognizer = _twoTapsRecognizer;
@synthesize pressRecognizer = _pressRecognizer;
@synthesize pinchRecognizer = _pinchRecognizer;
@synthesize rotationRecognizer = _rotationRecognizer;
@synthesize gestureEnabled;

- (void)dealloc {
    if (_activeGraphView == self)
        _activeGraphView = nil;
    delete _adapter;
    [super dealloc];
}

- (void)initView:(GiView*)mainView :(GiCoreView*)coreView {
    self.opaque = NO;                               // 透明背景
    self.multipleTouchEnabled = YES;                // 检测多个触点
    
    GiCoreView::setScreenDpi(GiCanvasAdapter::getScreenDpi());
    [self setupGestureRecognizers];
    
    if (mainView && coreView) {
        _adapter = new GiViewAdapter(self, coreView);
        coreView->createMagnifierView(_adapter, mainView);
    }
    else {
        _adapter = new GiViewAdapter(self, NULL);
        _adapter->coreView()->createView(_adapter);
    }
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        self.autoresizingMask = 0xFF;               // 自动适应大小
        _activeGraphView = self;                    // 设置为当前绘图视图
        [self initView:NULL :NULL];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame :(GiGraphView *)refView {
    self = [super initWithFrame:frame];
    if (self) {
        [self initView:[refView viewAdapter] :[refView coreView]];
    }
    return self;
}

+ (GiGraphView *)createGraphView:(CGRect)frame :(UIView *)parentView {
    GiGraphView *v = [[GiGraphView alloc]initWithFrame:frame];
    [parentView addSubview:v];
    [v release];
    return v;
}

+ (GiGraphView *)createMagnifierView:(CGRect)frame
                              refView:(GiGraphView *)refView
                           parentView:(UIView *)parentView
{
    refView = refView ? refView : [GiGraphView activeView];
    if (!refView)
        return nil;
    
    GiGraphView *v = [[GiGraphView alloc]initWithFrame:frame :refView];
    [parentView addSubview:v];
    [v release];
    
    return v;
}

+ (GiGraphView *)activeView {
    return _activeGraphView;
}

- (GiView *)viewAdapter {
    return _adapter;
}

- (GiCoreView *)coreView {
    return _adapter->coreView();
}

- (int)cmdViewHandle {
    return _adapter->coreView()->viewAdapterHandle();
}

- (MgView *)cmdView {
    return _adapter->coreView()->viewAdapter();
}

- (UIImage *)snapshot {
    return _adapter->snapshot(true);
}

- (BOOL)savePng:(NSString *)filename {
    BOOL ret = NO;
    NSData* imageData = UIImagePNGRepresentation([self snapshot]);
    
    if (imageData) {
        ret = [imageData writeToFile:filename atomically:NO];
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

- (void)drawRect:(CGRect)rect {
    GiCoreView *coreView = _adapter->coreView();
    GiCanvasAdapter canvas;
    
    coreView->onSize(_adapter, self.bounds.size.width, self.bounds.size.height);
    
    if (canvas.beginPaint(UIGraphicsGetCurrentContext())) {
        if (!_adapter->drawAppend(&canvas)) {
            coreView->drawAll(_adapter, &canvas);
        }
        canvas.endPaint();
    }
}

- (void)redraw {
    _adapter->redraw();
}

- (void)clearCachedData {
    _adapter->clearCachedData();
}

- (BOOL)gestureEnabled {
    return self.userInteractionEnabled;
}

- (void)setGestureEnabled:(BOOL)enabled {
    UIGestureRecognizer *recognizers[] = {
        _pinchRecognizer, _rotationRecognizer, _panRecognizer, 
        _tapRecognizer, _twoTapsRecognizer, _pressRecognizer, nil
    };
    for (int i = 0; recognizers[i]; i++) {
        recognizers[i].enabled = enabled;
    }
    self.userInteractionEnabled = enabled;
}

- (void)activiteView {
    if (_activeGraphView != self) {
        _activeGraphView = self;
    }
}

- (void)addDelegate:(id<GiGraphViewDelegate>)d {
    if (d) {
        [self removeDelegate:d];
        _adapter->delegates.push_back(d);
        _adapter->respondsTo.didCommandChanged |= [d respondsToSelector:@selector(onCommandChanged:)];
        _adapter->respondsTo.didSelectionChanged |= [d respondsToSelector:@selector(onSelectionChanged:)];
        _adapter->respondsTo.didContentChanged |= [d respondsToSelector:@selector(onContentChanged:)];
    }
}

- (void)removeDelegate:(id<GiGraphViewDelegate>)d {
    for (size_t i = 0; i < _adapter->delegates.size(); i++) {
        if (_adapter->delegates[i] == d) {
            _adapter->delegates.erase(_adapter->delegates.begin() + i);
            break;
        }
    }
}

- (void)hideContextActions {
    _adapter->hideContextActions();
}

- (IBAction)onContextAction:(id)sender
{
    UIView *btn = (UIView *)sender;
    int action = btn ? btn.tag : 0;
    
    [NSObject cancelPreviousPerformRequestsWithTarget:self
                                             selector:@selector(hideContextActions) object:nil];
    _adapter->hideContextActions();
    [self coreView]->doContextAction(action);
}

@end

@implementation GiGraphView(GestureRecognizer)

- (void)setupGestureRecognizers {
    UIGestureRecognizer *recognizers[7];
    int i = 0;
    
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
    CGPoint pt = [touch locationInView:touch.view];
    
    if (!_gestureRecognized && !_points.empty()) {          // 已经记录了轨迹
        _points.push_back(pt);                              // 继续记录轨迹
    }
    
    [super touchesMoved:touches withEvent:event];
}

// 手势没有生效、手指松开时会触发本事件
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event {
    UITouch *touch = [touches anyObject];
    CGPoint pt = [touch locationInView:touch.view];
    
    if (!_gestureRecognized) {
        _touchCount -= [touches count];                     // 累计触点
        
        if (!_points.empty()) {                             // 手势未生效，模拟分发手势
            [self dispatchTapPending];                      // 分发挂起的单击手势
            
            if (_adapter->dispatchPan(kGiGestureBegan, _points[0])) {
                for (int i = 1; i < _points.size(); i++) {
                    _adapter->dispatchPan(kGiGestureMoved, _points[i]);
                }
                _adapter->dispatchPan(kGiGestureEnded, pt);
            }
            _points.clear();
        }
    }
    
    [super touchesEnded:touches withEvent:event];
}

- (void)ignoreTouch:(NSValue *)pointValue :(UIView *)handledButton {
    CGPoint pt = [self.window convertPoint:[pointValue CGPointValue] toView:self];
    
    if (handledButton) {
        _buttonHandled = YES;
    }
    if (CGPointEqualToPoint(_ignorePt, pt) && _adapter->isContextActionsVisible()) {
        [self performSelector:@selector(hideContextActions) withObject:nil afterDelay:0.5];
    }
    _ignorePt = pt;
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
    return YES;
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
    const int touchCount = [sender numberOfTouches];
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
