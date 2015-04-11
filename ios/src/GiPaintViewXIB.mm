//! \file GiPaintViewXIB.mm
//! \brief 实现可在XIB中使用的绘图视图类 GiPaintViewXIB
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiViewHelper.h"
#import "GiPaintViewXIB.h"
#include "ARCMacro.h"
#include "mgview.h"

//! GiPaintViewXIB的内部数据
@interface GiPaintViewXIB () {
    GiViewHelper    *hlp;
}

@end

@implementation GiPaintViewXIB

@synthesize helper = hlp;
@synthesize command, lineWidth, strokeWidth, lineStyle;
@synthesize lineColor, lineAlpha, fillColor, fillAlpha;
@synthesize options, content, path, zoomEnabled;
@synthesize extent, zoomExtent, viewScale, viewCenter, modelScale;

- (id)initWithCoder:(NSCoder *)aDecoder {
    self = [super initWithCoder:aDecoder];
    if (self) {
        hlp = [[GiViewHelper alloc]initWithView:self];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        hlp = [[GiViewHelper alloc]initWithView:self];
    }
    return self;
}

- (id)initWithFrame:(CGRect)frame flags:(int)f {
    self = [super initWithFrame:frame flags:f];
    if (self) {
        hlp = [[GiViewHelper alloc]initWithView:self];
    }
    return self;
}

- (void)dealloc {
    [hlp RELEASEOBJ];
    [super DEALLOC];
}

- (NSString *)command {
    return hlp.command;
}

- (void)setCommand:(NSString *)value {
    hlp.command = value;
}

- (CGFloat)lineWidth {
    return hlp.lineWidth;
}

- (void)setLineWidth:(CGFloat)value {
    hlp.lineWidth = value;
}

- (CGFloat)strokeWidth {
    return hlp.strokeWidth;
}

- (void)setStrokeWidth:(CGFloat)value {
    hlp.strokeWidth = value;
}

- (NSString *)lineStyle {
    NSString *arr[] = { @"solid", @"dash", @"dot", @"dashdot", @"dashdotdot", @"null" };
    int n = hlp.lineStyle;
    return arr[n >= 0 && n < 6 ? n : 0];
}

- (void)setLineStyle:(NSString *)value {
    NSString *arr[] = { @"solid", @"dash", @"dot", @"dashdot", @"dashdotdot", @"null", nil };
    for (int i = 0; arr[i]; i++) {
        if ([arr[i] isEqualToString:value]) {
            hlp.lineStyle = (GILineStyle)i;
            break;
        }
    }
}

- (UIColor  *)lineColor {
    return hlp.lineColor;
}

- (void)setLineColor:(UIColor *)value {
    hlp.lineColor = value;
}

- (CGFloat)lineAlpha {
    return hlp.lineAlpha;
}

- (void)setLineAlpha:(CGFloat)value {
    hlp.lineAlpha = value;
}

- (UIColor  *)fillColor {
    return hlp.fillColor;
}

- (void)setFillColor:(UIColor *)value {
    hlp.fillColor = value;
}

- (CGFloat)fillAlpha {
    return hlp.fillAlpha;
}

- (void)setFillAlpha:(CGFloat)value {
    hlp.fillAlpha = value;
}

- (NSDictionary *)options {
    return hlp.options;
}

- (void)setOptions:(NSDictionary *)value {
    hlp.options = value;
}

- (NSString *)content {
    return hlp.content;
}

- (void)setContent:(NSString *)value {
    hlp.content = value;
}

- (NSString *)path {
    return [hlp exportSVGPath:hlp.selectedShapeID];
}

- (void)setPath:(NSString *)value {
    [hlp importSVGPath:hlp.selectedShapeID d:value];
}

- (CGRect)extent {
    MgView *view = hlp.cmdView;
    Box2d rect(view->shapes()->getExtent());
    return CGRectMake(rect.xmin, rect.ymin, rect.width(), rect.height());
}

- (CGRect)zoomExtent {
    MgView *view = hlp.cmdView;
    Box2d rect(view->xform()->getWndRectM());
    return CGRectMake(rect.xmin, rect.ymin, rect.width(), rect.height());
}

- (void)setZoomExtent:(CGRect)value {
    [hlp zoomToModel:value];
}

- (CGFloat)viewScale {
    MgView *view = hlp.cmdView;
    return view->xform()->getViewScale();
}

- (void)setViewScale:(CGFloat)value {
    MgView *view = hlp.cmdView;
    
    if (view->xform()->zoomScale(value)) {
        view->regenAll(false);
    }
}

- (CGPoint)viewCenter {
    MgView *view = hlp.cmdView;
    Point2d pt(view->xform()->getCenterW());
    return CGPointMake(pt.x, pt.y);
}

- (void)setViewCenter:(CGPoint)value {
    MgView *view = hlp.cmdView;
    
    if (view->xform()->zoomTo(Point2d(value.x, value.y))) {
        view->regenAll(false);
    }
}

- (CGSize)modelScale {
    MgView *view = hlp.cmdView;
    Matrix2d mat(view->xform()->modelToWorld());
    return CGSizeMake(mat.m11, mat.m22);
}

- (void)setModelScale:(CGSize)value {
    MgView *view = hlp.cmdView;
    Matrix2d mat(Matrix2d::scaling(value.width, value.height));
    
    if (view->xform()->setModelTransform(mat)) {
        view->regenAll(false);
    }
}

- (BOOL)zoomEnabled {
    return hlp.zoomEnabled;
}

- (void)setZoomEnabled:(BOOL)enabled {
    hlp.zoomEnabled = enabled;
}

@end
