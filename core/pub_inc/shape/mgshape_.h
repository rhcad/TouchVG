//! \file mgshape_.h
//! \brief 定义图形类实现用的辅助宏
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSHAPEIMPL_H_
#define TOUCHVG_MGSHAPEIMPL_H_

#include "mgstorage.h"

#define MG_IMPLEMENT_CREATE(Cls)                                \
    Cls* Cls::create() { return new Cls(); }                    \
    MgObject* Cls::clone() const                                \
        { Cls* p = create(); p->_copy(*this); return p; }       \
    void Cls::copy(const MgObject& src) {                       \
        if (src.isKindOf(Type())) _copy((const Cls&)src);       \
        else if (src.isKindOf(__super::Type()))                 \
            __super::_copy((const Cls&)src);                    \
    }                                                           \
    void Cls::release() { delete this; }                        \
    bool Cls::equals(const MgObject& src) const                 \
        { return src.isKindOf(Type()) && _equals((const Cls&)src); } \
    bool Cls::_isKindOf(int type) const                         \
        { return type == Type() || __super::_isKindOf(type); }  \
    Box2d Cls::getExtent() const { return _getExtent(); }       \
    void Cls::update() { _update(); }                           \
    void Cls::transform(const Matrix2d& mat) { _transform(mat); } \
    void Cls::clear() { _clear(); }                             \
    void Cls::clearCachedData() { _clearCachedData(); }         \
    int Cls::getPointCount() const { return _getPointCount(); } \
    Point2d Cls::getPoint(int index) const { return _getPoint(index); }  \
    void Cls::setPoint(int index, const Point2d& pt) { _setPoint(index, pt); }  \
    bool Cls::isClosed() const { return _isClosed(); }          \
    float Cls::hitTest(const Point2d& pt, float tol, MgHitResult& res) const \
        { return _hitTest(pt, tol, res); }                      \
    bool Cls::hitTestBox(const Box2d& rect) const               \
        { return _hitTestBox(rect); }                           \
    bool Cls::draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const  \
        { return _draw(mode, gs, ctx, segment); }               \
    bool Cls::save(MgStorage* s) const { return _save(s); }     \
    bool Cls::load(MgShapeFactory* factory, MgStorage* s) {     \
        return _load(factory, s); }                             \
    int Cls::getHandleCount() const { return _getHandleCount(); }    \
    Point2d Cls::getHandlePoint(int index) const                \
        { return _getHandlePoint(index); }                      \
    bool Cls::setHandlePoint2(int index, const Point2d& pt, float tol, int& data)   \
        { return _setHandlePoint2(index, pt, tol, data); }      \
    bool Cls::setHandlePoint(int index, const Point2d& pt, float tol)   \
        { return _rotateHandlePoint(index, pt) || _setHandlePoint(index, pt, tol); } \
    bool Cls::isHandleFixed(int index) const { return _isHandleFixed(index); } \
    int Cls::getHandleType(int index) const { return _getHandleType(index); } \
    bool Cls::offset(const Vector2d& vec, int segment)          \
        { return _offset(vec, segment); }

#endif // TOUCHVG_MGSHAPEIMPL_H_
