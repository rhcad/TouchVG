//! \file mgshapes.h
//! \brief 定义图形列表类 MgShapes
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPES_H_
#define TOUCHVG_MGSHAPES_H_

#include "mgshape.h"

//! 图形列表类
/*! \ingroup CORE_SHAPE
    \see MgShapeIterator
*/
class MgShapes : public MgObject
{
public:
    //! 返回本对象的类型
    static int Type() { return 1; }
    
    //! 复制出一个新图形列表对象
    MgShapes* cloneShapes() const { return (MgShapes*)clone(); }
    
    //! 创建图形列表
    static MgShapes* create(MgObject* owner = NULL, int index = -1);

    //! 添加一个指定类型的新图形
    MgShape* addShapeByType(MgShapeFactory* factory, int type);

#ifndef SWIG
    MgShape* getFirstShape(void*& it) const;
    MgShape* getNextShape(void*& it) const;
    void freeIterator(void*& it) const;
    typedef bool (*Filter)(const MgShape*);
#endif

    int getShapeCount() const;
    MgShape* getHeadShape() const;
    MgShape* getLastShape() const;
    MgShape* findShape(int sid) const;
    MgShape* findShapeByTag(int tag) const;
    MgShape* findShapeByType(int type) const;
    Box2d getExtent() const;
    
    MgShape* hitTest(const Box2d& limits, MgHitResult& res
#ifndef SWIG
        , Filter filter = NULL) const;
#else
        ) const;
#endif
    
    int draw(GiGraphics& gs, const GiContext *ctx = NULL) const;
    int dyndraw(int mode, GiGraphics& gs, const GiContext *ctx, int segment) const;

    bool save(MgStorage* s, int startIndex = 0) const;
    bool saveShape(MgStorage* s, MgShape* shape, int index) const;
    bool load(MgShapeFactory* factory, MgStorage* s, bool addOnly = false);
    void setNewShapeID(int sid);
    
    //! 删除所有图形
    void clear();
    
    //! 释放临时数据内存
    void clearCachedData();

    //! 复制(深拷贝)每一个图形
    void copyShapes(const MgShapes* src);
    
    //! 复制出新图形并添加到图形列表中
    MgShape* addShape(const MgShape& src);
    
    //! 移除一个图形，由调用者删除图形对象
    MgShape* removeShape(int sid, bool skipLockedShape = true);

    //! 将一个图形移到另一个图形列表
    MgShape* moveTo(int sid, MgShapes* dest);

    //! 将所有图形移到另一个图形列表
    void moveAllShapesTo(MgShapes* dest);
    
    //! 移动图形到最后，以便显示在最前面
    bool bringToFront(int sid);

    //! 返回拥有者对象
    MgObject* getOwner() const;
    
    //! 返回图层序号
    int getIndex() const;
    
    static MgShapes* fromHandle(long h) { MgShapes* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() { long h; *(MgShapes**)&h = this; return h; }   //!< 得到句柄，用于跨库转换
    
public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type(); }

protected:
    MgShapes(MgObject* owner, int index);
    virtual ~MgShapes();
    
    virtual bool saveExtra(MgStorage* s) const { return !!s; }
    virtual bool loadExtra(MgStorage* s) { return !!s; }
    
    struct I;
    I*  im;
};

//! 遍历图形的辅助类
/*! \ingroup CORE_SHAPE
    遍历过程中要避免增删图形。
*/
class MgShapeIterator
{
public:
    //! 给定图形列表(可为空)构造迭代器
    MgShapeIterator(const MgShapes* shapes) : _s(shapes), _it(NULL), _sp(NULL) {}
    ~MgShapeIterator() { if (_it && _s) _s->freeIterator(_it); }
    
    //! 检查是否还有图形可遍历
    bool hasNext() {
        if (!_it && _s) {
            _sp = _s->getFirstShape(_it);
        }
        return !!_sp;
    }
    
    //! 得到当前遍历位置的图形
    /*! 可使用 while (MgShape* sp = it.getNext()) {...} 遍历。
     */
    MgShape* getNext() {
        if (!_it && _s) {
            _sp = _s->getFirstShape(_it);
        }
        MgShape* sp = _sp;
        if (_sp && _s) {
            _sp = _s->getNextShape(_it);
        }
        return sp;
    }

private:
    MgShapeIterator();
    const MgShapes* _s;
    void* _it;
    MgShape* _sp;
};

#endif // TOUCHVG_MGSHAPES_H_
