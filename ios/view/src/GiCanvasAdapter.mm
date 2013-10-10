//! \file GiCanvasAdapter.mm
//! \brief 实现画布适配器类 GiCanvasAdapter
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#include "GiCanvasAdapter.h"
#include <sys/sysctl.h>

int GiCanvasAdapter::getScreenDpi()
{
    size_t size = 15;
    char machine[15 + 1] = "";
    
    sysctlbyname("hw.machine", machine, &size, NULL, 0);
    
    if (strcmp(machine, "i386") == 0) {     // 模拟器
        return 72;
    }
    
    bool iPadMini = (strcmp(machine, "iPad2,5") == 0 ||
                     strcmp(machine, "iPad2,6") == 0 ||
                     strcmp(machine, "iPad2,7") == 0);
    BOOL iPad = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad);
    
    return (iPad && !iPadMini) ? 132 : 163;
}

GiCanvasAdapter::GiCanvasAdapter() : _ctx(NULL)
{
}

GiCanvasAdapter::~GiCanvasAdapter()
{
}

CGContextRef GiCanvasAdapter::context()
{
    return _ctx;
}

bool GiCanvasAdapter::beginPaint(CGContextRef context)
{
    if (_ctx || !context) {
        return false;
    }
    
    _ctx = context;
    _fill = false;
    
    CGContextSetShouldAntialias(_ctx, true);        // 两者都为true才反走样
    CGContextSetAllowsAntialiasing(_ctx, true);
    CGContextSetFlatness(_ctx, 3);                  // 平滑度为3达到精确和速度的平衡点
    
    CGContextSetLineCap(_ctx, kCGLineCapRound);     // 圆端
    CGContextSetLineJoin(_ctx, kCGLineJoinRound);   // 折线转角圆弧过渡
    
    CGContextSetRGBFillColor(_ctx, 0, 0, 0, 0);     // 默认不填充
    
    return true;
}

void GiCanvasAdapter::endPaint()
{
    _ctx = NULL;
}

static inline float colorPart(int argb, int bit)
{
    return (float)((argb >> bit) & 0xFF) / 255.f;
}

void GiCanvasAdapter::setPen(int argb, float width, int style, float phase)
{
    const CGFloat patDash[]      = { 5, 5, 0 };
    const CGFloat patDot[]       = { 1, 2, 0 };
    const CGFloat patDashDot[]   = { 10, 2, 2, 2, 0 };
    const CGFloat dashDotdot[]   = { 20, 2, 2, 2, 2, 2, 0 };
    const CGFloat* const lpats[] = { NULL, patDash, patDot, patDashDot, dashDotdot };
    
    if (argb != 0) {
        CGContextSetRGBStrokeColor(_ctx, colorPart(argb, 16), colorPart(argb, 8),
                                   colorPart(argb, 0), colorPart(argb, 24));
    }
    if (width > 0) {
        CGContextSetLineWidth(_ctx, width);
    }
    
    if (style > 0 && style < sizeof(lpats)/sizeof(lpats[0])) {
        CGFloat pattern[6];
        int n = 0;
        for (; lpats[style][n] > 0.1f; n++) {
            pattern[n] = lpats[style][n] * (width < 1.f ? 1.f : width);
        }
        CGContextSetLineDash(_ctx, phase, pattern, n);
        CGContextSetLineCap(_ctx, kCGLineCapButt);
    }
    else if (0 == style) {
        CGContextSetLineDash(_ctx, 0, NULL, 0);
        CGContextSetLineCap(_ctx, kCGLineCapRound);
    }
}

void GiCanvasAdapter::setBrush(int argb, int style)
{
    if (0 == style) {
        float alpha = colorPart(argb, 24);
        _fill = alpha > 1e-2f;
        CGContextSetRGBFillColor(_ctx, colorPart(argb, 16), colorPart(argb, 8),
                                 colorPart(argb, 0), alpha);
    }
}

void GiCanvasAdapter::saveClip()
{
    CGContextSaveGState(_ctx);
}

void GiCanvasAdapter::restoreClip()
{
    CGContextRestoreGState(_ctx);
}

void GiCanvasAdapter::clearRect(float x, float y, float w, float h)
{
    CGContextClearRect(_ctx, CGRectMake(x, y, w, h));
}

void GiCanvasAdapter::drawRect(float x, float y, float w, float h, bool stroke, bool fill)
{
    if (fill && _fill) {
        CGContextFillRect(_ctx, CGRectMake(x, y, w, h));
    }
    if (stroke) {
        CGContextStrokeRect(_ctx, CGRectMake(x, y, w, h));
    }
}

bool GiCanvasAdapter::clipRect(float x, float y, float w, float h)
{
    CGContextClipToRect(_ctx, CGRectMake(x, y, w, h));
    CGRect rect = CGContextGetClipBoundingBox(_ctx);
    return !CGRectIsEmpty(rect);
}

void GiCanvasAdapter::drawLine(float x1, float y1, float x2, float y2)
{
    CGContextBeginPath(_ctx);
    CGContextMoveToPoint(_ctx, x1, y1);
    CGContextAddLineToPoint(_ctx, x2, y2);
    CGContextStrokePath(_ctx);
}

void GiCanvasAdapter::drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
{
    if (fill && _fill) {
        CGContextFillEllipseInRect(_ctx, CGRectMake(x, y, w, h));
    }
    if (stroke) {
        CGContextStrokeEllipseInRect(_ctx, CGRectMake(x, y, w, h));
    }
}

void GiCanvasAdapter::beginPath()
{
    CGContextBeginPath(_ctx);
}

void GiCanvasAdapter::moveTo(float x, float y)
{
    CGContextMoveToPoint(_ctx, x, y);
}

void GiCanvasAdapter::lineTo(float x, float y)
{
    CGContextAddLineToPoint(_ctx, x, y);
}

void GiCanvasAdapter::bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
{
    CGContextAddCurveToPoint(_ctx, c1x, c1y, c2x, c2y, x, y);
}

void GiCanvasAdapter::quadTo(float cpx, float cpy, float x, float y)
{
    CGContextAddQuadCurveToPoint(_ctx, cpx, cpy, x, y);
}

void GiCanvasAdapter::closePath()
{
    CGContextClosePath(_ctx);
}

void GiCanvasAdapter::drawPath(bool stroke, bool fill)
{
    fill = fill && _fill;
    CGContextDrawPath(_ctx, (stroke && fill) ? kCGPathEOFillStroke
                      : (fill ? kCGPathEOFill : kCGPathStroke));  // will clear the path
}

bool GiCanvasAdapter::clipPath()
{
    CGContextClip(_ctx);
    CGRect rect = CGContextGetClipBoundingBox(_ctx);
    return !CGRectIsEmpty(rect);
}

void GiCanvasAdapter::drawHandle(float x, float y, int type)
{
    if (type >= 0 && type < 6) {
        NSString *names[] = { @"vgdot1.png", @"vgdot2.png", @"vgdot3.png", 
            @"vg_lock.png", @"vg_unlock.png", @"vg_back.png", @"vg_endedit.png" };
        NSString *name = [@"TouchVG.bundle/" stringByAppendingString:names[type]];
        UIImage *image = [UIImage imageNamed:name];
        
        if (image) {
            CGImageRef img = [image CGImage];
            float w = CGImageGetWidth(img) / image.scale;
            float h = CGImageGetHeight(img) / image.scale;
            
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, x - w * 0.5f, y + h * 0.5f);
            CGContextConcatCTM(_ctx, af);
            CGContextDrawImage(_ctx, CGRectMake(0, 0, w, h), img);
            CGContextConcatCTM(_ctx, CGAffineTransformInvert(af));
            
            // 如果使用下面一行显示，则图像是上下颠倒的:
            // CGContextDrawImage(_ctx, CGRectMake(x - w * 0.5f, y - h * 0.5f, w, h), img);
        }
    }
}

void GiCanvasAdapter::drawBitmap(const char* name, float xc, float yc, 
                                  float w, float h, float angle)
{
    UIImage *image = [UIImage imageNamed:@"app57.png"];
    if (image) {
        CGImageRef img = [image CGImage];
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, xc, yc);
        af = CGAffineTransformRotate(af, angle);
        CGContextConcatCTM(_ctx, af);
        CGContextDrawImage(_ctx, CGRectMake(-w/2, -h/2, w, h), img);
        CGContextConcatCTM(_ctx, CGAffineTransformInvert(af));
    }
}

float GiCanvasAdapter::drawTextAt(const char* text, float x, float y, float h, int align)
{
    UIGraphicsPushContext(_ctx);        // 设置为当前上下文，供UIKit显示使用
    
    NSString *str = [[NSString alloc] initWithUTF8String:text];
    
    // 实际字体大小 = 目标高度 h * 临时字体大小 h / 临时字体行高 actsize.height
    UIFont *font = [UIFont systemFontOfSize:h]; // 以像素点高度作为字体大小得到临时字体
    CGSize actsize = [str sizeWithFont:font     // 使用临时字体计算文字显示宽高
                     constrainedToSize:CGSizeMake(1e4f, h)];    // 限制单行高度
    font = [UIFont systemFontOfSize: h * h / actsize.height];
    actsize = [str sizeWithFont:font];          // 文字实际显示的宽高
    
    x -= (align == 2) ? actsize.width : ((align == 1) ? actsize.width / 2 : 0);
    [str drawAtPoint:CGPointMake(x, y) withFont:font];  // 显示文字
    [str release];
    
    UIGraphicsPopContext();
    
    return actsize.width;
}
