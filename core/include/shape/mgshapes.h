//! \file mgshapes.h
//! \brief 定义图形列表类 MgShapes
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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
    
    //! 复制出一个新图形列表对象，并添加所有图形的引用
    MgShapes* shallowCopy() const;
    
    //! 创建图形列表
    static MgShapes* create(MgObject* owner = (MgObject*)0, int index = -1);

#ifndef SWIG
    const MgShape* getFirstShape(void*& it) const;
    const MgShape* getNextShape(void*& it) const;
    void freeIterator(void*& it) const;
    typedef bool (*Filter)(const MgShape* sp, void* data);
    int traverseByType(int type, void (*c)(const MgShape*, void*), void* d);
#endif

    int getShapeCount() const;
    int getShapeCountByTypeOrTag(int type, int tag) const;
    int getShapeIndex(int sid) const;
    const MgShape* getShapeAtIndex(int index) const;
    const MgShape* getHeadShape() const;
    const MgShape* getLastShape() const;
    const MgShape* findShape(int sid) const;
    const MgShape* findShapeByTag(int tag) const;
    const MgShape* findShapeByType(int type) const;
    const MgShape* findShapeByTypeAndTag(int type, int tag) const;
    Box2d getExtent() const;
    
    const MgShape* hitTest(const Box2d& limits, MgHitResult& res
#ifndef SWIG
        , Filter filter = (Filter)0, void* data = (void*)0) const;
#else
        ) const;
#endif
    
    int draw(GiGraphics& gs, const GiContext *ctx = (const GiContext*)0) const;
#ifndef SWIG
    int dyndraw(int mode, GiGraphics& gs, const GiContext *ctx, int segment,
                const int* ignoreIds = (const int*)0) const;
#endif

    bool save(MgStorage* s, int startIndex = 0) const;
    bool saveShape(MgStorage* s, const MgShape* shape, int index) const;
    int load(MgShapeFactory* factory, MgStorage* s, bool addOnly = false);
    void setNewShapeID(int sid);
    
    //! 删除所有图形
    void clear();
    
    //! 释放临时数据内存
    void clearCachedData();

    //! 复制(默认为深拷贝)每一个图形，浅拷贝则添加图形的引用计数且不改变图形的拥有者
    int copyShapes(const MgShapes* src, bool deeply = true, bool needClear = true);
    
    //! 复制出新图形并添加到图形列表中
    MgShape* addShape(const MgShape& src);
    
    //! 添加新图形到图形列表中，自动更新图形的包络框
    bool addShapeDirect(MgShape* shape, bool force = true);
    
    //! 更新为新的图形，该图形从原来图形克隆得到. 原图形对象会被删除!
    bool updateShape(MgShape* shape, bool force = true);
    
#ifndef SWIG
    //! 更新为新的图形，该图形从原来图形克隆得到. 原图形对象(oldsp)会被释放并指向新的图形对象
    bool updateShape(const MgShape*& oldsp, MgShape* newsp) {
        bool ret = updateShape(newsp, true);
        if (ret) {
            oldsp = newsp;
        } else {
            newsp->release();
        }
        return ret;
    }
    //! 指定新的顺序
    bool reorderShapes(int n, const int *ids);
#endif
    
    //! 复制出一个新图形对象
    MgShape* cloneShape(int sid) const;
    
    //! 对每个图形进行变形
    void transform(const Matrix2d& mat);
    
    //! 移除一个图形
    bool removeShape(int sid);

    //! 将一个图形移到另一个图形列表
    bool moveShapeTo(int sid, MgShapes* dest);

    //! 将所有图形复制到另一个图形列表
    void copyShapesTo(MgShapes* dest) const;
    
    //! 移动图形到最后，以便显示在最顶部
    bool bringToFront(int sid);
    
    //! 移动图形到开始处，以便显示在最下面
    bool bringToBack(int sid);
    
    //! 移动图形到指定序号位置
    bool bringToIndex(int sid, int index);
    
    //! 得到上一级图形对象，或NULL
    static const MgShape* getParentShape(const MgShape* shape);

    //! 返回拥有者对象
    MgObject* getOwner() const;
    
    //! 返回图层序号
    int getIndex() const;
    
    static MgShapes* fromHandle(long h) { MgShapes* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() const { long h; *(const MgShapes**)&h = this; return h; }   //!< 得到句柄，用于跨库转换
    
public:
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release();
    virtual void addRef();
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
    MgShapeIterator(const MgShapes* s) : _s(s), _it((void*)0), _sp(MgShape::Null()) {}
    ~MgShapeIterator() { if (_it && _s) _s->freeIterator(_it); }
    
    //! 检查是否还有图形可遍历
    bool hasNext() {
        if (!_it && _s) {
            _sp = _s->getFirstShape(_it);
        }
        return !!_sp;
    }
    
    //! 得到当前遍历位置的图形
    /*! 可使用 while (const MgShape* sp = it.getNext()) {...} 遍历。
     */
    const MgShape* getNext() {
        if (!_it && _s) {
            _sp = _s->getFirstShape(_it);
        }
        const MgShape* sp = _sp;
        if (_sp && _s) {
            _sp = _s->getNextShape(_it);
        }
        return sp;
    }
    
    //! 返回图形列表
    const MgShapes* shapes() const { return _s; }

private:
    MgShapeIterator();
    const MgShapes* _s;
    void* _it;
    const MgShape* _sp;
};

#endif // TOUCHVG_MGSHAPES_H_
