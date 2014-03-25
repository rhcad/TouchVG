//  GiMagnifierView.h
//

#import <UIKit/UIKit.h>

@interface GiMagnifierView : UIView {
    CGFloat _topMargin;
}

@property (nonatomic, WEAK) UIView *viewToMagnify;
@property (nonatomic) CGPoint touchPoint;
@property (nonatomic) BOOL followFinger;
@property (nonatomic) CGFloat scale;

- (void)show;
- (void)hide;

@end
