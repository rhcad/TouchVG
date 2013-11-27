//! \file mgshape.h
//! \brief 定义矢量图形基类 MgBaseShape 和 MgShape
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPE_H_
#define TOUCHVG_MGSHAPE_H_

#include "gigraph.h"
#include "gilock.h"
#include "mgobject.h"

class MgBaseShape;
class MgShape;
class MgShapes;
struct MgStorage;
struct MgShapeFactory;

//! 矢量图形接口
/*! \ingroup CORE_SHAPE
    \interface MgShape
    \see MgShapeType, MgBaseShape
*/
class MgShape : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 2; }

    //! 复制出一个新图形对象
    MgShape* cloneShape() const { return (MgShape*)clone(); }

    //! 返回显示属性
    virtual GiContext* context() = 0;

    //! 返回显示属性(const)
    virtual const GiContext* contextc() const = 0;

    //! 返回几何图形对象
    virtual MgBaseShape* shape() = 0;

    //! 返回几何图形对象(const)
    virtual const MgBaseShape* shapec() const = 0;

    //! 返回是否为闭合填充图形
    virtual bool hasFillColor() const;

    //! 显示图形
    /*!
        \param mode 绘图方式，0-正常显示，1-选中显示，2-拖动显示
        \param gs 绘图系统
        \param ctx 特殊显示参数，为NULL时使用图形自身显示属性
        \param segment 子段号，来源于图形的hitTest
        \return 是否显示了图形，通常返回true
    */
    virtual bool draw(int mode, GiGraphics& gs, const GiContext *ctx, int segment) const;
    
    //! 保存图形到指定的序列化对象
    virtual bool save(MgStorage* s) const;

    //! 从指定的序列化对象加载图形
    virtual bool load(MgShapeFactory* factory, MgStorage* s);

    //! 返回图形编号
    virtual int getID() const = 0;

    //! 得到上一级图形列表
    virtual MgShapes* getParent() const = 0;

    //! 由图形列表设置拥有者和编号
    virtual void setParent(MgShapes* p, int sid) = 0;

    //! 返回应用程序自定义标记
    virtual int getTag() const = 0;

    //! 设置应用程序自定义标记，可由此扩展数据
    virtual void setTag(int tag) = 0;

    virtual void copy(const MgObject& src);
    virtual bool equals(const MgObject& src) const;
    virtual bool isKindOf(int type) const;

protected:
    virtual ~MgShape() {}
};

//! 图形特征标志位
typedef enum {
    kMgSquare,          //!< 方形
    kMgClosed,          //!< 闭合
    kMgFixedLength,     //!< 边长固定
    kMgFixedSize,       //!< 大小固定，只能旋转和移动
    kMgRotateDisnable,  //!< 不能旋转
    kMgShapeLocked,     //!< 锁定形状
} MgShapeBit;

//! 图形特征点类型
typedef enum {
    kMgHandleVertext,   //!< 顶点
    kMgHandleCenter,    //!< 圆心
    kMgHandleMidPoint,  //!< 中点
    kMgHandleOutside,   //!< 线外点
} MgHandleType;

//! 选中点击测试的结果
struct MgHitResult {
    Point2d nearpt; //!< 图形上的最近点
    int segment;    //!< 最近点所在部分的序号，其含义由派生图形类决定
    bool inside;    //!< 是否在闭合图形内部
    float dist;     //!< 给定的外部点到最近点的距离，仅在图形列表的hitTest中有效
    MgHitResult() : segment(-1), inside(false), dist(_FLT_MAX) {}
};

//! 矢量图形基类
/*! \ingroup CORE_SHAPE
    \see MgShapeType, MgShape
*/
class MgBaseShape : public MgObject
{
public:
    MgBaseShape() : _flags(0), _changeCount(0) {}
    virtual ~MgBaseShape() {}

    //! 返回本对象的类型
    static int Type() { return 3; }
    
    //! 返回确实小容差，用于计算包络框等
    static Tol& minTol()
    {
        static Tol tol;
        return tol;
    }

    //! 复制出一个新图形对象
    MgBaseShape* cloneShape() const { return (MgBaseShape*)clone(); }
    
    //! 传入拥有者对象
    virtual void setOwner(MgShape* owner) {}

    //! 返回图形模型坐标范围
    virtual Box2d getExtent() const;

    //! 返回改变计数
    virtual long getChangeCount() const;
    
    //! update() 或改变图形内容后调用
    virtual void afterChanged();

    //! 参数改变后重新计算坐标
    virtual void update();

    //! 矩阵变换
    virtual void transform(const Matrix2d& mat);

    //! 清除图形数据
    virtual void clear();
    
    //! 释放临时数据内存
    virtual void clearCachedData();

    //! 返回顶点个数
    virtual int getPointCount() const = 0;

    //! 返回指定序号的顶点
    virtual Point2d getPoint(int index) const = 0;

    //! 设置指定序号的顶点坐标，需再调用 update()
    virtual void setPoint(int index, const Point2d& pt) = 0;

    //! 返回是否闭合
    virtual bool isClosed() const;
    
    //! 返回是否为曲线图形
    virtual bool isCurve() const = 0;

    //! 选中点击测试
    /*!
        \param[in] pt 外部点的模型坐标，将判断此点能否点中图形
        \param[in] tol 距离公差，正数，超出则不计算最近点
        \param[in,out] res 选中点击测试的结果
        \return 给定的外部点到最近点的距离，失败则为很大的数
    */
    virtual float hitTest(const Point2d& pt, float tol, MgHitResult& res) const = 0;

#ifndef SWIG
    //! 设置指定序号的控制点坐标，可以处理拖动状态
    virtual bool setHandlePoint2(int index, const Point2d& pt, float tol, int& data) {
        return _setHandlePoint2(index, pt, tol, data);
    }

    //! 选中点击测试，可输出段号
    float hitTest2(const Point2d& pt, float tol, Point2d& nearpt, int& segment) const {
        MgHitResult res;
        float dist = hitTest(pt, tol, res);
        nearpt = res.nearpt;
        segment = res.segment;
        return dist;
    }
#endif
    //! 选中点击测试
    float hitTest2(const Point2d& pt, float tol, Point2d& nearpt) const {
        MgHitResult res;
        float dist = hitTest(pt, tol, res);
        nearpt = res.nearpt;
        return dist;
    }
    
    //! 框选检查
    virtual bool hitTestBox(const Box2d& rect) const;

    //! 显示图形（mode：0-正常显示，1-选中显示，2-拖动显示）
    virtual bool draw(int mode, GiGraphics& gs, 
        const GiContext& ctx, int segment) const;
    
    //! 保存图形
    virtual bool save(MgStorage* s) const;
    
    //! 恢复图形
    virtual bool load(MgShapeFactory* factory, MgStorage* s);
    
    //! 返回控制点个数
    virtual int getHandleCount() const;
    
    //! 返回指定序号的控制点坐标
    virtual Point2d getHandlePoint(int index) const;
    
    //! 设置指定序号的控制点坐标，指定的容差用于比较重合点
    virtual bool setHandlePoint(int index, const Point2d& pt, float tol);
    
    //! 返回指定序号的控制点是否不允许移动
    virtual bool isHandleFixed(int index) const;
    
    //! 返回指定序号的控制点类型(MgHandleType)
    virtual int getHandleType(int index) const;
    
    //! 移动图形, 子段号 segment 由 hitTest() 得到
    virtual bool offset(const Vector2d& vec, int segment);
    
    //! 得到图形特征标志位
    bool getFlag(MgShapeBit bit) const;
    
    //! 设置图形特征标志位
    virtual void setFlag(MgShapeBit bit, bool on);

    virtual void copy(const MgObject& src);
    virtual bool equals(const MgObject& src) const;
    virtual bool isKindOf(int type) const;

    //! 设置图形模型坐标范围，供Java等语言代码调用
    void setExtent(const Box2d& rect) { _extent = rect; }
    
protected:
    Box2d   _extent;
    union {
        int _flags;
        struct {
            int _flagSquare:1;
            int _flagClosed:1;
            int _flagFixedLength:1;
            int _flagFixedSize:1;
            int _flagRotateDisnable:1;
            int _flagShapeLocked:1;
        } _bits;
    };
    long _changeCount;

protected:
    bool _isClosed() const { return getFlag(kMgClosed); }
    void _copy(const MgBaseShape& src);
    bool _equals(const MgBaseShape& src) const;
    bool _isKindOf(int type) const { return type == Type(); }
    Box2d _getExtent() const { return _extent; }
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    void _clearCachedData() {}
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;
    bool _hitTestBox(const Box2d& rect) const;
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _setHandlePoint2(int index, const Point2d& pt, float tol, int& data);
    bool _isHandleFixed(int) const { return false; }
    int _getHandleType(int) const { return kMgHandleVertext; }
    bool _offset(const Vector2d& vec, int segment);
    bool _rotateHandlePoint(int index, const Point2d& pt);
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
};

#if !defined(_MSC_VER) || _MSC_VER <= 1200
#define MG_DECLARE_DYNAMIC(Cls, Base)                           \
    typedef Base __super;
#else
#define MG_DECLARE_DYNAMIC(Cls, Base)
#endif

#define MG_INHERIT_CREATE(Cls, Base, TypeNum)                   \
    MG_DECLARE_DYNAMIC(Cls, Base)                               \
public:                                                         \
    Cls();                                                      \
    virtual ~Cls();                                             \
    static Cls* create();                                       \
    static int Type() { return TypeNum; }                       \
protected:                                                      \
    bool _isKindOf(int type) const;                             \
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const; \
public:                                                         \
    virtual MgObject* clone() const;                            \
    virtual void copy(const MgObject& src);                     \
    virtual void release();                                     \
    virtual bool equals(const MgObject& src) const;             \
    virtual int getType() const { return Type(); }              \
    virtual bool isKindOf(int type) const { return _isKindOf(type); } \
    virtual Box2d getExtent() const;                            \
    virtual void update();                                      \
    virtual void transform(const Matrix2d& mat);                \
    virtual void clear();                                       \
    virtual void clearCachedData();                             \
    virtual int getPointCount() const;                          \
    virtual Point2d getPoint(int index) const;                  \
    virtual void setPoint(int index, const Point2d& pt);        \
    virtual bool isClosed() const;                              \
    virtual bool hitTestBox(const Box2d& rect) const;           \
    virtual bool draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;  \
    virtual bool save(MgStorage* s) const;                      \
    virtual bool load(MgShapeFactory* factory, MgStorage* s);   \
    virtual int getHandleCount() const;                         \
    virtual Point2d getHandlePoint(int index) const;            \
    virtual bool setHandlePoint(int index, const Point2d& pt, float tol);   \
    virtual bool isHandleFixed(int index) const;                \
    virtual int getHandleType(int index) const;                 \
    virtual bool offset(const Vector2d& vec, int segment);      \
    virtual float hitTest(const Point2d& pt, float tol, MgHitResult& res) const; \
protected:                                                      \
    virtual bool setHandlePoint2(int index, const Point2d& pt, float tol, int& data);

#define MG_DECLARE_CREATE(Cls, Base, TypeNum)                   \
    MG_INHERIT_CREATE(Cls, Base, TypeNum)                       \
protected:                                                      \
    void _copy(const Cls& src);                                 \
    bool _equals(const Cls& src) const;                         \
    void _update();                                             \
    void _transform(const Matrix2d& mat);                       \
    void _clear();                                              \
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const; \
    int _getPointCount() const;                                 \
    Point2d _getPoint(int index) const;                         \
    void _setPoint(int index, const Point2d& pt);

#endif // TOUCHVG_MGSHAPE_H_
