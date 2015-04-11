//! \file mglines.h
//! \brief 定义折线类 MgBaseLines,MgLines
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#ifndef TOUCHVG_LINES_SHAPE_H_
#define TOUCHVG_LINES_SHAPE_H_

#include "mgbasesp.h"

//! 折线基类
/*! \ingroup CORE_SHAPE
 */
class MgBaseLines : public MgBaseShape
{
    MG_DECLARE_DYNAMIC(MgBaseLines, MgBaseShape)
public:
    //! 返回本对象的类型
    static int Type() { return 5; }
    
    //! 设置是否闭合
    void setClosed(bool closed) { setFlag(kMgClosed, closed); }
    
    //! 返回终点
    Point2d endPoint() const;
    
    //! 改变顶点数
    virtual bool resize(int count);
    
    //! 添加一个顶点
    virtual bool addPoint(const Point2d& pt);
    
    //! 在指定段插入一个顶点
    virtual bool insertPoint(int segment, const Point2d& pt);
    
    //! 删除一个顶点
    virtual bool removePoint(int index);
    
    //! 返回边的最大序号
    int maxEdgeIndex() const;
    
    //! 增量路径比较
    bool isIncrementFrom(const MgBaseLines& src) const;

#ifndef SWIG
    virtual int getSubType() const { return isClosed() ? 1 : 0; }
    virtual const Point2d* getPoints() const { return _points; }
#endif
    
protected:
    MgBaseLines();
    virtual ~MgBaseLines();
    bool _isClosed() const;
    int _getPointCount() const;
    Point2d _getPoint(int index) const;
    void _setPoint(int index, const Point2d& pt);
    void _copy(const MgBaseLines& src);
    bool _equals(const MgBaseLines& src) const;
    bool _isKindOf(int type) const;
    void _update();
    void _transform(const Matrix2d& mat);
    void _clear();
    float _hitTest(const Point2d& pt, float tol, MgHitResult& res) const;
    bool _hitTestBox(const Box2d& rect) const;
    bool _save(MgStorage* s) const;
    bool _load(MgShapeFactory* factory, MgStorage* s);
    
protected:
    Point2d*    _points;
    int      _maxCount;
    int      _count;
};

//! 折线图形类
/*! \ingroup CORE_SHAPE
 */
class MgLines : public MgBaseLines
{
    MG_INHERIT_CREATE(MgLines, MgBaseLines, 15)
public:

protected:
    int _getHandleCount() const;
    Point2d _getHandlePoint(int index) const;
    int _getHandleType(int index) const;
    bool _isHandleFixed(int index) const;
    void _output(MgPath& path) const;
};

#endif // TOUCHVG_LINES_SHAPE_H_
