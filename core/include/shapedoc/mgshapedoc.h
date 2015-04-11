//! \file mgshapedoc.h
//! \brief 定义图形文档 MgShapeDoc
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_SHAPEDOC_H_
#define TOUCHVG_SHAPEDOC_H_

#include "mgshapes.h"

class MgLayer;
struct MgShapeFactory;

//! 图形文档
/*! \ingroup CORE_SHAPE
*/
class MgShapeDoc : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 8; }
    
    //! 复制出一个新图形文档对象，不复制图形
    MgShapeDoc* cloneDoc() const { return (MgShapeDoc*)clone(); }
    
    //! 复制出一个新图形文档对象，并添加所有图形的引用
    MgShapeDoc* shallowCopy() const;
    
    //! 复制(默认为深拷贝)每一个图形，浅拷贝则添加图形的引用计数且不改变图形的拥有者
    int copyShapes(const MgShapeDoc* src, bool deeply);

    //! 创建图形文档对象
    static MgShapeDoc* createDoc();

    //! 保存图形
    bool save(MgStorage* s, int startIndex) const;
    
    //! 保存图形和放缩状态
    bool saveAll(MgStorage* s, const GiTransform* xform);

    //! 加载图形
    bool load(MgShapeFactory* factory, MgStorage* s, bool addOnly);
    
    //! 加载图形，并自动放缩到之前的状态
    bool loadAll(MgShapeFactory* factory, MgStorage* s, GiTransform* xform);

    //! 删除所有图形
    void clear();

    //! 释放临时数据内存
    void clearCachedData();

    //! 显示所有图形
    int draw(GiGraphics& gs) const;
    
    //! 动态显示所有图形
    int dyndraw(int mode, GiGraphics& gs) const;
    
#ifndef SWIG
    //! 显示除了特定ID图形外的所有图形
    int dyndraw(int mode, GiGraphics& gs, const int* ignoreIds) const;
#endif
    
    //! 返回图形范围
    Box2d getExtent() const;

    //! 返回图形总数
    int getShapeCount() const;
    
    //! 查找指定ID的图形
    const MgShape* findShape(int sid) const;
    
    //! 得到最新添加的图形
    const MgShape* getLastShape() const;

    //! 返回当前图形列表（图层或复合图形）
    MgShapes* getCurrentShapes() const;

    //! 设置当前图形列表（图层或复合图形）
    bool setCurrentShapes(MgShapes* shapes);
    
    //! 返回当前图层
    MgLayer* getCurrentLayer() const;

    //! 切换图层，自动追加末尾图层
    bool switchLayer(int index);

    //! 得到图层数量
    int getLayerCount() const;

    //! 返回新图形的图形属性
    GiContext* context();

    //! 模型变换矩阵
    Matrix2d& modelTransform();

    //! 得到页面范围的世界坐标
    const Box2d& getPageRectW() const;

    //! 得到显示比例
    float getViewScale() const;

    //! 设置页面范围的世界坐标
    void setPageRectW(const Box2d& rectW, float viewScale, bool resetInitial = false);
    
    //! 放缩到文档初始状态
    bool zoomToInitial(GiTransform* xform);

    //! 返回文档是否只读
    bool isReadOnly() const;

    //! 设置文档是否只读
    void setReadOnly(bool readOnly);
    
    static MgShapeDoc* fromHandle(long h) { MgShapeDoc* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() const { long h; *(const MgShapeDoc**)&h = this; return h; }   //!< 得到句柄，用于跨库转换

public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual void addRef();
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type(); }

protected:
    MgShapeDoc();
    virtual ~MgShapeDoc();

private:
    struct Impl;
    Impl*  im;
};

#endif // TOUCHVG_SHAPEDOC_H_
