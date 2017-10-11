//! \file mgbasesp.h
//! \brief 定义矢量图形基类 MgBaseShape
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGBASESHAPE_H_
#define TOUCHVG_MGBASESHAPE_H_

#include "mgobject.h"
#include "mgmat.h"
#include "mgpath.h"

#include "mgbase.h"
#include "mglnrel.h"
#include "mgnear.h"
#include "mgcurv.h"

struct MgStorage;
struct MgShapeFactory;
class GiGraphics;
class GiContext;

//! 图形特征标志位
typedef enum {
    kMgSquare,          //!< 方形
    kMgClosed,          //!< 闭合
    kMgFixedLength,     //!< 边长固定
    kMgFixedSize,       //!< 大小固定，只能旋转和移动
    kMgRotateDisnable,  //!< 不能旋转
    kMgLocked,          //!< 锁定形状
    kMgNoSnap,          //!< 禁止捕捉
    kMgNoAction,        //!< 禁止上下文按钮
    kMgNoClone,         //!< 禁止克隆
    kMgHideContent,     //!< 隐藏内容
    kMgNoDel,           //!< 禁止删除
    kMgCanSelLocked,    //!< 允许选中，即使锁定
    kMgNotAddRel,       //!< 不自动加约束
    kMgNotShowSnap,     //!< 不显示捕捉提示
    kMgCanAddVertex,    //!< 允许增删顶点
} MgShapeBit;

//! 图形特征点类型
typedef enum {
    kMgHandleVertex,    //!< 顶点
    kMgHandleCenter,    //!< 圆心
    kMgHandleMidPoint,  //!< 中点
    kMgHandleQuadrant,  //!< 象限点
    kMgHandleOutside,   //!< 线外点
    kMgHandleNoSnap,    //!< 不捕捉的线外点
} MgHandleType;

//! 选中点击测试的结果
struct MgHitResult {
    Point2d nearpt; //!< 图形上的最近点
    int segment;    //!< 最近点所在部分的序号，其含义由派生图形类决定
    bool inside;    //!< 是否在闭合图形内部
    bool contained; //!< 是否包含在捕捉盒子中
    float dist;     //!< 给定的外部点到最近点的距离，仅在图形列表的hitTest中有效
    int mask;       //!< 允许捕捉类型位(1<<kMgHandleVertex)顶点|(1<<kMgHandleOutside)线上点
    int ignoreHandle;   //!< 不捕捉的顶点序号
    
    MgHitResult() : segment(-1), inside(false), contained(false), dist(_FLT_MAX), mask(-1), ignoreHandle(-1) {}
    bool snapVertexEnabled() const { return !!(mask & (1<<kMgHandleVertex)); }
    bool snapEdgeEnabled() const { return !!(mask & (1<<kMgHandleOutside)); }
    void disnableSnapVertex() { mask &= ~(1<<kMgHandleVertex); }
    void disnableSnapEdge() { mask &= ~(1<<kMgHandleOutside); }
};

//! 矢量图形基类
/*! \ingroup CORE_SHAPE
    \see MgShapeType, MgShape
*/
class MgBaseShape : public MgObject
{
public:
    MgBaseShape();
    virtual ~MgBaseShape();

    //! 返回本对象的类型
    static int Type() { return 3; }
    
    //! 返回缺省小容差，用于计算包络框等
    static Tol& minTol() {
        static Tol tol(1e-4f);
        return tol;
    }
    
	//! 得到句柄，用于跨库转换
	long toHandle() const { long h; *(const MgBaseShape**)&h = this; return h; }

    //! 复制出一个新图形对象
    MgBaseShape* cloneShape() const { return (MgBaseShape*)clone(); }

    //! 返回图形模型坐标范围
    virtual Box2d getExtent() const;

    //! 返回改变计数，该数将不永久保存
    virtual long getChangeCount() const;
    
    //! 重置改变计数
    virtual void resetChangeCount(long count);
    
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
    virtual bool isCurve() const { return false; }

    //! 选中点击测试
    /*!
        \param[in] pt 外部点的模型坐标，将判断此点能否点中图形
        \param[in] tol 距离公差，正数，超出则不计算最近点
        \param[in,out] res 选中点击测试的结果
        \return 给定的外部点到最近点的距离，失败则为很大的数
    */
    virtual float hitTest(const Point2d& pt, float tol, MgHitResult& res) const = 0;

#ifndef SWIG
    virtual const Point2d* getPoints() const { return (const Point2d*)0; }
    
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
    virtual bool draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const { return false; }
    
    //! 可查询上级图形列表对象的图形显示重载方法
    virtual bool draw2(const MgObject* owner, int mode, GiGraphics& gs, const GiContext& ctx, int segment) const {
        return draw(mode, gs, ctx, segment) && owner;
    }
    
    //! 输出路径
    virtual void output(MgPath& path) const = 0;
    
    //! 返回路径
    MgPath getPath() const { MgPath p; output(p); return p; }
    
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
    virtual void addRef() {}

    //! 设置图形模型坐标范围，供Java等语言代码调用
    void setExtent(const Box2d& rect) { _extent = rect; }

    //! 设置拥有者图形对象
    virtual void setOwner(MgObject* owner) {}
    
    //! 子类型，由具体图形类解释含义
    virtual int getSubType() const { return 0; }
    
    //! 返回是否可见
    bool isVisible() const { return !getFlag(kMgHideContent); }
    
    //! 返回是否锁定
    bool isLocked() const { return getFlag(kMgLocked); }
    
    //! 计算点到折线或多边形的最近距离
    static float linesHit(int n, const Point2d* points, bool closed,
                          const Point2d& pt, float tol, MgHitResult& res);
    
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
            int _flagNoSnap:1;
            int _flagNoAction:1;
            int _flagNoClone:1;
            int _flagHide:1;
            int _flagNoDel:1;
            int _flagCanSelLocked:1;
            int _flagNotAddRel:1;
            int _flagNotShowSnap:1;
            int _flagCanAddVertex:1;
        } _bits;
    };
    long _changeCount;

protected:
    bool _isClosed() const { return getFlag(kMgClosed); }
    void _copy(const MgBaseShape& src);
    bool _equals(const MgBaseShape& src) const;
    bool _isKindOf(int type) const { return type == Type(); }
    Box2d _getExtent() const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    void _clearCachedData() {}
    bool _hitTestBox(const Box2d& rect) const;
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _setHandlePoint2(int index, const Point2d& pt, float tol, int& data);
    bool _isHandleFixed(int) const { return false; }
    int _getHandleType(int) const { return kMgHandleVertex; }
    bool _offset(const Vector2d& vec, int segment);
    bool _rotateHandlePoint(int index, const Point2d& pt);
    bool _draw(int, GiGraphics&, const GiContext&, int) const { return false; }
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
    static Cls* cast(MgBaseShape* obj);                         \
    static Cls* fromHandle(long h);                             \
protected:                                                      \
    bool _isKindOf(int type) const;                             \
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
    virtual bool draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const; \
    virtual void output(MgPath& path) const;                    \
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

#endif // TOUCHVG_MGBASESHAPE_H_
