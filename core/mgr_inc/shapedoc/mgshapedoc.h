//! \file mgshapedoc.h
//! \brief 定义图形文档 MgShapeDoc
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_SHAPEDOC_H_
#define TOUCHVG_SHAPEDOC_H_

#include "mgshapes.h"
#include <mgspfactory.h>

class MgLayer;

#ifndef SWIG
//! 读写锁定数据类
/*! \ingroup CORE_SHAPE
*/
class MgLockRW
{
public:
    MgLockRW();
    bool lockData(bool forWrite, int timeout = 200);
    long unlockData(bool forWrite);

    bool firstLocked() const;
    bool lockedForRead() const;
    bool lockedForWrite() const;

    int getEditFlags() const { return _editFlags; }
    void setEditFlags(int flags) {
        _editFlags = flags ? (_editFlags | flags) : 0;
    }

private:
    volatile long _counts[3];
    int     _editFlags;
};
#endif

//! 图形文档
/*! \ingroup CORE_SHAPE
*/
class MgShapeDoc : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 8; }

    //! 复制出一个新图形文档对象
    MgShapeDoc* cloneDoc() const { return (MgShapeDoc*)clone(); }

    //! 创建图形文档对象
    static MgShapeDoc* createDoc();

    //! 增加引用
    void addRef();

    //! 保存图形
    bool save(MgStorage* s, int startIndex = 0) const;

    //! 加载图形
    bool load(MgShapeFactory* factory, MgStorage* s, bool addOnly = false);

    //! 删除所有图形
    void clear();

    //! 释放临时数据内存
    void clearCachedData();

    //! 显示所有图形
    int draw(GiGraphics& gs) const;

    //! 返回图形范围
    Box2d getExtent() const;

    //! 返回图形总数
    int getShapeCount() const;

    //! 返回当前图形列表（图层或复合图形）
    MgShapes* getCurrentShapes() const;

    //! 设置当前图形列表（图层或复合图形）
    bool setCurrentShapes(MgShapes* shapes);

    //! 切换图层，自动追加末尾图层
    bool switchLayer(int index);

    //! 得到图层数量
    int getLayerCount() const;

    //! 返回新图形的图形属性
    GiContext* context();

    //! 模型变换矩阵
    Matrix2d& modelTransform();

    //! 得到页面范围的世界坐标
    Box2d getPageRectW() const;

    //! 得到显示比例
    float getViewScale() const;

    //! 设置页面范围的世界坐标
    void setPageRectW(const Box2d& rectW, float viewScale);

    //! 返回改变计数
    int getChangeCount() const;

#ifndef SWIG
    typedef void (*ShapesLocked)(MgShapeDoc* doc, void* obj, bool locked);
    void registerObserver(ShapesLocked func, void* obj);
    void unregisterObserver(ShapesLocked func, void* obj);

    MgLockRW* getLockData() const;
    MgLockRW* getDynLockData() const;
    bool lockData(int flags, int timeout);
    long unlockData(bool forWrite);
    void afterChanged();
#endif

public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
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
