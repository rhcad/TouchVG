//! \file gigraph.h
//! \brief 定义图形系统类 GiGraphics
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_GRAPHSYS_H_
#define TOUCHVG_GRAPHSYS_H_

#include "gixform.h"
#include "gicontxt.h"
#include "mgpath.h"

#ifndef SWIG
class GiGraphicsImpl;
class GiCanvas;
struct GiTextWidthCallback;
#endif

enum GiHandleTypes {        //!< 符号类型
    kGiHandleVertex,        //!< 顶点或控制点
    kGiHandleHotVertex,     //!< 活动点
    kGiHandleRotate,        //!< 旋转
    kGiHandleLock,          //!< 锁定
    kGiHandleUnlock,        //!< 解锁
    kGiHandleCancel,        //!< 放弃
    kGiHandleAccept,        //!< 完成
    kGiHandleNode,          //!< 顶点
    kGiHandleCenter,        //!< 圆心
    kGiHandleMidPoint,      //!< 中点
    kGiHandleQuadrant,      //!< 象限点
    kGiHandleTangent,       //!< 切点
    kGiHandleIntersect,     //!< 交点
    kGiHandleParallel,      //!< 平行
    kGiHandleNear,          //!< 最近点
    kGiHandlePivot,         //!< 变形中心
    kGiHandleOverturn,      //!< 翻转
    kGiHandleCustom = 20,   //!< 应用自定义
};

//! 图形系统类
/*! 本类用于显示各种图形，图元显示原语由外部的 GiCanvas 实现类来实现。
    显示图形所用的坐标计算和坐标系转换是在 GiTransform 中定义的。
    \ingroup GRAPH_INTERFACE
*/
class GiGraphics
{
public:
    //! 构造函数，自动创建坐标系管理对象
    GiGraphics();
    
    //! 构造函数，坐标系管理对象必须有效
    GiGraphics(GiTransform* xform, bool needFreeXf = false);
    
    //! 拷贝构造函数
    GiGraphics(const GiGraphics& src);

    //! 析构函数
    virtual ~GiGraphics();
    
    static GiGraphics* fromHandle(long h) { GiGraphics* p; *(long*)&p = h; return p; }  //!< 转为对象
    long toHandle() const { long h; *(const GiGraphics**)&h = this; return h; }                     //!< 得到句柄

    enum {  //!< for drawTextAt(..align..)
        kAlignLeft      = 0,
        kAlignCenter    = 1,
        kAlignRight     = 2,
        kAlignTop       = 0,
        kAlignBottom    = 0x10,
        kAlignVCenter   = 0x20,
        kAlignHVCenter  = kAlignCenter|kAlignVCenter,
    };
    
public:
    //! 复制指定对象到本对象
    void copy(const GiGraphics& src);

    //! 返回坐标系管理对象
    const GiTransform& xf() const;

    //! 返回是否正在绘制，即调用了 beginPaint() 还未调用 endPaint()
    bool isDrawing() const;

    //! 返回是否打印或打印预览
    bool isPrint() const;
    
    //! 返回是否需要停止绘图
    bool isStopping() const;
    
    //! 标记需要停止绘图
    void stopDrawing(bool stopped = true);

public:
    //! 返回剪裁框，模型坐标
    Box2d getClipModel() const;

    //! 返回剪裁框，世界坐标
    Box2d getClipWorld() const;

    //! 得到剪裁框，逻辑坐标
    RECT_2D& getClipBox(RECT_2D& rc) const;

    //! 设置剪裁框，逻辑坐标
    /*! 只有正处于绘图状态时，该函数的返回值才有效。
        失败时不改变图形系统内部变量和设备描述表的剪裁框，
        因此在失败时不要进行相应绘图操作，否则处于错误的剪裁框中。
        \param[in] rc 逻辑坐标矩形
        \return 是否成功设置剪裁框，没有处于绘图状态中或计算出的剪裁框为空时失败
    */
    bool setClipBox(const RECT_2D& rc);

    //! 设置剪裁框，世界坐标
    /*! 新剪裁框将会和原始剪裁框相叠加求交，所以函数可能返回失败。\n
        失败时不改变图形系统内部变量和设备描述表的剪裁框，
        因此在失败时不要进行相应绘图操作，否则处于错误的剪裁框中。\n
        只有正处于绘图状态时，该函数的返回值才有效。
        \param[in] rectWorld 世界坐标矩形
        \return 是否成功设置剪裁框，没有处于绘图状态中或计算出的剪裁框为空时失败
    */
    bool setClipWorld(const Box2d& rectWorld);

    //! 返回颜色模式
    bool isGrayMode() const;

    //! 设置颜色模式
    void setGrayMode(bool gray);
    
    //! 返回背景色(打印或打印预览时为白色)
    GiColor getBkColor() const;
    
    //! 设置新的背景色，返回原来的背景色
    GiColor setBkColor(const GiColor& color);

    //! 计算画笔颜色
    /*! 根据颜色模式和设备属性调整显示颜色。\n
        如果传入颜色和背景色相同，本函数不会自动反色，因此可能看不见图形
        \param color 普通的RGB颜色
        \return 实际绘图的RGB颜色
    */
    GiColor calcPenColor(const GiColor& color) const;

    //! 计算画笔宽度
    /*! 打印时会自动调整以像素为单位的线宽，使得不致太细
        \param lineWidth 线宽。正数单位为0.01毫米，负数单位为像素，0表示1像素
        \param useViewScale lineWidth<0时是否使用放缩比例计算线宽
        \return 画笔宽度，非负数，像素
    */
    float calcPenWidth(float lineWidth, bool useViewScale) const;

    //! 设置最大画笔宽度，像素
    /*! 只需要按屏幕显示来设置，打印时会自动调整
        \param pixels 最大画笔宽度，像素，小于0时不改变
        \param minw 最小画笔宽度，像素，小于0时不改变
    */
    void setMaxPenWidth(float pixels, float minw = 1);
    
    //! 设置像素线宽的放大系数
    static void setPenWidthFactor(float factor);
    
    //! 是否允许虚线偏移量
    bool setPhaseEnabled(bool enabled);
    
public:
    //! 绘制直线段，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param startPt 起点
        \param endPt 终点
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawLine(const GiContext* ctx, const Point2d& startPt,
                  const Point2d& endPt, bool modelUnit = true);
    
    //! 绘制射线，模型坐标或世界坐标
    bool drawRayline(const GiContext* ctx, const Point2d& startPt,
                     const Point2d& endPt, bool modelUnit = true);
    
    //! 绘制无穷直线，模型坐标或世界坐标
    bool drawBeeline(const GiContext* ctx, const Point2d& startPt,
                     const Point2d& endPt, bool modelUnit = true);

    //! 绘制折线，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为2
        \param points 顶点数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawLines(const GiContext* ctx, int count, const Point2d* points, bool modelUnit = true);

    //! 绘制多条三次贝塞尔曲线，模型坐标或世界坐标
    /*! 第一条曲线从第一个点绘制到第四个点，以第二个点和第三个点为控制点。
        此序列中的每一条后续曲线都需要三个点：
        前一个曲线的终点作为起点，序列中的另两个点为控制点，第三个点为终点。

        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为4，必须为3的倍数加1
        \param points 控制点和端点的数组，点数为count
        \param closed 是否闭合
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawBeziers(const GiContext* ctx, int count, const Point2d* points,
                     bool closed = false, bool modelUnit = true);
    
    //! 绘制多条贝塞尔曲线，模型坐标或世界坐标
    /*! 第一条曲线从第一个点绘制到第二个点，以第一个矢量和第二个矢量计算中间的两个控制点。
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为4，必须为3的倍数加1
        \param knot 型值点的数组，点数为count
        \param knotvs 型值点的切矢量数组，元素个数为count
        \param closed 是否闭合
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see drawQuadSplines
     */
    bool drawBeziers(const GiContext* ctx, int count,
                     const Point2d* knot, const Vector2d* knotvs,
                     bool closed = false, bool modelUnit = true);

    //! 绘制椭圆弧，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param center 椭圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param startAngle 起始角度，弧度，相对于指定坐标系+X轴，逆时针为正
        \param sweepAngle 转角，弧度，相对于起始角度，逆时针为正，顺时针为负，
            大于2PI或小于-2PI时将取2PI或-2PI
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawArc(const GiContext* ctx, const Point2d& center, float rx, float ry,
                 float startAngle, float sweepAngle, bool modelUnit = true);
    
    //! 给定线上三点绘制椭圆弧，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param startpt 起点
        \param midpt 中点
        \param endpt 终点
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawArc3P(const GiContext* ctx, const Point2d& startpt, 
                   const Point2d& midpt, const Point2d& endpt, bool modelUnit = true);


    //! 绘制并填充多边形，模型坐标或世界坐标
    /*! 通过绘制从最后一个顶点到第一个顶点的线段而自动闭合
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param count 点的个数，至少为2，不需要首末两点重合
        \param points 顶点数组，点数为count
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawPolygon(const GiContext* ctx, int count,
                     const Point2d* points, bool modelUnit = true);
    
    //! 绘制并填充圆，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param center 圆心
        \param r 半径
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
     */
    bool drawCircle(const GiContext* ctx, const Point2d& center, float r,
                    bool modelUnit = true);

    //! 绘制并填充椭圆，模型坐标或世界坐标
    /*! 如果半长轴和半短轴的长度相同，则相当于圆
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param center 圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawEllipse(const GiContext* ctx, const Point2d& center,
                     float rx, float ry, bool modelUnit = true);
    
    //! 绘制并填充椭圆，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 椭圆外框
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
     */
    bool drawEllipse(const GiContext* ctx, const Box2d& rect, bool modelUnit = true);

    //! 绘制并填充椭圆弧饼图，模型坐标或世界坐标
    /*! 绘制一个由椭圆弧和两个半径相交闭合而成的饼状楔形图。
        如果半长轴和半短轴的长度相同，则相当于圆弧。
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param center 椭圆心
        \param rx 椭圆半长轴的长度
        \param ry 椭圆半短轴的长度，为0时取为rx
        \param startAngle 起始角度，弧度，相对于指定坐标系+X轴，逆时针为正
        \param sweepAngle 转角，弧度，相对于起始角度，逆时针为正，顺时针为负，
            大于2PI或小于-2PI时将取2PI或-2PI
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawPie(const GiContext* ctx, const Point2d& center, float rx, float ry,
                 float startAngle, float sweepAngle, bool modelUnit = true);

    //! 绘制并填充直角矩形，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 直角矩形
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawRect(const GiContext* ctx, const Box2d& rect, bool modelUnit = true);

    //! 绘制并填充圆角矩形，模型坐标或世界坐标
    /*! 
        \param ctx 绘图参数，为NULL时取为上一个绘图参数
        \param rect 矩形外框
        \param rx X方向的圆角半径，正数时有效，否则忽略该参数
        \param ry Y方向的圆角半径，为非正数时取为rx
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawRoundRect(const GiContext* ctx, const Box2d& rect,
                       float rx, float ry = 0, bool modelUnit = true);

    //! 绘制三次参数样条曲线(Hermite曲线)，模型坐标或世界坐标
    /*! 切矢量可以通过 mgcurv::cubicSplines 函数计算得到，一般先计算出切矢量供每次显示用
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 型值点的点数，至少为2
        \param knots 型值点坐标数组，元素个数为count
        \param knotvs 型值点的切矢量数组，元素个数为count
        \param closed 是否闭合
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see mgcurv::cubicSplines
    */
    bool drawHermiteSplines(const GiContext* ctx, int count, const Point2d* knots,
        const Vector2d* knotvs, bool closed = false, bool modelUnit = true);

    //! 绘制三次B样条曲线，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 控制点的点数，至少为4
        \param ctlpts 控制点坐标数组，点数为count
        \param closed 是否闭合
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
    */
    bool drawBSplines(const GiContext* ctx, int count, const Point2d* ctlpts,
                      bool closed = false, bool modelUnit = true);
    
    //! 绘制二次B样条曲线，模型坐标或世界坐标
    /*!
        \param ctx 绘图参数，忽略填充参数，为NULL时取为上一个绘图参数
        \param count 控制点的点数，至少为3
        \param ctlpts 控制点坐标数组，点数为count
        \param closed 是否闭合
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功。失败原因为参数错误或超出剪裁区域
        \see drawBeziers
    */
    bool drawQuadSplines(const GiContext* ctx, int count, const Point2d* ctlpts,
                         bool closed = false, bool modelUnit = true);

    //! 显示路径对象
    bool drawPath(const GiContext* ctx, const MgPath& path, bool fill, bool modelUnit = true);
    
    //! 在给定中心位置显示特殊符号
    /*!
        \param pnt 符号中心位置
        \param type 符号类型, GiHandleTypes 枚举值
        \param angle 符号绕其中心点的旋转角度，弧度
        \param modelUnit 指定的坐标尺寸是模型坐标(true)还是世界坐标(false)
        \return 是否显示成功
     */
    bool drawHandle(const Point2d& pnt, int type, float angle = 0, bool modelUnit = true);
    
    //! 在给定模型坐标位置显示指定像素高的单行文本
    /*!
        \param argb 文字颜色，0则为黑色
        \param text 要显示的文字内容
        \param pnt 文字对齐位置，模型坐标
        \param h 文字高度，毫米单位，小于零时跟随显示比例变化
        \param align 水平对齐方式，0-左对齐，1-居中，2-右对齐
        \param angle 文字角度，弧度，以模型坐标系的X正方形为0度，模型坐标系的逆时针方向为角度正方向
        \return 实际显示宽度，毫米
     */
    float drawTextAt(int argb, const char* text, const Point2d& pnt, float h, int align = 1, float angle = 0);

    //! 在显示适配类的 beginPaint() 中调用
    bool beginPaint(GiCanvas* canvas, const RECT_2D& clipBox = RECT_2D());

    //! 在显示适配类的 endPaint() 中调用
    void endPaint();
    
#ifndef SWIG
    //! 返回当前绘图画布对象
    GiCanvas* getCanvas();
    
    //! 返回坐标系管理对象
    GiTransform& _xf();
    
    bool rawLine(const GiContext* ctx, float x1, float y1, float x2, float y2);
    bool rawLines(const GiContext* ctx, const Point2d* pxs, int count);
    bool rawBeziers(const GiContext* ctx, const Point2d* pxs, int count, bool closed = false);
    bool rawPolygon(const GiContext* ctx, const Point2d* pxs, int count);
    bool rawRect(const GiContext* ctx, float x, float y, float w, float h);
    bool rawEllipse(const GiContext* ctx, float x, float y, float w, float h);
    bool rawBeginPath();
    bool rawEndPath(const GiContext* ctx, bool fill);
    bool rawMoveTo(float x, float y);
    bool rawLineTo(float x, float y);
    bool rawBezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    bool rawQuadTo(float cpx, float cpy, float x, float y);
    bool rawClosePath();
    float rawText(const char* text, float x, float y, float h, int align = 1);
    bool rawImage(const char* name, float xc, float yc, float w, float h, float angle);
    bool beginShape(int type, int sid, int version, float x, float y, float w, float h);
    void endShape(int type, int sid, float x, float y);
    float drawTextAt(GiTextWidthCallback* c, int argb, const char* text, const Point2d& pnt, float h, int align = 1, float angle = 0);
#endif
    
private:
    bool setPen(const GiContext* ctx);
    bool setBrush(const GiContext* ctx);
    bool _drawPolygon(const GiContext* ctx, int count, const Point2d* points,
                      bool m2d, bool fill, bool edge, bool modelUnit);
    bool drawPathWithArrayHead(const GiContext& ctx, MgPath& path, int startArray, int endArray);
    void drawArrayHead(const GiContext& ctx, MgPath& path, int type, float px, float scale);
    bool drawPath_(const GiContext* ctx, const MgPath& path, bool fill, const Matrix2d& matD);

private:
    GiGraphics& operator=(const GiGraphics&);

    GiGraphicsImpl* m_impl;     //!< 内部实现
};

//! 保存和恢复图形系统的剪裁框的辅助类
/*! 利用该类在堆栈上定义局部变量，该变量出作用区后自动析构从而恢复剪裁框。
    利用该类可以避免因中途退出或异常时没有执行恢复剪裁框的语句。
    \ingroup GRAPH_INTERFACE
*/
class GiSaveClipBox
{
public:
    //! 构造函数，将新的剪裁框压栈
    /*! 必须调用 succeed 函数检查是否成功，失败时不要继续绘图
        \param gs 要保存剪裁框的图形系统
        \param rectWorld 新剪裁框的世界坐标矩形
        \see succeed
    */
    GiSaveClipBox(GiGraphics* gs, const Box2d& rectWorld)
    {
        m_gs = gs;
        gs->getClipBox(m_clipRect);
        m_clip = gs->setClipWorld(rectWorld);
    }

    //! 析构函数，恢复上一个剪裁框
    ~GiSaveClipBox()
    {
        if (m_clip)
            m_gs->setClipBox(m_clipRect);
    }

    //! 返回是否成功设置了新剪裁框
    /*! 如果返回false，则不要继续在该世界坐标剪裁框下的绘图操作，
        因为失败时表明无法绘制任何图形，但是设置剪裁框时并不改变剪裁框，
        如果还继续绘图，则可能绘制出图形，但已经不在正确的剪裁框中。
        \return 是否成功设置了新剪裁框
    */
    bool succeed() const
    {
        return m_clip;
    }

private:
    GiGraphics* m_gs;
    RECT_2D     m_clipRect;
    bool        m_clip;
};

#endif // TOUCHVG_GRAPHSYS_H_
