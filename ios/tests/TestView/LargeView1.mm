// LargeView1.mm
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "LargeView1.h"
#import "GiGraphView1.h"

@implementation LargeView1
@synthesize subview2 = _subview2;

- (id)initWithFrame:(CGRect)frame withType:(int)type
{
    self = [super initWithFrame:frame];
    if (self) {
        float w = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) ? 1024 : 2048;
        
        UIView *view = [[UIView alloc]initWithFrame:CGRectMake(0, 0, w, w)];
        [self addSubview:view];
        
        UIView *subview;
        
        if (type == 0) {
            _subview1 = [[GiGraphView1 alloc]initWithFrame:view.bounds];
            subview = _subview1;
        }
        else {
            _subview2 = [[GiGraphView2 alloc]initWithFrame:view.bounds withType:type];
            subview = _subview2;
#ifdef __IPHONE_5_0
            if (_subview2.panRecognizer) {
                [self.panGestureRecognizer requireGestureRecognizerToFail:_subview2.panRecognizer];
            }
            if (_subview2.pinchRecognizer) {
                [self.pinchGestureRecognizer requireGestureRecognizerToFail:_subview2.pinchRecognizer];
            }
#endif
        }
        [view addSubview:subview];
        
        self.delegate = self;
        self.contentSize = subview.frame.size;
        self.minimumZoomScale = 0.25f;
        self.maximumZoomScale = 5.f;
    }
    
    return self;
}

- (UIView *)viewForZoomingInScrollView:(UIScrollView *)scrollView
{
    return _subview1 ? _subview1.superview : _subview2.superview;
}

- (BOOL)exportPNG:(NSString *)filename
{
    if (_subview1) {
        return [_subview1 exportPNG:filename];
    }
    else {
        return [_subview2 exportPNG:filename];
    }
}

- (void)removeFromSuperview {
    [[self viewForZoomingInScrollView:nil] removeFromSuperview];
    [super removeFromSuperview];
}

@end
