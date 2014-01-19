//! \file GiViewAdapter.mm
//! \brief 实现iOS绘图视图适配器 GiViewAdapter
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphViewImpl.h"
#import "ImageCache.h"
#import "ARCMacro.h"

static NSString* const CAPTIONS[] = { nil, @"全选", @"重选", @"绘图", @"取消",
    @"删除", @"克隆", @"定长", @"不定长", @"锁定", @"解锁", @"编辑", @"返回",
    @"闭合", @"不闭合", @"加点", @"删点", @"成组", @"解组", @"翻转",
};
static NSString* const IMAGENAMES[] = { nil, @"vg_selall.png", nil, @"vg_draw.png",
    @"vg_back.png", @"vg_delete.png", @"vg_clone.png", @"vg_fixlen.png",
    @"vg_freelen.png", @"vg_lock.png", @"vg_unlock.png", @"vg_edit.png",
    @"vg_endedit.png", nil, nil, @"vg_addvertex.png", @"vg_delvertex.png",
    @"vg_group.png", @"vg_ungroup.png", @"vg_overturn.png",
};
NSString* EXTIMAGENAMES[41] = { nil };
static const int EXTRA_ACTION_BEGIN = 40;

static int getExtraImageCount() { int n = 0; while (EXTIMAGENAMES[n]) n++; return n; }

//! Button class for showContextActions().
@interface UIButtonAutoHide : UIButton
@property (nonatomic,assign) GiGraphView *delegate;
@end

@implementation UIButtonAutoHide
@synthesize delegate;

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event {
	BOOL ret = [super pointInside:point withEvent:event];
    CGPoint pt = [self.window convertPoint:point fromView:self];
    
    [delegate ignoreTouch:pt :ret ? self : nil];
    
	return ret;
}

@end

#define APPENDSIZE sizeof(_appendIDs)/sizeof(_appendIDs[0])

GiViewAdapter::GiViewAdapter(GiGraphView *mainView, GiCoreView *coreView)
    : _view(mainView), _dynview(nil), _buttons(nil), _buttonImages(nil)
    , _actionEnabled(true), _oldAppendCount(0)
{
    _coreView = new GiCoreView(coreView);
    memset(&respondsTo, 0, sizeof(respondsTo));
    _imageCache = [[ImageCache alloc]init];
    _render = [[GiLayerRender alloc]initWithAdapter:this];
    
    for (int i = 0; i < APPENDSIZE; i++)
        _appendIDs[i] = 0;
}

GiViewAdapter::~GiViewAdapter() {
    [_buttons RELEASE];
    [_buttonImages RELEASE];
    [_imageCache RELEASE];
    [_render RELEASE];
    _coreView->destoryView(this);
    delete _coreView;
}

void GiViewAdapter::clearCachedData() {
    if (_buttonImages) {
        [_buttonImages removeAllObjects];
    }
    _coreView->clearCachedData();
}

int GiViewAdapter::getAppendCount() const {
    int n = 0;
    for (int i = 0; i < APPENDSIZE; i++) {
        if (_appendIDs[i] != 0) {
            n++;
        }
    }
    return n;
}

void GiViewAdapter::beginRender() {
    _oldAppendCount = getAppendCount();
}

bool GiViewAdapter::renderInContext(CGContextRef ctx) {
    if (![_render getLayer]) {
        return false;
    }
    
    [[_render getLayer] renderInContext:ctx];
    
    for (int i = 0, j = _oldAppendCount; i < APPENDSIZE; i++, j++) {
        _appendIDs[i] = j < APPENDSIZE ? _appendIDs[j] : 0;
    }
    if (_oldAppendCount > 0) {
        [_dynview setNeedsDisplay];
    }
    _oldAppendCount = 0;
    [_render startRender:YES];
    
    return true;
}

int GiViewAdapter::getAppendID(int index) const {
    return index < APPENDSIZE ? _appendIDs[index] : 0;
}

void GiViewAdapter::regenAll(bool changed) {
    if (isMainThread()) {
        regen_(changed, 0);
    }
    else {
        dispatch_async(dispatch_get_main_queue(), ^{ regen_(changed, 0); });
    }
}

void GiViewAdapter::regen_(bool changed, int sid) {
    if (!_view.window) {
        return;
    }
    if (changed) {
        _coreView->submitBackDoc(this);
    }
    _coreView->submitDynamicShapes(this);
    
    for (int i = 0; i < APPENDSIZE; i++) {
        if (_appendIDs[i] == 0) {
            _appendIDs[i] = sid;
            break;
        }
    }
    [_render startRender:NO];
    [_dynview setNeedsDisplay];
}

void GiViewAdapter::regenAppend(int sid) {
    if (isMainThread()) {
        regen_(true, sid);
    }
    else {
        dispatch_async(dispatch_get_main_queue(), ^{ regen_(true, sid); });
    }
}

void GiViewAdapter::stopRegen() {
    _coreView->stopDrawing(this);
}

UIView *GiViewAdapter::getDynView() {
    if (!_dynview && _view && _view.window) {
        _dynview = [[IosTempView alloc]initView:_view.frame :this];
        _dynview.autoresizingMask = _view.autoresizingMask;
        [_view.superview addSubview:_dynview];
        [_dynview RELEASE];
    }
    return _dynview;
}

void GiViewAdapter::redraw_() {
    if (getDynView()) {
        _coreView->submitDynamicShapes(this);
        [_dynview setNeedsDisplay];
    }
    else {
        [_view performSelector:@selector(redrawForDelay) withObject:nil afterDelay:0.2];
    }
}

void GiViewAdapter::redraw() {
    if (isMainThread()) {
        redraw_();
    }
    else {
        dispatch_async(dispatch_get_main_queue(), ^{ redraw_(); });
    }
}

bool GiViewAdapter::isMainThread() const {
    return dispatch_get_current_queue() == dispatch_get_main_queue();
}

bool GiViewAdapter::dispatchGesture(GiGestureType gestureType, GiGestureState gestureState, CGPoint pt) {
    return _coreView->onGesture(this, gestureType, gestureState, pt.x, pt.y);
}

bool GiViewAdapter::dispatchPan(GiGestureState gestureState, CGPoint pt, bool switchGesture) {
    return _coreView->onGesture(this, kGiGesturePan, gestureState, pt.x, pt.y, switchGesture);
}

bool GiViewAdapter::twoFingersMove(UIGestureRecognizer *sender, int state, bool switchGesture) {
    CGPoint pt1, pt2;
    
    if ([sender numberOfTouches] == 2) {
        pt1 = [sender locationOfTouch:0 inView:sender.view];
        pt2 = [sender locationOfTouch:1 inView:sender.view];
    }
    else {
        pt1 = [sender locationInView:sender.view];
        pt2 = pt1;
    }
    
    state = state < 0 ? (int)sender.state : state;
    return _coreView->twoFingersMove(this, (GiGestureState)state, 
                                     pt1.x, pt1.y, pt2.x, pt2.y, switchGesture);
}

void GiViewAdapter::hideContextActions() {
    if (_buttons) {
        for (UIView *button in _buttons) {
            [button removeFromSuperview];
        }
        [_buttons removeAllObjects];
    }
}

bool GiViewAdapter::isContextActionsVisible() {
    return _buttons && [_buttons count] > 0;
}

bool GiViewAdapter::showContextActions(const mgvector<int>& actions,
                                       const mgvector<float>& buttonXY,
                                       float x, float y, float w, float h) {
    int n = actions.count();
    UIView *btnParent = _view;
    
    if (n == 0 || !_actionEnabled) {
        hideContextActions();
        return true;
    }
    
    if (!_buttons) {
        _buttons = [[NSMutableArray alloc]init];
    }
    if ([_buttons count] > 0 && _coreView->isPressDragging()) {
        return false;
    }
    [NSObject cancelPreviousPerformRequestsWithTarget:_view
                                             selector:@selector(hideContextActions) object:nil];
    hideContextActions();
    
    for (int i = 0; i < n; i++) {
        const int action = actions.get(i);
        NSString *caption, *imageName;
        
        if (action > 0 && action < sizeof(CAPTIONS)/sizeof(CAPTIONS[0])) {
            caption = CAPTIONS[action];
            imageName = IMAGENAMES[action];
        }
        else if (action - EXTRA_ACTION_BEGIN >= 0
                 && action - EXTRA_ACTION_BEGIN < getExtraImageCount()) {
            caption = @"?";
            imageName = EXTIMAGENAMES[action - EXTRA_ACTION_BEGIN];
        }
        else {
            continue;
        }
        
        UIButtonAutoHide *btn = [[UIButtonAutoHide alloc]initWithFrame:CGRectNull];
        
        btn.delegate = _view;
        btn.tag = action;
        btn.showsTouchWhenHighlighted = YES;
        setContextButton(btn, caption, imageName);
        btn.center = CGPointMake(buttonXY.get(2 * i), buttonXY.get(2 * i + 1));
        
        [btn addTarget:_view action:@selector(onContextAction:) forControlEvents:UIControlEventTouchUpInside];
        btn.frame = [btnParent convertRect:btn.frame fromView:_view];
        [btnParent addSubview:btn];
        [_buttons addObject:btn];
        [btn RELEASE];
    }
    [_view performSelector:@selector(onContextActionsDisplay:) withObject:_buttons];
    
    return [_buttons count] > 0;
}

void GiViewAdapter::setContextButton(UIButton *btn, NSString *caption, NSString *imageName) {
    UIImage *image = nil;
    
    if (imageName) {
        if (!_buttonImages) {
            _buttonImages = [[NSMutableDictionary alloc]init];
        }
        imageName = [@"TouchVG.bundle/" stringByAppendingString:imageName];
        image = [_buttonImages objectForKey:imageName];
        if (!image) {
            image = [UIImage imageNamed:imageName];
            if (image) {
                [_buttonImages setObject:image forKey:imageName];
            }
        }
    }
    if (image) {
        [btn setImage:image forState: UIControlStateNormal];
        [btn setTitle:nil forState: UIControlStateNormal];
        btn.backgroundColor = [UIColor clearColor];
        btn.frame = CGRectMake(0, 0, 32, 32);
    }
    else if (caption) {
        [btn setTitle:caption forState: UIControlStateNormal];
        [btn setTitle:caption forState: UIControlStateHighlighted];
        [btn setTitleColor:[UIColor blackColor] forState: UIControlStateHighlighted];
        btn.backgroundColor = [UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:0.8];
        btn.frame = CGRectMake(0, 0, 60, 36);
    }
}

void GiViewAdapter::commandChanged() {
    for (size_t i = 0; i < delegates.size() && respondsTo.didCommandChanged; i++) {
        if ([delegates[i] respondsToSelector:@selector(onCommandChanged:)]) {
            [delegates[i] onCommandChanged:_view];
        }
    }
}

void GiViewAdapter::selectionChanged() {
    for (size_t i = 0; i < delegates.size() && respondsTo.didSelectionChanged; i++) {
        if ([delegates[i] respondsToSelector:@selector(onSelectionChanged:)]) {
            [delegates[i] onSelectionChanged:_view];
        }
    }
}

void GiViewAdapter::contentChanged() {
    for (size_t i = 0; i < delegates.size() && respondsTo.didContentChanged; i++) {
        if ([delegates[i] respondsToSelector:@selector(onContentChanged:)]) {
            [delegates[i] onContentChanged:_view];
        }
    }
}

void GiViewAdapter::dynamicChanged() {
    for (size_t i = 0; i < delegates.size() && respondsTo.didDynamicChanged; i++) {
        if ([delegates[i] respondsToSelector:@selector(onDynamicChanged:)]) {
            [delegates[i] onDynamicChanged:_view];
        }
    }
}
