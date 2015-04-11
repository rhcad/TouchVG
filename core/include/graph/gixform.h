//! \file gixform.h
//! \brief 定义坐标系管理类 GiTransform
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_TRANSFORMSYS_H_
#define TOUCHVG_TRANSFORMSYS_H_

#include "mgbox.h"
#include "mgmat.h"

struct GiTransformImpl;

//! 坐标系管理类
/*!
    \ingroup GRAPH_INTERFACE
    坐标系管理类的功能是管理图形系统用到的各种坐标系及其转换。
    用到以下三种坐标系：\n
    1. 世界坐标系：固定不变的笛卡尔坐标系，用作定义所有对象和其他坐标系的基础，
    　 用单位矩阵表示，单位为毫米。\n
    2. 模型坐标系：用户定义的坐标系，用于描述真实世界中的图形，其单位、比例尺、
    　 X轴和Y轴的方向可以任意独立设定。模型坐标系用一个变换矩阵(Matrix)表示。\n
    3. 显示坐标系：在显示器、打印机、绘图仪上显示时用的坐标系，单位为像素，
    　 +X向右，+Y向下，原点在显示窗口的左上角。显示坐标系由显示窗口的中心的世界
    　 坐标、显示比例、显示设备的分辨率(DPI)共同决定，可以计算出一个变换矩阵。
*/
class GiTransform
{
public:
    //! 默认构造函数
    /*! 对象属性为：96dpi，显示窗口中心的世界坐标为(0,0)，显示比例为100％，
        模型坐标系的变换矩阵为单位矩阵，显示比例范围为1％到500％，
        显示极限的世界坐标范围为(-1e5f,-1e5f)到(1e5f,1e5f)
        \param ydown true表示显示设备的+Y方向为向下，false则向上
    */
    GiTransform(bool ydown = true);

    //! 拷贝构造函数
    /*! 不复制 enableZoom 相应参数
        \param src 源对象
    */
    GiTransform(const GiTransform& src);

    //! 析构函数
    ~GiTransform();

    //! 赋值函数
    /*! 不复制 enableZoom 相应参数
        \param src 源对象
        \return 本对象的引用
    */
    GiTransform& copy(const GiTransform& src);
    
public:
    //! 返回当前绘图的设备X分辨率
    /*! 屏幕窗口显示时为屏幕分辨率，打印或打印预览时为打印机分辨率
    */
    float getDpiX() const;

    //! 返回当前绘图的设备Y分辨率
    /*! 屏幕窗口显示时为屏幕分辨率，打印或打印预览时为打印机分辨率
    */
    float getDpiY() const;

    //! 返回显示窗口的宽度，像素
    /*! 屏幕显示时为窗口的客户区宽度，打印和打印预览时为纸张可打印区域宽度
    */
    int getWidth() const;

    //! 返回显示窗口的高度，像素
    /*! 屏幕显示时为窗口的客户区高度，打印和打印预览时为纸张可打印区域高度
    */
    int getHeight() const;

    //! 返回显示窗口中心的世界坐标
    Point2d getCenterW() const;

    //! 返回显示比例
    /*! 显示比例为1时表示100％显示，小于1时缩小显示，大于1时放大显示
    */
    float getViewScale() const;

    //! 返回X方向上世界单位对应的像素数
    /*! 该值表示X方向上世界坐标系中1单位长度对应于显示坐标系中的长度(像素)，
        由显示设备分辨率和显示比例决定，即为 getDpiX() * getViewScale()
    */
    float getWorldToDisplayX(bool useViewScale = true) const;

    //! 返回Y方向上世界单位对应的像素数
    /*! 该值表示Y方向上世界坐标系中1单位长度对应于显示坐标系中的长度(像素)，
        由显示设备分辨率和显示比例决定，即为 getDpiY() * getViewScale()
    */
    float getWorldToDisplayY(bool useViewScale = true) const;
    
    //! 返回显示长度(默认为像素，可为毫米）对应的模型长度
    float displayToModel(float dist, bool mm = false) const;

    //! 返回模型坐标系到世界坐标系的变换矩阵
    const Matrix2d& modelToWorld() const;

    //! 返回世界坐标系到模型坐标系的变换矩阵
    const Matrix2d& worldToModel() const;

    //! 返回显示坐标系到世界坐标系的变换矩阵
    const Matrix2d& displayToWorld() const;

    //! 返回世界坐标系到显示坐标系的变换矩阵
    const Matrix2d& worldToDisplay() const;

    //! 返回显示坐标系到模型坐标系的变换矩阵
    const Matrix2d& displayToModel() const;

    //! 返回模型坐标系到显示坐标系的变换矩阵
    const Matrix2d& modelToDisplay() const;

    //! 设置显示窗口的大小，像素
    /*! 显示窗口在屏幕显示时为窗口的客户区矩形，打印和打印预览时为纸张可打印区域。\n
        屏幕显示时，一般是在窗口的WM_SIZE事件的响应函数中调用该函数，例如在
        OnSize(UINT nType, int cx, int cy)函数中调用setWndSize(cx, cy)。\n
        打印和打印预览时，如果调用了图形系统的 printSetup 函数，则不用再调用本函数，
        否则需要将纸张可打印区域的大小作为参数调用本函数
        \param width 显示窗口的宽度，像素
        \param height 显示窗口的高度，像素
        \return 是否改变了宽高
    */
    bool setWndSize(int width, int height);

    //! 设置显示分辨率
    /*! 一般不直接调用本函数，而是调用图形系统的 beginPaint 或 printSetup 函数
        \param dpiX 水平分辨率DPI
        \param dpiY 垂直分辨率DPI，为0则取为dpiX
    */
    void setResolution(float dpiX, float dpiY = 0);

    //! 设置模型坐标系的变换矩阵
    /*! 建议在调用图形系统的 beginPaint 函数前调用本函数。\n
        可用 Matrix2d::coordSystem 函数计算变换矩阵，
        可用 GiSaveModelTransform 类保存和恢复模型坐标系的变换矩阵
        \param mat 模型坐标系到世界坐标系的变换矩阵，必须可逆
    */
    bool setModelTransform(const Matrix2d& mat);
    
    //! 返回显示窗口的世界坐标范围
    Box2d getWndRectW() const;

    //! 返回显示窗口的模型坐标范围
    Box2d getWndRectM() const;
    
    //! 返回显示窗口的显示坐标范围
    Box2d getWndRect() const;

    //! 返回最小显示比例
    float getMinViewScale() const;

    //! 返回最大显示比例
    float getMaxViewScale() const;

    //! 返回显示极限的世界坐标范围
    Box2d getWorldLimits() const;

    //! 设置显示比例范围
    /*!
        \param minScale 最小显示比例，在1e-5到0.5之间
        \param maxScale 最大显示比例，在1.0到20之间
    */
    void setViewScaleRange(float minScale, float maxScale);

    //! 设置显示极限的世界坐标范围
    /*! 如果为空矩形框，则放缩时不限制显示位置，否则将限制在显示极限的世界坐标范围内
        \param rect 世界坐标范围
        \return 原来的坐标极限范围
    */
    Box2d setWorldLimits(const Box2d& rect);

    
    //! 开窗放大
    /*! 将给定的两点形成的矩形中的图形完全显示到显示窗口整个矩形中，
        并使放缩比例最小，且将给定的两点的中点处的图形位置平移到显示窗口的中心
        \param pt1 显示窗口中的一点，逻辑坐标
        \param pt2 显示窗口中的另一点，逻辑坐标
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomWnd(const Point2d& pt1, const Point2d& pt2, bool adjust = true);

    //! 放缩图形到窗口区域
    /*! 将指定的图形范围rectWorld放缩显示到显示窗口中的指定区域rcTo，
        并使放缩比例最小，且将给定的两点的中点处的图形位置平移到显示窗口的中心
        \param rectWorld 要显示的图形范围，世界坐标
        \param rcTo 显示窗口中的指定区域，逻辑坐标，如果为NULL则为整个显示窗口范围
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomTo(const Box2d& rectWorld, 
        const RECT_2D* rcTo = (RECT_2D*)0, bool adjust = true);

    //! 平移显示
    /*! 将一个世界坐标点平移显示到屏幕上指定点
        \param pntWorld 图形的世界坐标点
        \param pxAt 显示窗口中的一点，逻辑坐标，如果为NULL则为显示窗口的中心
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomTo(const Point2d& pntWorld, 
        const Point2d* pxAt = (Point2d*)0, bool adjust = true);

    //! 平移显示
    /*! 
        \param dxPixel 图形向右的平移量，像素
        \param dyPixel 图形向上的平移量，像素
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomPan(float dxPixel, float dyPixel, bool adjust = true);

    //! 以一点为中心按照倍率放缩
    /*! 当factor大于0时，显示比例将变为原来的(1+factor)倍；\n
        当factor小于0时，显示比例将变为原来的1/(1+fabs(factor))倍
        \param factor 放缩倍率，大于0时放大，小于0时缩小
        \param pxAt 显示窗口中保持不动的点的逻辑坐标，为NULL时取显示窗口中心点
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomByFactor(float factor, 
        const Point2d* pxAt = (Point2d*)0, bool adjust = true);

    //! 以一点为中心指定比例放缩
    /*! 
        \param viewScale 显示比例
        \param pxAt 显示窗口中保持不动的点的逻辑坐标，为NULL时取显示窗口中心点
        \param adjust 如果显示比例或位置超出范围，是否调整
        \return 是否放缩成功
    */
    bool zoomScale(float viewScale, 
        const Point2d* pxAt = (Point2d*)0, bool adjust = true);

    //! 设置显示放缩状态
    /*! 
        \param[in] centerW 显示窗口中心的世界坐标
        \param[in] viewScale 显示比例
        \return 填充是否已改变放缩参数的标记
    */
    bool zoom(Point2d centerW, float viewScale);

    //! 设置各种放缩函数是否立即生效
    /*! 本函数可用于检查放缩参数的有效性，避免改变图形系统的放缩状态；
        在拷贝函数中不复制本参数。
        \param enabled 新的允许状态。true: 直接改变放缩状态，立即生效；
            false: 仅记录临时放缩参数，不触发改变通知
        \return 原来的允许状态，用于恢复状态用
    */
    bool enableZoom(bool enabled);

    //! 得到上一次放缩函数的结果
    /*! 不论 enableZoom 设置什么值，都能获取放缩结果
        \param[out] centerW 显示窗口中心的世界坐标
        \return viewScale 显示比例
    */
    float getZoomValue(Point2d& centerW) const;

    //! 返回放缩结果改变的次数，供图形系统等观察者作比较使用
    long getZoomTimes() const;

private:
    GiTransformImpl*    m_impl;
};

//! 保存和恢复模型坐标系的变换矩阵的辅助类
/*! 利用该类在堆栈上定义局部变量，该变量出作用区后自动析构从而恢复模型坐标系。
    利用该类可以避免因中途退出或异常时没有执行恢复模型坐标系的语句。
    \ingroup GRAPH_INTERFACE
*/
class GiSaveModelTransform
{
public:
    //! 构造函数，将新的模型坐标系压栈
    /*!
        \param xform 要保存模型坐标系的图形系统
        \param mat 变换矩阵，在原来的模型坐标系基础上施加附加的几何变换，
            即设置模型坐标系的变换矩阵为原来矩阵乘以指定矩阵xMat的结果
    */
    GiSaveModelTransform(const GiTransform* xform, const Matrix2d& mat)
        : m_xform(const_cast<GiTransform*>(xform))
        , m_mat(xform->modelToWorld())
    {
        m_xform->setModelTransform(m_mat * mat);
    }

    //! 析构函数，恢复上一个模型坐标系的变换矩阵
    ~GiSaveModelTransform()
    {
        m_xform->setModelTransform(m_mat);
    }

private:
    GiTransform*    m_xform;
    Matrix2d        m_mat;
};

#endif // TOUCHVG_TRANSFORMSYS_H_
