//! \file GiPaintViewDelegate.h
//! \brief 定义绘图消息的观察者协议 GiPaintViewDelegate
// Copyright (c) 2014, https://github.com/rhcad/touchvg

#import <UIKit/UIKit.h>

//! 绘图消息的观察者协议
/*! 也可以在 GiPaintView 派生类直接实现这些通知函数。
    \ingroup GROUP_IOS
    \see GiPaintView, GiViewHelper
 */
@protocol GiPaintViewDelegate <NSObject>
@optional

- (void)onCommandChanged:(id)view;      //!< 当前命令改变的通知
- (void)onSelectionChanged:(id)view;    //!< 图形选择集改变的通知
- (void)onContentChanged:(id)view;      //!< 图形数据改变的通知
- (void)onDynamicChanged:(id)view;      //!< 图形动态改变的通知
- (void)onResizeFrame:(id)view;         //!< 视图大小改变(setFrame:)的通知
- (void)onZoomChanged:(id)view;         //!< 视图放缩的通知
- (void)onFirstRegen:(id)view;          //!< 第一次后台渲染结束的通知
- (void)onDynDrawEnded:(id)view;        //!< 动态绘图完成的通知
- (void)onShapesRecorded:(NSDictionary *)info;  //!< 录制的通知
- (void)onShapeWillDelete:(id)num;      //!< 图形将删除的通知, [NSNumber intValue]
- (void)onShapeDeleted:(id)num;         //!< 图形已删除的通知, [NSNumber intValue]
- (BOOL)onShapeDblClick:(NSDictionary *)info;   //!< 图形双击编辑的通知
- (BOOL)onShapeClicked:(NSDictionary *)info;    //!< 图形点击的通知

- (BOOL)onGestureShouldBegin:(UIGestureRecognizer *)g;    //!< 手势生效前的判断
- (void)onGestureBegan:(UIGestureRecognizer *)g;    //!< 手势开始的通知
- (void)onGestureEnded:(UIGestureRecognizer *)g;    //!< 手势结束的通知

@end
