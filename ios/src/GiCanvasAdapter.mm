//! \file GiCanvasAdapter.mm
//! \brief 实现画布适配器类 GiCanvasAdapter
// Copyright (c) 2012-2015, https://github.com/rhcad/vgios, BSD License

#import "GiImageCache.h"
#include "GiCanvasAdapter.h"
#import "NSString+Drawing.h"

static const float patDash[]      = { 4, 2, 0 };
static const float patDot[]       = { 1, 2, 0 };
static const float patDashDot[]   = { 10, 2, 2, 2, 0 };
static const float dashDotdot[]   = { 20, 2, 2, 2, 2, 2, 0 };
const float* const GiCanvasAdapter::LINEDASH[] = { NULL, patDash, patDot, patDashDot, dashDotdot };

GiCanvasAdapter::GiCanvasAdapter(GiImageCache *cache) : _ctx(NULL), _cache(cache), _gradient0(NULL)
{
}

GiCanvasAdapter::~GiCanvasAdapter()
{
}

CGContextRef GiCanvasAdapter::context()
{
    return _ctx;
}

bool GiCanvasAdapter::beginPaint(CGContextRef context, bool fast)
{
    if (_ctx || !context) {
        return false;
    }
    
    _ctx = context;
    _fill = false;
    _gradient = NULL;
    _fillARGB = 0;
    
    CGContextSetShouldAntialias(_ctx, true);       // 两者都为true才反走样
    CGContextSetAllowsAntialiasing(_ctx, true);
    
    //CGContextSetFlatness(_ctx, fast ? 10 : 3);      // 平滑度为3达到精确和速度的平衡点
    CGContextSetInterpolationQuality(_ctx, kCGInterpolationLow);
    
    CGContextSetLineCap(_ctx, kCGLineCapRound);     // 圆端
    CGContextSetLineJoin(_ctx, kCGLineJoinRound);   // 折线转角圆弧过渡
    
    CGContextSetRGBFillColor(_ctx, 0, 0, 0, 0);     // 默认不填充
    
    return true;
}

void GiCanvasAdapter::endPaint()
{
    _ctx = NULL;
    if (_gradient0) {
        CGGradientRelease(_gradient0);
        _gradient0 = NULL;
    }
}

float GiCanvasAdapter::colorPart(int argb, int byteOrder)
{
    return (float)((argb >> (byteOrder * 8)) & 0xFF) / 255.f;
}

void GiCanvasAdapter::setPen(int argb, float width, int style, float phase, float)
{
    if (argb != 0) {
        CGFloat r = colorPart(argb, 2), g = colorPart(argb, 1);
        CGFloat b = colorPart(argb, 0), a = colorPart(argb, 3);
        CGContextSetRGBStrokeColor(_ctx, r, g, b, a);
    }
    if (width > 0) {
        CGContextSetLineWidth(_ctx, width);
    }
    if (style >= 0) {
        int linecap = style & kLineCapMask;
        
        style = style & kLineDashMask;
        if (style > 0 && style < 5) {
            CGFloat pattern[6];
            int n = 0;
            for (; LINEDASH[style][n] > 0.1f; n++) {
                pattern[n] = LINEDASH[style][n] * (width < 1.f ? 1.f : width);
            }
            CGContextSetLineDash(_ctx, phase, pattern, n);
        }
        else if (0 == style) {
            CGContextSetLineDash(_ctx, 0, NULL, 0);
        }
        if (linecap & kLineCapButt)
            CGContextSetLineCap(_ctx, kCGLineCapButt);
        else if (linecap & kLineCapRound)
            CGContextSetLineCap(_ctx, kCGLineCapRound);
        else if (linecap & kLineCapSquare)
            CGContextSetLineCap(_ctx, kCGLineCapSquare);
        else {
            CGContextSetLineCap(_ctx, (style > 0 && style < 5) ? kCGLineCapButt : kCGLineCapRound);
        }
    }
}

void GiCanvasAdapter::setBrush(int argb, int style)
{
    if (0 == style) {
        CGFloat alpha = colorPart(argb, 3);
        CGFloat r = colorPart(argb, 2), g = colorPart(argb, 1), b = colorPart(argb, 0);
        _fill = alpha > 1e-2f;
        CGContextSetRGBFillColor(_ctx, r, g, b, alpha);
        _fillARGB = argb;
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
    if (!stroke && !fill) {
        return; // can used to clip later
    }
    if (_gradient && !CGContextIsPathEmpty(_ctx)) {
        CGContextSaveGState(_ctx);
        CGRect rect = CGContextGetPathBoundingBox(_ctx);
        CGContextReplacePathWithStrokedPath(_ctx);
        CGContextClip(_ctx);
        
        CGContextDrawLinearGradient(_ctx, _gradient, CGPointMake(CGRectGetMinX(rect), CGRectGetMinY(rect)),
                                    CGPointMake(CGRectGetMaxX(rect), CGRectGetMaxY(rect)), 0);
        CGContextRestoreGState(_ctx);
    }
    else {
        fill = fill && _fill;
        CGContextDrawPath(_ctx, (stroke && fill) ? kCGPathEOFillStroke
                          : (fill ? kCGPathEOFill : kCGPathStroke));  // will clear the path
    }
}

bool GiCanvasAdapter::clipPath()
{
    CGContextClip(_ctx);
    CGRect rect = CGContextGetClipBoundingBox(_ctx);
    return !CGRectIsEmpty(rect);
}

bool GiCanvasAdapter::drawHandle(float x, float y, int type, float angle)
{
    if (type >= 0) {    // GiHandleTypes
        static NSString *names[] = { @"vgdot1.png", @"vgdot2.png", @"vgdot3.png",
            @"vg_lock.png", @"vg_unlock.png", @"vg_back.png", @"vg_endedit.png",
            @"vgnode.png", @"vgcen.png", @"vgmid.png", @"vgquad.png", @"vgtangent.png",
            @"vgcross.png", @"vgparallel.png", @"vgnear.png", @"vgpivot.png", @"vg_overturn.png"
        };
        NSString *name;
        
        if (type < sizeof(names)/sizeof(names[0])) {
            name = [@"TouchVG.bundle" stringByAppendingPathComponent:names[type]];
        } else {
            name = [NSString stringWithFormat:@"vgdot%d.png", type];
        }
        
        UIImage *image = _cache ? [_cache loadImage:name] : [UIImage imageNamed:name];
        
        if (image) {
            CGImageRef img = [image CGImage];
            float w = CGImageGetWidth(img) / image.scale;
            float h = CGImageGetHeight(img) / image.scale;
            
            CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, x - w * 0.5f, y + h * 0.5f);
            CGContextConcatCTM(_ctx, af);
            CGContextDrawImage(_ctx, CGRectMake(0, 0, w, h), img);
            CGContextConcatCTM(_ctx, CGAffineTransformInvert(af));
            
            return true;
            // 如果使用下面一行显示，则图像是上下颠倒的:
            // CGContextDrawImage(_ctx, CGRectMake(x - w * 0.5f, y - h * 0.5f, w, h), img);
        }
    }
    
    return false;
}

bool GiCanvasAdapter::drawBitmap(const char* name, float xc, float yc,
                                 float w, float h, float angle)
{
    UIImage *image = (name && _cache ? [_cache loadImage:@(name)]
                      : [UIImage imageNamed:@"app57.png"]);
    if (image) {
        CGImageRef img = [image CGImage];
        CGAffineTransform af = CGAffineTransformMake(1, 0, 0, -1, xc, yc);
        af = CGAffineTransformRotate(af, angle);
        CGContextConcatCTM(_ctx, af);
        CGContextDrawImage(_ctx, CGRectMake(-w/2, -h/2, w, h), img);
        CGContextConcatCTM(_ctx, CGAffineTransformInvert(af));
    }
    
    return !!image;
}

NSString *GiLocalizedString(NSString *name)
{
    if ([name length] == 0) {
        return name;
    }
    
    NSString *str = name;
    NSString *names[] = { @"TouchVG", @"vg1", @"vg2", @"vg3", @"vg4" };
    NSString *language = [[[NSUserDefaults standardUserDefaults]
                           objectForKey:@"AppleLanguages"] objectAtIndex:0];
    
    for (int i = 0; i < 5 && [str isEqualToString:name]; i++) {
        NSString *path = [[NSBundle mainBundle] pathForResource:names[i] ofType:@"bundle"];
        NSBundle *bundle = [NSBundle bundleWithPath:path];
        NSBundle *languageBundle = [NSBundle bundleWithPath:[bundle pathForResource:language ofType:@"lproj"]];
        if (!languageBundle) {  // for iOS 9
            NSRange range = [language rangeOfString:@"-" options:NSBackwardsSearch];
            if (range.length > 0) {
                language = [language substringToIndex:range.location];
                languageBundle = [NSBundle bundleWithPath:[bundle pathForResource:language ofType:@"lproj"]];
            }
        }
        str = NSLocalizedStringFromTableInBundle(name, nil, languageBundle, nil);
        str = str ? str : name;
    }
    
    return [str isEqualToString:name] ? NSLocalizedString(name, nil) : str;
}

float GiCanvasAdapter::drawTextAt(const char* text, float x, float y, float h, int align, float angle)
{
    UIGraphicsPushContext(_ctx);            // 设置为当前上下文，供UIKit显示使用
    
    NSString *str = (*text == '@') ? GiLocalizedString(@(text+1)) : @(text);
    
    // 实际字体大小(磅) / 目标字体行高 h = 临时字体大小(磅) h / 临时字体行高 actsize.height
    UIFont *font = [UIFont systemFontOfSize:h];                             // 以磅单位作为字体大小得到临时字体
    CGSize actsize = boundingRectWithSize6(str, CGSizeMake(1e4f, h),        // 限制单行高度
                                           NSStringDrawingTruncatesLastVisibleLine,
                                           @{NSFontAttributeName:font},     // 使用临时字体计算文字显示宽高
                                           nil).size;
    font = [UIFont systemFontOfSize: h * h / actsize.height];
    
    UIColor *color = [UIColor colorWithRed:colorPart(_fillARGB, 2) green:colorPart(_fillARGB, 1)
                                      blue:colorPart(_fillARGB, 0) alpha:colorPart(_fillARGB, 3)];
    NSDictionary *attrs = @{NSFontAttributeName:font, NSForegroundColorAttributeName:color};
    actsize = sizeWithAttributes6(str, attrs);                              // 文字实际显示的宽高
    
    if (_fill) {
        CGAffineTransform af = CGAffineTransformIdentity;
        
        if (fabsf(angle) > 1e-3f) {
            af = CGAffineTransformRotate(CGAffineTransformMakeTranslation(x, y), -angle);
            CGContextConcatCTM(_ctx, af);
            x = y = 0;
        }
        y -= (align & kAlignBottom) ? h : (align & kAlignVCenter) ? h / 2 : 0.f;
        x -= (align & kAlignRight) ? actsize.width : ((align & kAlignCenter) ? actsize.width / 2 : 0.f);
        drawAtPoint6(str, CGPointMake(x, y), attrs);                        // 显示文字
        
        if (fabsf(angle) > 1e-3f) {
            CGContextConcatCTM(_ctx, CGAffineTransformInvert(af));
        }
    }
    UIGraphicsPopContext();
    
    return actsize.width;
}
