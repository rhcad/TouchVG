//! \file gicolor.h
//! \brief 定义RGB颜色类: GiColor
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_GICOLOR_H_
#define TOUCHVG_GICOLOR_H_

//! RGB颜色
/*!
    \ingroup GRAPH_INTERFACE
    \see GiContext
*/
struct GiColor
{
    unsigned char    r;  //!< Red component, 0 to 255.
    unsigned char    g;  //!< Green component, 0 to 255.
    unsigned char    b;  //!< Blue component, 0 to 255.
    unsigned char    a;  //!< Alpha component, 0 to 255. 0: transparent, 255: opaque.

    GiColor() : r(0), g(0), b(0), a(255)
    {
    }

    GiColor(int _r, int _g, int _b, int _a = 255)
        : r((unsigned char)_r), g((unsigned char)_g)
        , b((unsigned char)_b), a((unsigned char)_a)
    {
    }

    GiColor(const GiColor& c) : r(c.r), g(c.g), b(c.b), a(c.a)
    {
    }

    GiColor(int rgb, bool alpha = false)
    {
        setARGB(rgb);
        if (!alpha)
            a = 255;
    }

    static GiColor White() { return GiColor(255, 255, 255); }
    static GiColor Black() { return GiColor(0, 0, 0); }
    static GiColor Blue() { return GiColor(0, 0, 255); }
    static GiColor Red() { return GiColor(255, 0, 0); }
    static GiColor Green() { return GiColor(0, 255, 0); }
    static GiColor Invalid() { return GiColor(0, 0, 0, 0); }

    int getARGB() const
    {
        return ((unsigned int)a) << 24 | ((unsigned int)r) << 16 | ((unsigned int)g) << 8 | b;
    }

    void setARGB(int value)
    {
        a = (unsigned char)(value >> 24);
        r = (unsigned char)(value >> 16);
        g = (unsigned char)(value >> 8);
        b = (unsigned char)value;
    }

    void set(int _r, int _g, int _b)
    {
        r = (unsigned char)_r;
        g = (unsigned char)_g;
        b = (unsigned char)_b;
        a = (unsigned char)(a ? a : 255);
    }

    void set(int _r, int _g, int _b, int _a)
    {
        r = (unsigned char)_r;
        g = (unsigned char)_g;
        b = (unsigned char)_b;
        a = (unsigned char)_a;
    }
    
    GiColor withAlpha(int alpha) const
    {
        return GiColor(r, g, b, alpha);
    }

    bool isInvalid() const
    {
        return !a && !r && !g && !b;
    }

    bool equals(const GiColor& src) const
    {
        return r==src.r && g==src.g && b==src.b && a==src.a;
    }

#ifndef SWIG
    GiColor& operator=(const GiColor& src)
    {
        r = src.r; g = src.g; b = src.b; a = src.a;
        return *this;
    }
    
    bool operator==(const GiColor& src) const
    {
        return equals(src);
    }

    bool operator!=(const GiColor& src) const
    {
        return !equals(src);
    }
#endif // SWIG
};

#endif // TOUCHVG_GICOLOR_H_
