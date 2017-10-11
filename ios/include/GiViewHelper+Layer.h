//! \file GiViewHelper+Layer.h
//! \brief 定义iOS绘图视图辅助类 GiViewHelper 的图层转换器部分
// Copyright (c) 2013-2015, https://github.com/rhcad/vgios, BSD License

#import "GiViewHelper.h"

/*! \category GiViewHelper(Layer)
    \brief GiViewHelper 的图层转换器部分
 */
@interface GiViewHelper(Layer)

//! 将静态图形转换到三级层，第二级为每个图形的层，其下有CAShapeLayer，返回顶级层
- (CALayer *)exportLayerTree:(BOOL)hidden;

//! 将静态图形转换到二级层，第二级为每个路径的CAShapeLayer，不按图形分组
- (CALayer *)exportLayers;

//! 将指定句柄(MgShape)的图形转换到二级层，第二级为多个CAShapeLayer
- (CALayer *)exportLayersForShape:(long)handle;

//! 得到指定句柄(MgShape)的图形的所有路径对象(UIBezierPath)
- (NSArray *)exportPathsForShape:(long)handle;

@end
