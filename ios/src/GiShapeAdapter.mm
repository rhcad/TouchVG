//! \file GiShapeAdapter.mm
//! \brief 实现输出 UIBezierPath 的画布适配器类 GiShapeAdapter
// Copyright (c) 2013-2015, https://github.com/rhcad/vgios, BSD License

#include "GiShapeAdapter.h"
#include "GiCanvasAdapter.h"

GiShapeAdapter::GiShapeAdapter(GiShapeAdapterCallback* shapeCallback)
    : _callback(shapeCallback), _container([UIBezierPath bezierPath]), _path(nil)
    , _lineColor([UIColor blackColor]), _fillColor([UIColor clearColor])
{
}

void GiShapeAdapter::endOutput()
{
    fireLastPath();
}

bool GiShapeAdapter::hasLineColor() const
{
    return _lineColor != [UIColor clearColor];
}

bool GiShapeAdapter::hasFillColor() const
{
    return _fillColor != [UIColor clearColor];
}

void GiShapeAdapter::fireLastPath()
{
    if (!CGRectIsEmpty(_container.bounds)) {
        _callback->addPath(_container, _lineColor, _fillColor);
        _container = [UIBezierPath bezierPath];
    }
}

void GiShapeAdapter::checkNeedFire(bool stroke, bool fill)
{
    if (!stroke && hasLineColor()) {
        fireLastPath();
        _lineColor = [UIColor clearColor];
    }
    if (!fill && hasFillColor()) {
        fireLastPath();
        _fillColor = [UIColor clearColor];
    }
}

bool GiShapeAdapter::beginShape(int type, int sid, int, float x, float y, float w, float h)
{
    fireLastPath();
    return _callback->beginShape(type, sid, CGRectMake(x, y, w, h));
}

void GiShapeAdapter::endShape(int type, int sid, float, float)
{
    fireLastPath();
    _callback->endShape(type, sid);
}

void GiShapeAdapter::setPen(int argb, float width, int style, float phase, float)
{
    fireLastPath();
    
    float alpha = GiCanvasAdapter::colorPart(argb, 3);
    _lineColor = (alpha < 1e-2f ? [UIColor clearColor] :
                  [UIColor colorWithRed:GiCanvasAdapter::colorPart(argb, 2)
                                  green:GiCanvasAdapter::colorPart(argb, 1)
                                   blue:GiCanvasAdapter::colorPart(argb, 0)
                                  alpha:alpha]);
    if (width > 0) {
        _container.lineWidth = width;
    }
    if (style >= 0) {
        int linecap = style & kLineCapMask;
        
        style = style & kLineDashMask;
        if (style > 0 && style < 5) {
            CGFloat pattern[6];
            int n = 0;
            for (; GiCanvasAdapter::LINEDASH[style][n] > 0.1f; n++) {
                pattern[n] = GiCanvasAdapter::LINEDASH[style][n] * (width < 1.f ? 1.f : width);
            }
            [_container setLineDash:pattern count:n phase:phase];
        }
        else {
            [_container setLineDash:NULL count:0 phase:0];
        }
        
        if (linecap & kLineCapButt)
            _container.lineCapStyle = kCGLineCapButt;
        else if (linecap & kLineCapRound)
            _container.lineCapStyle = kCGLineCapRound;
        else if (linecap & kLineCapSquare)
            _container.lineCapStyle = kCGLineCapSquare;
        else {
            _container.lineCapStyle = ((style > 0 && style < 5) ? kCGLineCapButt : kCGLineCapRound);
        }
    }
}

void GiShapeAdapter::setBrush(int argb, int style)
{
    if (0 == style) {
        fireLastPath();
        
        float alpha = GiCanvasAdapter::colorPart(argb, 3);
        _fillColor = (alpha < 1e-2f ? [UIColor clearColor] :
                      [UIColor colorWithRed:GiCanvasAdapter::colorPart(argb, 2)
                                      green:GiCanvasAdapter::colorPart(argb, 1)
                                       blue:GiCanvasAdapter::colorPart(argb, 0)
                                      alpha:alpha]);
    }
}

void GiShapeAdapter::saveClip()
{
    NSLog(@"GiShapeAdapter::saveClip() not supported");
}

void GiShapeAdapter::restoreClip()
{
    NSLog(@"GiShapeAdapter::restoreClip() not supported");
}

void GiShapeAdapter::clearRect(float x, float y, float w, float h)
{
    NSLog(@"GiShapeAdapter::clearRect() not supported");
}

void GiShapeAdapter::drawRect(float x, float y, float w, float h, bool stroke, bool fill)
{
    checkNeedFire(stroke, fill);
    [_container appendPath:[UIBezierPath bezierPathWithRect:CGRectMake(x, y, w, h)]];
}

bool GiShapeAdapter::clipRect(float x, float y, float w, float h)
{
    NSLog(@"GiShapeAdapter::clipRect() not supported");
    return false;
}

void GiShapeAdapter::drawLine(float x1, float y1, float x2, float y2)
{
    UIBezierPath *path = [UIBezierPath bezierPath];
    [path moveToPoint:CGPointMake(x1, y1)];
    [path addLineToPoint:CGPointMake(x2, y2)];
    [_container appendPath:path];
}

void GiShapeAdapter::drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
{
    checkNeedFire(stroke, fill);
    [_container appendPath:[UIBezierPath bezierPathWithOvalInRect:CGRectMake(x, y, w, h)]];
}

void GiShapeAdapter::beginPath()
{
    _path = [UIBezierPath bezierPath];
}

void GiShapeAdapter::moveTo(float x, float y)
{
    [_path moveToPoint:CGPointMake(x, y)];
}

void GiShapeAdapter::lineTo(float x, float y)
{
    [_path addLineToPoint:CGPointMake(x, y)];
}

void GiShapeAdapter::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    [_path addCurveToPoint:CGPointMake(x, y) controlPoint1:CGPointMake(c1x, c1y)
             controlPoint2:CGPointMake(c2x, c2y)];
}

void GiShapeAdapter::quadTo(float cpx, float cpy, float x, float y)
{
    [_path addQuadCurveToPoint:CGPointMake(x, y) controlPoint:CGPointMake(cpx, cpy)];
}

void GiShapeAdapter::closePath()
{
    [_path closePath];
}

void GiShapeAdapter::drawPath(bool stroke, bool fill)
{
    checkNeedFire(stroke, fill);
    [_container appendPath:_path];
    _path = nil;
}

bool GiShapeAdapter::clipPath()
{
    NSLog(@"GiShapeAdapter::clipPath() not supported");
    return false;
}

bool GiShapeAdapter::drawHandle(float x, float y, int type, float angle)
{
    NSLog(@"GiShapeAdapter::drawHandle() not supported");
    return false;
}

bool GiShapeAdapter::drawBitmap(const char* name, float xc, float yc,
                                float w, float h, float angle)
{
    NSLog(@"GiShapeAdapter::drawBitmap() not supported");
    return false;
}

float GiShapeAdapter::drawTextAt(const char* text, float x, float y, float h, int align, float angle)
{
    NSLog(@"GiShapeAdapter::drawTextAt() not supported");
    return 0;
}

// GiShapeLayerCallback
//

GiShapeLayerCallback::GiShapeLayerCallback(CALayer *rootLayer, bool hidden)
    : _rootLayer(rootLayer), _shapeLayer(nil), _hidden(hidden)
{
}

void GiShapeLayerCallback::addPath(UIBezierPath *path, UIColor *strokeColor, UIColor *fillColor)
{
    if (!_shapeLayer || !path) {
        return;
    }
    
    CAShapeLayer *pathLayer = [CAShapeLayer layer];
    CGRect frame = _shapeLayer.frame;
    
    pathLayer.frame = _rootLayer ? CGRectMake(0, 0, frame.size.width, frame.size.height) : frame;
    pathLayer.hidden = _hidden;
    [_shapeLayer addSublayer:pathLayer];
    
    if (_rootLayer) {
        [path applyTransform:CGAffineTransformMakeTranslation(-frame.origin.x, -frame.origin.y)];
    }
    pathLayer.path = path.CGPath;
    pathLayer.strokeColor = strokeColor.CGColor;
    pathLayer.fillColor = fillColor.CGColor;
    
    NSInteger count = 10;
    CGFloat pattern[count];
    CGFloat phase;
    
    [path getLineDash:pattern count:&count phase:&phase];
    pathLayer.lineWidth = path.lineWidth;
    pathLayer.lineDashPhase = phase;
    if (count > 0) {
        NSMutableArray * arr = [NSMutableArray array];
        for (int i = 0; i < count; i++) {
            [arr addObject:[NSNumber numberWithFloat:pattern[i]]];
        }
        pathLayer.lineDashPattern = arr;
    }
    switch (path.lineCapStyle) {
        case kCGLineCapButt: pathLayer.lineCap = kCALineCapButt; break;
        case kCGLineCapRound: pathLayer.lineCap = kCALineCapRound; break;
        case kCGLineCapSquare: pathLayer.lineCap = kCALineCapSquare; break;
    }
}

bool GiShapeLayerCallback::beginShape(int, int, CGRect frame)
{
    if (!_shapeLayer) {
        _shapeLayer = [CALayer layer];
        [_rootLayer addSublayer:_shapeLayer];
    }
    _shapeLayer.frame = frame;
    _shapeLayer.position = frame.origin;
    
    return true;
}

void GiShapeLayerCallback::endShape(int type, int)
{
    if (_rootLayer) {
        _shapeLayer = nil;
    }
}
