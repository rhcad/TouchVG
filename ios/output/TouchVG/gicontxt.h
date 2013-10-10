//! \file gicontxt.h
//! \brief 定义绘图环境类 GiContext
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_DRAWCONTEXT_H_
#define TOUCHVG_DRAWCONTEXT_H_

#include "gicolor.h"

//! 线型
typedef enum {
    kGiLineSolid = 0,     //!< ----------
    kGiLineDash,          //!< － － － －
    kGiLineDot,           //!< ..........
    kGiLineDashDot,       //!< _._._._._
    kGiLineDashDotdot,    //!< _.._.._.._
    kGiLineNull           //!< Not draw.
} GiLineStyle;

//! 设置属性的位掩码类型
typedef enum {
    kContextCopyNone  = 0,      //!< 不设置属性
    kContextLineRGB   = 0x01,   //!< 设置线色的RGB分量
    kContextLineAlpha = 0x02,   //!< 设置线色的透明度分量
    kContextLineARGB  = 0x03,   //!< 设置线色的所有分量
    kContextLineWidth = 0x04,   //!< 设置线宽
    kContextLineStyle = 0x08,   //!< 设置线型
    kContextFillRGB   = 0x10,   //!< 设置填充色的RGB分量
    kContextFillAlpha = 0x20,   //!< 设置填充色的透明度分量
    kContextFillARGB  = 0x30,   //!< 设置填充色的所有分量
    kContextCopyAll   = 0xFF,   //!< 设置所有属性
} GiContextBits;

//! 绘图参数上下文类
/*! 用于在图形系统的绘图函数中传入绘图参数
    \ingroup GRAPH_INTERFACE
    \see GiLineStyle, GiColor
*/
class GiContext
{
public:
    //! 默认构造函数
    /*! 绘图参数为1像素宽的黑实线、不填充
    */
    GiContext() : m_type(0), m_lineStyle(kGiLineSolid), m_lineWidth(0)
        , m_lineColor(GiColor::Black()), m_fillColor(GiColor::Invalid())
        , m_autoFillColor(false), m_autoScale(false)
    {
    }
    
    //! 指定线条参数来构造
    /*! 填充参数为不填充
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
        \param color 线条颜色， GiColor::Invalid() 表示不画线条
        \param style 线型, GiLineStyle, 取值为 kGiLineSolid 等
        \param fillcr 填充颜色， GiColor::Invalid() 表示不填充
        \param autoScale 像素单位线宽(width<0时)是否自动缩放
    */
    GiContext(float width, GiColor color = GiColor::Black(), int style = kGiLineSolid,
              const GiColor& fillcr = GiColor::Invalid(), bool autoScale = false)
        : m_type(0), m_lineStyle(style), m_lineWidth(width)
        , m_lineColor(color), m_fillColor(fillcr), m_autoFillColor(false), m_autoScale(autoScale)
    {
    }
    
    //! 拷贝构造函数
    GiContext(const GiContext& src)
    {
        m_lineStyle = src.m_lineStyle;
        m_lineWidth = src.m_lineWidth;
        m_lineColor = src.m_lineColor;
        m_fillColor = src.m_fillColor;
        m_autoFillColor = src.m_autoFillColor;
        m_autoScale = src.m_autoScale;
    }

    //! 赋值函数, GiContextBits 按位设置
    GiContext& copy(const GiContext& src, int mask = -1)
    {
        if (this != &src)
        {
            if (mask & 0x01) {
                GiColor c = src.m_lineColor;
                m_lineColor.set(c.r, c.g, c.b);
                if (m_autoFillColor) {
                    m_fillColor = m_lineColor;
                    m_fillColor.a /= 3;
                }
            }
            if (mask & 0x02) {
                m_lineColor.a = src.m_lineColor.a;
                if (m_autoFillColor) {
                    m_fillColor.a = (unsigned char)(m_lineColor.a / 3);
                }
            }
            if (mask & 0x04) {
                m_lineWidth = src.m_lineWidth;
                m_autoScale = src.m_autoScale;
            }
            if (mask & 0x08) {
                m_lineStyle = src.m_lineStyle;
            }
            if (mask & 0x10) {
                GiColor c = src.m_fillColor;
                m_fillColor.set(c.r, c.g, c.b);
                m_autoFillColor = src.m_autoFillColor;
            }
            if (mask & 0x20) {
                m_fillColor.a = src.m_autoFillColor ? (unsigned char)(src.m_lineColor.a / 3) : src.m_fillColor.a;
                m_autoFillColor = src.m_autoFillColor;
            }
        }
        return *this;
    }

    //! 比较相等操作符函数
    bool equals(const GiContext& src) const
    {
        return m_lineStyle == src.m_lineStyle
            && m_lineWidth == src.m_lineWidth
            && m_autoScale == src.m_autoScale
            && m_lineColor == src.m_lineColor
            && m_fillColor == src.m_fillColor
            && m_autoFillColor == src.m_autoFillColor;
    }
    
#ifndef SWIG
    //! 赋值操作符函数
    GiContext& operator=(const GiContext& src)
    {
        return copy(src);
    }
    
    //! 比较相等操作符函数
    bool operator==(const GiContext& src) const
    {
        return equals(src);
    }
    
    //! 比较不相等操作符函数
    bool operator!=(const GiContext& src) const
    {
        return !equals(src);
    }
#endif // SWIG
    
    //! 返回线型, GiLineStyle
    int getLineStyle() const
    {
        return m_lineColor.isInvalid() ? kGiLineNull : m_lineStyle;
    }
    
    //! 设置线型, GiLineStyle
    void setLineStyle(int style)
    {
        m_lineStyle = style;
    }
    
    //! 返回线宽
    /*! 正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
    */
    float getLineWidth() const
    {
        return m_lineWidth;
    }
    
    //! 返回像素单位线宽是否自动缩放
    bool isAutoScale() const
    {
        return m_autoScale || m_lineWidth > 0;
    }
    
    //! 设置线宽
    /*!
        \param width 线宽，正数表示单位为0.01mm，零表示1像素宽，负数时表示单位为像素
        \param autoScale 像素单位线宽(width<0时)是否自动缩放
    */
    void setLineWidth(float width, bool autoScale)
    {
        m_lineWidth = width;
        m_autoScale = autoScale;
    }
    
    //! 返回是否为空线，即不画线
    bool isNullLine() const
    {
        return m_lineStyle == kGiLineNull || m_lineColor.isInvalid();
    }
    
    //! 设置为空线，即不画线
    /*! 如果要恢复成普通画线状态，可调setLineStyle(kGiLineSolid)
    */
    void setNullLine()
    {
        m_lineStyle = kGiLineNull;
    }
    
    //! 返回线条颜色
    GiColor getLineColor() const
    {
        return m_lineColor;
    }
    
    //! 设置线条颜色， GiColor::Invalid() 表示不画线条
    void setLineColor(const GiColor& color)
    {
        m_lineColor = color;
        if (m_autoFillColor) {
            m_fillColor = m_lineColor;
            m_fillColor.a /= 3;
        }
    }
    
    //! 设置线条颜色
    void setLineColor(int r, int g, int b)
    {
        m_lineColor.set(r, g, b);
        if (m_autoFillColor) {
            m_fillColor = m_lineColor;
            m_fillColor.a /= 3;
        }
    }

    //! 设置线条颜色
    void setLineColor(int r, int g, int b, int a)
    {
        m_lineColor.set(r, g, b, a);
        if (m_autoFillColor) {
            m_fillColor = m_lineColor;
            m_fillColor.a /= 3;
        }
    }

    //! 返回线条ARGB颜色
    int getLineARGB() const
    {
        return m_lineColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不画线条
    void setLineARGB(int argb)
    {
        m_lineColor.setARGB(argb);
        setLineColor(m_lineColor);
    }
    
    //! 返回线条透明度
    int getLineAlpha() const
    {
        return m_lineColor.a;
    }
    
    //! 设置线条透明度，0到255，0表示全透明，255表示不透明
    void setLineAlpha(int alpha)
    {
        m_lineColor.a = (unsigned char)alpha;
        if (m_autoFillColor) {
            m_fillColor.a = (unsigned char)(m_lineColor.a / 3);
        }
    }
    
    //! 返回是否填充
    bool hasFillColor() const
    {
        return !m_fillColor.isInvalid();
    }
    
    //! 设置为不填充
    void setNoFillColor()
    {
        m_fillColor = GiColor::Invalid();
        m_autoFillColor = false;
    }
    
    //! 返回填充颜色
    GiColor getFillColor() const
    {
        return m_fillColor;
    }
    
    //! 设置填充颜色， GiColor::Invalid() 表示不填充
    void setFillColor(const GiColor& color)
    {
        m_fillColor = color;
        m_autoFillColor = false;
    }
    
    //! 设置填充颜色
    void setFillColor(int r, int g, int b)
    {
        if (m_fillColor.a < 1)
            m_fillColor.a = m_lineColor.a;
        m_fillColor.set(r, g, b);
        m_autoFillColor = false;
    }

    //! 设置填充颜色
    void setFillColor(int r, int g, int b, int a)
    {
        if (m_fillColor.a < 1)
            m_fillColor.a = m_lineColor.a;
        m_fillColor.set(r, g, b, a);
        m_autoFillColor = false;
    }

    //! 返回线条ARGB颜色
    int getFillARGB() const
    {
        return m_fillColor.getARGB();
    }
    
    //! 设置线条ARGB颜色，0表示不填充
    void setFillARGB(int argb)
    {
        m_fillColor.setARGB(argb);
        m_autoFillColor = false;
    }
    
    //! 返回填充透明度
    int getFillAlpha() const
    {
        return m_fillColor.a;
    }
    
    //! 设置填充透明度，0到255，0表示全透明，255表示不透明
    void setFillAlpha(int alpha)
    {
        if (m_fillColor.a == 0 && alpha > 0)
            m_fillColor = m_lineColor;
        m_fillColor.a = (unsigned char)alpha;
        m_autoFillColor = false;
    }
    
    //! 返回填充颜色是否随线条颜色自动变化
    bool isAutoFillColor() const
    {
        return m_autoFillColor;
    }
    
    //! 设置填充颜色是否随线条颜色自动变化
    void setAutoFillColor(bool value)
    {
        m_autoFillColor = value;
        if (m_autoFillColor) {
            m_fillColor = m_lineColor;
            m_fillColor.a /= 3;
        }
    }
    
    //! 返回绘图环境类型，供派生类用
    /*! 本类始终返回1，派生类可设置 m_type 为其他值
    */
    int getType() const
    {
        return m_type;
    }
    
protected:
    int         m_type;             //!< 派生类可指定其他值来表示不同类型
private:
    int         m_lineStyle;        //!< 线型, GiLineStyle
    float       m_lineWidth;        //!< 线宽, >0: 0.01mm, =0: 1px, <0:px
    GiColor     m_lineColor;        //!< 线条颜色
    GiColor     m_fillColor;        //!< 填充颜色
    bool        m_autoFillColor;    //!< 填充颜色随线条颜色自动变化
    bool        m_autoScale;        //!< 像素单位线宽是否自动缩放
};

#endif // TOUCHVG_DRAWCONTEXT_H_
