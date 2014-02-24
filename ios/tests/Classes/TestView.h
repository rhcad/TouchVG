// TestView.h
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

@protocol TestView <NSObject>
@optional

- (void)save;
- (void)edit;
- (BOOL)exportPNG:(NSString *)filename;

@end
