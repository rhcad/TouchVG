//! \file GiGraphView1.h
//! \brief 定义iOS绘图视图类 GiGraphView1
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphView.h"

class ViewAdapter1;
class GiCoreView;

//! iOS绘图视图类
/*! \ingroup GROUP_IOS
 */
@interface GiGraphView1 : UIView {
    ViewAdapter1    *_viewAdapter;
}

- (GiCoreView *)coreView;               //!< 得到跨平台内核视图
- (UIImage *)snapshot;                  //!< 得到静态图形的快照，自动释放
- (BOOL)savePng:(NSString *)filename;   //!< 保存静态图形的快照到PNG文件

@end

@interface GiGraphView2 : GiGraphView

- (BOOL)savePng:(NSString *)filename;
+ (NSString *)lastFileName;

@end
