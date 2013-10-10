//! \file GiViewAdapter.mm
//! \brief 实现iOS绘图视图适配器 GiViewAdapter
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphViewImpl.h"
#import <QuartzCore/CALayer.h>

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

//! Button class for showContextActions().
@interface UIButtonAutoHide : UIButton
@property (nonatomic,assign) id delegate;
@end

@implementation UIButtonAutoHide
@synthesize delegate;

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event {
	BOOL ret = [super pointInside:point withEvent:event];
    CGPoint pt = [self.window convertPoint:point fromView:self];
    
    [delegate performSelector:@selector(ignoreTouch::) withObject:[NSValue valueWithCGPoint:pt]
                   withObject:ret ? self : nil];
    
	return ret;
}

@end

GiViewAdapter::GiViewAdapter(UIView *mainView, GiCoreView *coreView)
: _view(mainView), _dynview(nil), _tmpshot(nil), _drawCount(0)
, _buttons(nil), _buttonImages(nil) {
    _coreView = new GiCoreView(coreView);
    memset(&respondsTo, 0, sizeof(respondsTo));
}

GiViewAdapter::~GiViewAdapter() {
    [_buttons release];
    [_buttonImages release];
    _coreView->destoryView(this);
    delete _coreView;
    [_tmpshot release];
}

UIImage * GiViewAdapter::snapshot(bool autoDraw) {
    if (!autoDraw) {
        _drawCount = 1;
    }
    long oldCount = _drawCount;
    UIImage *image = nil;
    
    UIGraphicsBeginImageContextWithOptions(_view.bounds.size, _view.opaque, 0);
    [_view.layer renderInContext:UIGraphicsGetCurrentContext()];
    
    if (autoDraw || oldCount == _drawCount) {   // 不允许renderInContext触发drawRect时返回nil
        image = UIGraphicsGetImageFromCurrentImageContext();
    }
    UIGraphicsEndImageContext();
    if (!autoDraw) {
        _drawCount = 0;
    }
    
    return image;
}

bool GiViewAdapter::drawAppend(GiCanvas* canvas) {
    if (_drawCount > 0) {   // 还在regenAppend调用中
        _drawCount++;       // 让snapshot函数返回nil
        return true;        // 不需要绘图，反正regenAppend调用snapshot将得到nil
    }
    if (_tmpshot) {
        [_tmpshot drawAtPoint:CGPointZero];         // 先绘制原来的内容
        [_tmpshot release];
        _tmpshot = nil;
        return _coreView->drawAppend(this, canvas); // 然后绘制增量图形
    }
    return false;
}

void GiViewAdapter::clearCachedData() {
    if (_tmpshot) {
        [_tmpshot release];
        _tmpshot = nil;
    }
    if (_buttonImages) {
        [_buttonImages removeAllObjects];
    }
    _coreView->clearCachedData();
}

void GiViewAdapter::regenAll() {
    [_view setNeedsDisplay];
    [_dynview setNeedsDisplay];
}

void GiViewAdapter::regenAppend() {
    [_tmpshot release];
    _tmpshot = nil;                 // renderInContext可能会调用drawRect
    _tmpshot = snapshot(false);     // 获取现有绘图快照
    [_tmpshot retain];
    
    [_view setNeedsDisplay];
    [_dynview setNeedsDisplay];
}

void GiViewAdapter::redraw() {
    if (!_dynview && _view && _view.window) {    // 自动创建动态图形视图
        _dynview = [[IosTempView alloc]initView:_view.frame :this];
        _dynview.autoresizingMask = _view.autoresizingMask;
        [_view.superview addSubview:_dynview];
        [_dynview release];
    }
    if (_dynview) {
        [_dynview setNeedsDisplay];
    }
    else {
        [_view performSelector:@selector(redraw) withObject:nil afterDelay:0.2];
    }
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
    
    state = state < 0 ? sender.state : state;
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
    
    if (n == 0) {
        hideContextActions();
        return true;
    }
    
    if (!_buttons) {
        _buttons = [[NSMutableArray alloc]init];
    }
    if ([_buttons count] > 0 && _coreView->isPressDragging()) {
        return false;
    }
    hideContextActions();
    
    for (int i = 0; i < n; i++) {
        const int action = actions.get(i);
        NSString *caption, *imageName;
        
        if (action > 0 && action < sizeof(CAPTIONS)/sizeof(CAPTIONS[0])) {
            caption = CAPTIONS[action];
            imageName = IMAGENAMES[action];
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
        [btn release];
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
