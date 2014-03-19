// GiPlayProvider.h
// TouchVG
//
// Created by Zhang Yungui on 14-3-7.
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg
//

#import <Foundation/Foundation.h>

struct GiFrame {
    id  view;
    int tag;
    long shapes;
    int tick;
    int lastTick;
    id  extra;
    long backShapes;
};
typedef struct GiFrame GiFrame;

@protocol GiPlayProvider <NSObject>

- (BOOL)initProvider:(GiFrame *)frame;
- (int)provideFrame:(GiFrame)frame;
- (void)onProvideEnded:(GiFrame)frame;

@optional

- (void)beforeSubmitShapes:(GiFrame)frame;
- (void)onBackDocChanged:(GiFrame)frame;

@end
