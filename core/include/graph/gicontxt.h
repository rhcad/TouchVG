//! \file gicontxt.h
//! \brief 定义绘图环境类 GiContext
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_DRAWCONTEXT_H_
#define TOUCHVG_DRAWCONTEXT_H_

#include "gicolor.h"
#include <math.h>

//! 绘图参数上下文类
/*! 用于在图形系统的绘图函数中传入绘图参数
    \ingroup GRAPH_INTERFACE
    \see GiColor
*/
class GiContext
{
public:
    enum {
        kSolidLine = 0,     //!< ----------
        kDashLine,          //!< － － － －
        kDotLine,           //!< ..........
        kDashDot,           //!< _._._._._
        kDashDotdot,        //!< _.._.._.._
        kNullLine,          //!< Not draw.
        
        kCopyNone  = 0,     //!< 不设置属性
        kLineRGB   = 0x01,  //!< 设置线色的RGB分量
        kLineAlpha = 0x02,  //!< 设置线色的透明度分量
        kLineARGB  = 0x03,  //!< 设置线色的所有分量
        kLineWidth = 0x04,  //!< 设置线宽
        kLineStyle = 0x08,  //!< 设置线型
        kFillRGB   = 0x10,  //!< 设置填充色的RGB分量
        kFillAlpha = 0x20,  //!< 设置填充色的透明度分量
        kFillARGB  = 0x30,  //!< 设置填充色的所有分量
        kLineArrayHead = 0x40,  //!< 设置线端箭头
        kCopyAll   = 0xFF,  //!< 设置所有属性
        
        kLineDashMask   = 0xFFF,
        
        kArrowNone = 0,     //!< 无箭头
        kArrowSharpClosed,  //!< 实心尖头
        kArrowSharpLine,    //!< 简单线条尖头
        kArrowTLine,        //!< T型箭头
        kArrowSlashLine,    //!< 斜线箭头
        kArrowClosedCircle, //!< 实心圆头
        kArrowOpenedCircle, //!< 空心圆头
    };
    
    //! 默认构造函数
    /*! 绘图参数为3像素宽的黑实线、不填充
    */
    GiContext() : m_lineStyle(kSolidLine), m_lineWidth(-3)
        , m_lineColor(GiColor(0, 0, 0, 168)), m_fillColor(GiColor::Invalid()), m_autoScale(false), m_arrayHead(0) {
    }
    
    //! 指定线条参数来构造
    /*! 填充参数为不填充
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
        \param color 线条颜色， GiColor::Invalid() 表示不画线条
        \param style 线型, 取值为 kSolidLine 等
        \param fillcr 填充颜色， GiColor::Invalid() 表示不填充
        \param autoScale 像素单位线宽(width<0时)是否自动缩放
    */
    GiContext(float width, GiColor color = GiColor::Black(), int style = kSolidLine,
              const GiColor& fillcr = GiColor::Invalid(), bool autoScale = false)
        : m_lineStyle(style), m_lineWidth(width)
        , m_lineColor(color), m_fillColor(fillcr), m_autoScale(autoScale), m_arrayHead(0) {
    }
    
    //! 拷贝构造函数
    GiContext(const GiContext& src) {
        m_lineStyle = src.m_lineStyle;
        m_lineWidth = src.m_lineWidth;
        m_lineColor = src.m_lineColor;
        m_fillColor = src.m_fillColor;
        m_autoScale = src.m_autoScale;
        m_arrayHead = src.m_arrayHead;
    }

    //! 赋值函数, GiContextBits 按位设置
    GiContext& copy(const GiContext& src, int mask = -1) {
        if (this != &src) {
            if (mask & kLineRGB) {
                GiColor c = src.m_lineColor;
                m_lineColor.set(c.r, c.g, c.b);
            }
            if (mask & kLineAlpha) {
                m_lineColor.a = src.m_lineColor.a;
            }
            if (mask & kLineWidth) {
                m_lineWidth = src.m_lineWidth;
                m_autoScale = src.m_autoScale;
            }
            if (mask & kLineStyle) {
                m_lineStyle = src.m_lineStyle;
            }
            if (mask & kFillRGB) {
                GiColor c = src.m_fillColor;
                m_fillColor.set(c.r, c.g, c.b);
            }
            if (mask & kFillAlpha) {
                m_fillColor.a = src.m_fillColor.a;
            }
            if (mask & kLineArrayHead) {
                m_arrayHead = src.m_arrayHead;
            }
        }
        return *this;
    }

    //! 比较相等操作符函数
    bool equals(const GiContext& src) const {
        return (m_lineStyle == src.m_lineStyle
                && m_lineWidth == src.m_lineWidth
                && m_autoScale == src.m_autoScale
                && m_lineColor == src.m_lineColor
                && m_fillColor == src.m_fillColor
                && m_arrayHead == src.m_arrayHead);
    }
    
#ifndef SWIG
    //! 赋值操作符函数
    GiContext& operator=(const GiContext& src) {
        return copy(src);
    }
    
    //! 比较相等操作符函数
    bool operator==(const GiContext& src) const {
        return equals(src);
    }
    
    //! 比较不相等操作符函数
    bool operator!=(const GiContext& src) const {
        return !equals(src);
    }
#endif // SWIG
    
    //! 返回线型, kSolidLine..kNullLine
    int getLineStyle() const {
        return m_lineColor.isInvalid() ? kNullLine : (m_lineStyle & kLineDashMask);
    }
    
    int getLineStyleEx() const {
        return (m_lineColor.isInvalid() ? kNullLine : m_lineStyle) | (m_lineStyle & ~kLineDashMask);
    }
    
    //! 设置线型, kSolidLine..kNullLine
    void setLineStyle(int style, bool all = false) {
        m_lineStyle = all ? style : style | (m_lineStyle & ~kLineDashMask);
    }
    
    //! 返回线宽
    /*! 正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
    */
    float getLineWidth() const {
        return m_lineWidth > 0.f ? fmodf(m_lineWidth, 1e4f) : -fmodf(-m_lineWidth, 1e4f);
    }
    
    //! 返回附加的线宽，像素单位，非负数
    float getExtraWidth() const {
        if (m_lineWidth > 1e4f - 1e-7f)
            return m_lineWidth / 1e5f;
        if (m_lineWidth < -1e4f + 1e-7f)
            return m_lineWidth / -1e5f;
        return 0.f;
    }
    
    //! 返回像素单位线宽是否自动缩放
    bool isAutoScale() const {
        return m_autoScale || m_lineWidth > 0;
    }
    
    //! 设置线宽
    /*!
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
        \param autoScale 像素单位线宽(width<0时)是否自动缩放
    */
    void setLineWidth(float width, bool autoScale) {
        m_lineWidth = width;
        m_autoScale = autoScale;
    }
    
    //! 设置附加的线宽，像素单位，非负数
    void setExtraWidth(float pixels) {
        pixels = floorf(fabsf(pixels * 10.f) + 0.5f) * (m_lineWidth > 0.f ? 1e4f : -1e4f);
        m_lineWidth = getLineWidth() + pixels;
    }
    
    //! 返回是否为空线，即不画线
    bool isNullLine() const {
        return (m_lineStyle & kLineDashMask) == kNullLine || m_lineColor.isInvalid();
    }
    
    //! 设置为空线，即不画线
    /*! 如果要恢复成普通画线状态，可调setLineStyle(kSolid)
    */
    void setNullLine() {
        m_lineStyle = kNullLine | (m_lineStyle & ~kLineDashMask);
    }
    
    //! 返回线条颜色
    GiColor getLineColor() const {
        return m_lineColor;
    }
    
    //! 设置线条颜色， GiColor::Invalid() 表示不画线条
    void setLineColor(const GiColor& color) {
        m_lineColor = color;
    }
    
    //! 设置线条颜色
    void setLineColor(int r, int g, int b) {
        m_lineColor.set(r, g, b);
    }

    //! 设置线条颜色
    void setLineColor(int r, int g, int b, int a) {
        m_lineColor.set(r, g, b, a);
    }

    //! 返回线条ARGB颜色
    int getLineARGB() const {
        return m_lineColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不画线条
    void setLineARGB(int argb) {
        m_lineColor.setARGB(argb);
        setLineColor(m_lineColor);
    }
    
    //! 返回线条透明度
    int getLineAlpha() const {
        return m_lineColor.a;
    }
    
    //! 设置线条透明度，0到255，0表示全透明，255表示不透明
    void setLineAlpha(int alpha) {
        m_lineColor.a = (unsigned char)alpha;
    }
    
    //! 返回是否填充
    bool hasFillColor() const {
        return !m_fillColor.isInvalid();
    }
    
    //! 设置为不填充
    void setNoFillColor() {
        m_fillColor = GiColor::Invalid();
    }
    
    //! 返回填充颜色
    GiColor getFillColor() const {
        return m_fillColor;
    }
    
    //! 设置填充颜色， GiColor::Invalid() 表示不填充
    void setFillColor(const GiColor& color) {
        m_fillColor = color;
    }
    
    //! 设置填充颜色
    void setFillColor(int r, int g, int b) {
        if (m_fillColor.a < 1)
            m_fillColor.a = m_lineColor.a;
        m_fillColor.set(r, g, b);
    }

    //! 设置填充颜色
    void setFillColor(int r, int g, int b, int a) {
        if (m_fillColor.a < 1)
            m_fillColor.a = m_lineColor.a;
        m_fillColor.set(r, g, b, a);
    }

    //! 返回线条ARGB颜色
    int getFillARGB() const {
        return m_fillColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不填充
    void setFillARGB(int argb) {
        m_fillColor.setARGB(argb);
    }
    
    //! 返回填充透明度
    int getFillAlpha() const {
        return m_fillColor.a;
    }
    
    //! 设置填充透明度，0到255，0表示全透明，255表示不透明
    void setFillAlpha(int alpha) {
        if (m_fillColor.a == 0 && alpha > 0)
            m_fillColor = m_lineColor;
        m_fillColor.a = (unsigned char)alpha;
    }
    
    //! 返回是否有线端箭头
    bool hasArrayHead() const {
        return m_arrayHead > 0;
    }
    
    //! 返回起始端的箭头类型, kArrowSharpClosed 等
    int getStartArrayHead() const {
        return m_arrayHead % 100;
    }
    
    //! 设置起始端的箭头类型, kArrowSharpClosed 等
    void setStartArrayHead(int type) {
        if (type >= kArrowNone && type <= kArrowOpenedCircle) {
            m_arrayHead = m_arrayHead / 100 * 100 + type;
        }
    }
    
    //! 返回起始端的箭头类型, kArrowSharpClosed 等
    int getEndArrayHead() const {
        return m_arrayHead / 100;
    }
    
    //! 设置起始端的箭头类型, kArrowSharpClosed 等
    void setEndArrayHead(int type) {
        if (type >= kArrowNone && type <= kArrowOpenedCircle) {
            m_arrayHead = m_arrayHead % 100 + type * 100;
        }
    }
    
private:
    int         m_lineStyle;        //!< 线型, kSolidLine..kNullLine
    float       m_lineWidth;        //!< 线宽, >0: 0.01mm, =0: 1px, <0:px
    GiColor     m_lineColor;        //!< 线条颜色
    GiColor     m_fillColor;        //!< 填充颜色
    bool        m_autoScale;        //!< 像素单位线宽是否自动缩放
    int         m_arrayHead;        //!< 线端箭头，start|end*100, kArrowSharpClosed 等
};

#endif // TOUCHVG_DRAWCONTEXT_H_
