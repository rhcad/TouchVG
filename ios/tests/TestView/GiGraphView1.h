//! \file GiGraphView1.h
//! \brief 定义iOS绘图视图类 GiGraphView1
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiPaintView.h"

class ViewAdapter1;
class GiCoreView;

enum kTestFlags {
    kSplinesCmd = 1,
    kSelectCmd  = 2,
    kSelectLoad = 3,
    kLineCmd    = 4,
    kLinesCmd   = 5,
    kHitTestCmd = 6,
    kAddImages  = 7,
    kLoadImages = 8,
    kSVGImages  = 9,
    kSVGPages   = 10,
    kCmdMask    = 15,
    kSwitchCmd  = 16,
    kRandShapes = 32,
    kRecord     = 64,
};

//! iOS测试绘图视图类
@interface GiGraphView1 : UIView {
    ViewAdapter1    *_adapter;
}

- (GiCoreView *)coreView;               //!< 得到跨平台内核视图
- (UIImage *)snapshot;                  //!< 得到静态图形的快照，自动释放
- (BOOL)exportPNG:(NSString *)filename; //!< 保存静态图形的快照到PNG文件

@end

//! iOS测试绘图视图类
@interface GiGraphView2 : GiPaintView {
    int         _testType;
    int         _frameIndex;
    UIButton    *_undoBtn;
    UIButton    *_redoBtn;
    UIButton    *_pauseBtn;
    UIGestureRecognizer *_recognizer;
}

- (id)initWithFrame:(CGRect)frame withType:(int)type;
+ (NSString *)lastFileName;

@end
