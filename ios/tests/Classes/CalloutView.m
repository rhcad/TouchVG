#import "CalloutView.h"

static CalloutView* _view = nil;

@implementation CalloutView

- (id)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        if (_view == nil) {
            _view = self;
        }
    }
    return self;
}

- (void)dealloc
{
    if (_view == self) {
        _view = nil;
    }
    [super DEALLOC];
}

+ (id)currentView
{
    return _view;
}

- (BOOL)pointInside:(CGPoint)point withEvent:(UIEvent *)event
{
    BOOL ret = [super pointInside:point withEvent:event];
    if (!ret) {
        [self removeFromSuperview];
    }
    return ret;
}

@end
