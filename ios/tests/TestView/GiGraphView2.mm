//! \file GiGraphView2.mm
//! \brief 实现iOS绘图视图类 GiGraphView2
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphView1.h"
#import "GiViewHelper.h"
#include "RandomShape.h"
#include "gicoreview.h"

static char _lastVgFile[256] = { 0 };

@implementation GiGraphView2

- (BOOL)exportPNG:(NSString *)filename
{
    NSString *vgfile = [[filename stringByDeletingPathExtension]
                        stringByAppendingPathExtension:@"vg"];
    [[GiViewHelper sharedInstance:self] saveToFile:vgfile];
    [[GiViewHelper sharedInstance] exportSVG:vgfile];
    strncpy(_lastVgFile, [vgfile UTF8String], sizeof(_lastVgFile));
    return [super exportPNG:filename];
}

+ (NSString *)lastFileName
{
    if (_lastVgFile[0] == 0) {
        NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                              NSUserDomainMask, YES) objectAtIndex:0];
        return [path stringByAppendingPathComponent:@"page0.png"];
    }
    
    return [NSString stringWithUTF8String:_lastVgFile];
}

- (id)initWithFrame:(CGRect)frame withType:(int)type {
    self = [super initWithFrame:frame];
    if (self) {
        _testType = type;
    }
    return self;
}

- (void)onFirstRegen:(id)view {
    GiViewHelper *helper = [GiViewHelper sharedInstance];
    NSString *path = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,
                                                          NSUserDomainMask, YES) objectAtIndex:0];
    if (_testType == kPlayShapes) {
        _pauseBtn = [[UIButton alloc]initWithFrame:CGRectMake(0, 50, 100, 32)];
        _pauseBtn.showsTouchWhenHighlighted = YES;
        _pauseBtn.backgroundColor = [UIColor colorWithRed:0.5 green:0.5 blue:0.5 alpha:0.8];
        [_pauseBtn setTitleColor:[UIColor blackColor] forState: UIControlStateNormal];
        [_pauseBtn addTarget:self action:@selector(onPause) forControlEvents:UIControlEventTouchUpInside];
        [self addSubview:_pauseBtn];
        [_pauseBtn RELEASE];
        
        [helper startPlay:[path stringByAppendingPathComponent:@"record"]];
    }
    else if (_testType & kRecord) {
        [helper startRecord:[path stringByAppendingPathComponent:@"record"]];
    }
    [helper startUndoRecord:[path stringByAppendingPathComponent:@"undo"]];
    [self addUndoRedoButton];
}

- (void)layoutSubviews {
    [self layoutButtons];
    [super layoutSubviews];
}

- (void)layoutButtons {
    _undoBtn.frame = CGRectMake(0, self.bounds.size.height - 32, 50, 32);
    _redoBtn.frame = CGRectMake(self.bounds.size.width - 50, self.bounds.size.height - 32, 50, 32);
}

- (void)addUndoRedoButton {
    _undoBtn = [[UIButton alloc]initWithFrame:CGRectNull];
    _undoBtn.showsTouchWhenHighlighted = YES;
    [_undoBtn setTitle:@"Undo" forState: UIControlStateNormal];
    _undoBtn.backgroundColor = [UIColor clearColor];
    [_undoBtn setTitleColor:[UIColor blackColor] forState: UIControlStateHighlighted];
    [_undoBtn addTarget:self action:@selector(undo) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:_undoBtn];
    [_undoBtn RELEASE];
    
    _redoBtn = [[UIButton alloc]initWithFrame:CGRectNull];
    _redoBtn.showsTouchWhenHighlighted = YES;
    [_redoBtn setTitle:@"Redo" forState: UIControlStateNormal];
    _redoBtn.backgroundColor = [UIColor clearColor];
    [_redoBtn setTitleColor:[UIColor blackColor] forState: UIControlStateHighlighted];
    [_redoBtn addTarget:self action:@selector(redo) forControlEvents:UIControlEventTouchUpInside];
    [self addSubview:_redoBtn];
    [_redoBtn RELEASE];
    
    [self layoutButtons];
}

- (void)onPause {
    if ([[GiViewHelper sharedInstance] isPaused]) {
        [[GiViewHelper sharedInstance] playResume];
    } else {
        [[GiViewHelper sharedInstance] playPause];
    }
}

- (void)onContentChanged:(id)view {
    GiViewHelper *helper = [GiViewHelper sharedInstance];
    _undoBtn.backgroundColor = [helper canUndo] ? [UIColor grayColor] : [UIColor clearColor];
    _redoBtn.backgroundColor = [helper canRedo] ? [UIColor grayColor] : [UIColor clearColor];
}

@end
