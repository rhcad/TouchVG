//! \file mgshape.h
//! \brief 定义矢量图形类 MgShape
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGSHAPE_H_
#define TOUCHVG_MGSHAPE_H_

#include "gigraph.h"
#include "gilock.h"
#include "mgbasesp.h"

class MgShapes;

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
	static MgShape* Null() { return (MgShape*)0; } //!< 零指针
    
    static MgShape* fromHandle(long h) { MgShape* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() const { long h; *(const MgShape**)&h = this; return h; }   //!< 得到句柄，用于跨库转换

    //! 复制出一个新图形对象
    MgShape* cloneShape() const { return (MgShape*)clone(); }

    //! 返回显示属性
    virtual const GiContext& context() const = 0;

    //! 设置显示属性
    virtual void setContext(const GiContext& ctx, int mask);
    void setContext(const GiContext& ctx) { setContext(ctx, GiContext::kCopyAll); }

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

    //! 显示内部图形
    static bool drawShape(const MgShapes* shapes, const MgBaseShape& sp, int mode,
                          GiGraphics& gs, const GiContext& ctx, int segment);
    
    //! 返回顶点个数
    virtual int getPointCount() const { return shapec()->getPointCount(); }
    //! 返回指定序号的顶点
    virtual Point2d getPoint(int index) const { return shapec()->getPoint(index); }
    //! 返回控制点个数
    int getHandleCount() const { return shapec()->getHandleCount(); }
    //! 返回指定序号的控制点坐标
    Point2d getHandlePoint(int index) const { return shapec()->getHandlePoint(index); }
    //! 返回指定序号的控制点类型(MgHandleType)
    int getHandleType(int index) const { return shapec()->getHandleType(index); }

protected:
    virtual ~MgShape() {}
};

#endif // TOUCHVG_MGSHAPE_H_
