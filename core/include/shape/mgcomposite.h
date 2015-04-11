//! \file mgcomposite.h
//! \brief 定义复合图形基类 MgComposite
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_COMPOSITE_SHAPE_H_
#define TOUCHVG_COMPOSITE_SHAPE_H_

#include "mgshapes.h"
#ifndef SWIG
#include <string>
#endif

//! 复合图形基类
/*! \ingroup CORE_SHAPE
*/
class MgComposite : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgComposite, MgBaseShape)
public:
    //! 返回本对象的类型
    static int Type() { return 6; }

    //! 返回图形总数
    int getShapeCount() const;

    //! 返回子图形列表
    MgShapes* shapes() const { return _shapes; }
    
    //! 返回拥有者图形对象
    const MgShape* getOwnerShape() const { return _owner; }

    //! 返回是否可以单独移动一个子图形，在 offset() 中调用
    virtual bool canOffsetShapeAlone(MgShape* shape) { return !!shape; }
    
#ifndef SWIG
    virtual bool isCurve() const { return true; }
    virtual void setOwner(MgObject* owner);
#endif

protected:
    MgComposite();
    virtual ~MgComposite();

    void _copy(const MgComposite& src);
    bool _equals(const MgComposite& src) const;
    bool _isKindOf(int type) const;
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _isHandleFixed(int index) const;
    void _update();
    void _clearCachedData();
    void _transform(const Matrix2d& mat);
    void _clear();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _offset(const Vector2d& vec, int segment);
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;
    void _output(MgPath& path) const;

protected:
    MgShape*    _owner;
    MgShapes*   _shapes;
};

//! 成组图形类
/*! \ingroup CORE_SHAPE
*/
class MgGroup : public MgComposite
{
    MG_INHERIT_CREATE(MgGroup, MgComposite, 9)
public:
    //! 添加一个新图形或其他图形列表中的图形
    bool addShapeToGroup(const MgShape* shape);
    
    //! 返回插入点
    Point2d getInsertionPoint() const { return _getPoint(1); }
    void setInsertionPoint(const Point2d& pt) { _setPoint(1, pt); }
    bool hasInsertionPoint() const { return !_getPoint(1).isDegenerate(); }
    
    //! 返回子图形中心点
    Point2d getCenterPoint() const { return _getPoint(0); }
    
    //! 设置名称
    void setName(const char* name);
    
    //! 查找指定名称的成组图形
    static const MgShape* findGroup(const MgShapes* shapes, const char* name);

#ifndef SWIG
    virtual int getSubType() const { return hasInsertionPoint() ? 1 : 0; }
    const char* getName() const { return _name.c_str(); }
#endif
    
protected:
    void _copy(const MgGroup& src);
    bool _equals(const MgGroup& src) const;
    void _update();
    void _transform(const Matrix2d& mat);
    bool _offset(const Vector2d& vec, int segment);
    void _clear();
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _setHandlePoint(int index, const Point2d& pt, float tol);
    bool _isHandleFixed(int index) const;
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
private:
    Point2d     _insert;
    Box2d       _box;
    std::string _name;
};

#endif // TOUCHVG_COMPOSITE_SHAPE_H_
