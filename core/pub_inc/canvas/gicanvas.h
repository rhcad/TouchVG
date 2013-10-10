//! \file gicanvas.h
//! \brief 定义设备相关的画布接口 GiCanvas
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_GICANVAS_H
#define TOUCHVG_CORE_GICANVAS_H

//! 设备相关的画布回调接口
/*! 在特定平台上使用某一种图形库实现其绘图函数，坐标单位为点（通常等同于像素）。\n
    在派生类中可实现beginPaint()和endPaint()函数，开始绘图时的默认绘图属性为：
    黑色画笔、线宽=1、实线、圆端、反走样、不填充。
    \ingroup GRAPH_INTERFACE
 */
class GiCanvas
{
public:
    virtual ~GiCanvas() {}
    
    //! 设置画笔属性
    /*!
        \param argb 颜色值，如果为0则忽略该参数，包含alpha分量，
            按字节从高到低顺序依次是A、R、G、B分量值，例如 alpha = (argb>>24) & 0xFF
        \param width 线宽，单位为点，正数，如果为零或负数则忽略该参数
        \param style 线型，0-实线，1-虚线，2-点线，3-点划线，4-双点划线，5-空线，
            其余值为自定义线型，如果为负数则忽略该参数。
            实线时线端为圆端，其余线型的线端为平端。
        \param phase 虚线起始偏移位置，style大于0时有效，与线宽成比例，通常为0
     */
    virtual void setPen(int argb, float width, int style, float phase) = 0;
    
    //! 设置画刷填充属性
    /*!
        \param argb 填充色，包含alpha分量，
            按字节从高到低顺序依次是A、R、G、B分量值，例如 alpha = (argb>>24) & 0xFF
        \param style 填充类型，0-实填充，其余正数值为自定义填充.
     */
    virtual void setBrush(int argb, int style) = 0;
    
    
    //! 清除指定矩形区域的显示内容，透明显示，仅用于在视图和图像上的绘制
    virtual void clearRect(float x, float y, float w, float h) = 0;
    
    //! 给定角点坐标和宽高显示矩形框，可描边和填充
    virtual void drawRect(float x, float y, float w, float h, bool stroke, bool fill) = 0;
    
    //! 显示一段直线
    virtual void drawLine(float x1, float y1, float x2, float y2) = 0;
    
    //! 在给定矩形框内显示一个椭圆，可描边和填充
    virtual void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill) = 0;
    
    
    //! 开始新的路径，清除当前路径
    virtual void beginPath() = 0;
    
    //! 在当前路径中添加一个子路径，指定子路径的当前点、起始点
    virtual void moveTo(float x, float y) = 0;
    
    //! 添加一条从当前点到指定点的线段
    virtual void lineTo(float x, float y) = 0;
    
    //! 添加一条从当前点到指定点(x,y)的三次贝塞尔曲线段，中间控制点为(c1x,c1y)、(c2x,c2y)
    virtual void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y) = 0;
    
    //! 添加一条从当前点到指定点(x,y)的二次贝塞尔曲线段，中间控制点为(cpx,cpy)
    virtual void quadTo(float cpx, float cpy, float x, float y) = 0;
    
    //! 闭合当前路径，添加一条从当前点到子路径起始点的线段
    virtual void closePath() = 0;
    
    //! 绘制并清除当前路径
    virtual void drawPath(bool stroke, bool fill) = 0;
    
    //! 保存剪裁区域
    /*! 在调用 clipRect() 或 clipPath() 前调用本函数，使用 restoreClip() 恢复剪裁区域.
        \see restoreClip
     */
    virtual void saveClip() = 0;
    
    //! 恢复剪裁区域. 与 saveClip() 配套使用 */
    virtual void restoreClip() = 0;
    
    //! 设置剪裁区域为当前剪裁区域与给定矩形的交集
    /*! 给定矩形的宽高不一定为正数，所设置的剪裁区域影响后续绘图，可能会清除当前路径.
        \return 是否执行成功并有交集
        \see saveClip, restoreClip
     */
    virtual bool clipRect(float x, float y, float w, float h) = 0;
    
    //! 设置剪裁区域为当前剪裁区域与当前路径的交集
    /*! 调用 beginPath() 等函数设置当前路径，本函数可能会清除当前路径.
        \return 是否执行成功并有交集
        \see saveClip, restoreClip, beginPath
     */
    virtual bool clipPath() = 0;
    
    //! 在给定中心位置显示特殊符号
    /*!
        \param x 符号中心位置X
        \param y 符号中心位置Y
        \param type 符号类型，0-顶点或控制点，1-活动点，2-左旋，3-旋转，4-锁定，5-解锁
     */
    virtual void drawHandle(float x, float y, int type) = 0;
    
    //! 在指定中心位置显示图像
    /*!
        \param name 图像标识，通常为不带路径的文件名，由设备相关实现类决定其含义和图像存放位置
        \param xc 图像中心位置X
        \param yc 图像中心位置Y
        \param w 图像显示宽度
        \param h 图像显示高度
        \param angle 旋转角度，弧度，正方向为世界坐标系的逆时针方向
     */
    virtual void drawBitmap(const char* name, float xc, float yc, 
                            float w, float h, float angle) = 0;
    
    //! 显示一行文字内容
    /*! 使用当前字体和当前填充色显示文字，顶端对齐。
        \param text 一行文字内容，可以有汉字
        \param x 水平对齐位置X
        \param y 文字顶端位置Y
        \param h 文字显示高度，像素点单位
        \param align 水平对齐方式，0-左，1-居中，2-右
        \return 实际显示宽度
     */
    virtual float drawTextAt(const char* text, float x, float y, float h, int align) = 0;
    
#ifndef SWIG
    //! 清除后备缓冲位图，以便重新绘图
    virtual void clearCachedBitmap(bool clearAll = false) { clearAll = !clearAll; }
#endif
};

#endif // TOUCHVG_CORE_GICANVAS_H
