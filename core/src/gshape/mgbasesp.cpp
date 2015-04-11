// mgbasesp.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgbasesp.h"
#include "mgshape_.h"

MgBaseShape::MgBaseShape() : _flags(0), _changeCount(0) {
}
MgBaseShape::~MgBaseShape() {
}
void MgBaseShape::copy(const MgObject& src) {
    if (src.isKindOf(Type()))
        _copy((const MgBaseShape&)src);
}
bool MgBaseShape::equals(const MgObject& src) const {
    return src.isKindOf(Type()) && _equals((const MgBaseShape&)src);
}
bool MgBaseShape::isKindOf(int type) const {
    return type == Type();
}
Box2d MgBaseShape::getExtent() const { return _getExtent(); }
long MgBaseShape::getChangeCount() const { return _changeCount; }
void MgBaseShape::resetChangeCount(long count) { update(); _changeCount = count; }
void MgBaseShape::afterChanged() { _changeCount++; }
void MgBaseShape::update() { _update(); }
void MgBaseShape::transform(const Matrix2d& mat) { _transform(mat); }
void MgBaseShape::clear() { _clear(); }
void MgBaseShape::clearCachedData() { _clearCachedData(); }

bool MgBaseShape::isClosed() const {
    return _isClosed();
}
bool MgBaseShape::hitTestBox(const Box2d& rect) const {
    return _hitTestBox(rect);
}
bool MgBaseShape::save(MgStorage* s) const {
    return _save(s);
}
bool MgBaseShape::load(MgShapeFactory* factory, MgStorage* s) {
    return _load(factory, s);
}
int MgBaseShape::getHandleCount() const {
    return _getHandleCount();
}
Point2d MgBaseShape::getHandlePoint(int index) const {
    return _getHandlePoint(index);
}
bool MgBaseShape::setHandlePoint(int index, const Point2d& pt, float tol) {
    return _rotateHandlePoint(index, pt) || _setHandlePoint(index, pt, tol);
}
bool MgBaseShape::isHandleFixed(int index) const {
    return _isHandleFixed(index);
}
int MgBaseShape::getHandleType(int index) const {
    return _getHandleType(index);
}
bool MgBaseShape::offset(const Vector2d& vec, int segment) {
    return _offset(vec, segment);
}

void MgBaseShape::_copy(const MgBaseShape& src)
{
    _extent = src._extent;
    _flags = src._flags;
    _changeCount = src._changeCount;
}

bool MgBaseShape::_equals(const MgBaseShape& src) const
{
    return _flags == src._flags;
}

Box2d MgBaseShape::_getExtent() const
{
    if (_extent.isNull() || !_extent.isEmpty(minTol())) {
        return _extent;
    }
    
    Box2d rect(_extent);
    
    if (rect.width() < minTol().equalPoint() && getPointCount() > 0) {
        rect.inflate(minTol().equalPoint() / 2.f, 0);
    }
    if (rect.height() < minTol().equalPoint() && getPointCount() > 0) {
        rect.inflate(0, minTol().equalPoint() / 2.f);
    }
    
    return rect;
}

void MgBaseShape::_update()
{
    _extent = _getExtent();
    afterChanged();
}

void MgBaseShape::_transform(const Matrix2d& mat)
{
    _extent *= mat;
}

void MgBaseShape::_clear()
{
    _extent.empty();
}

int MgBaseShape::_getHandleCount() const
{
    return getPointCount();
}

Point2d MgBaseShape::_getHandlePoint(int index) const
{
    return getPoint(index);
}

bool MgBaseShape::_rotateHandlePoint(int index, const Point2d& pt)
{
    if (getFlag(kMgFixedLength)) {
        if (getFlag(kMgRotateDisnable)) {
            offset(pt - getHandlePoint(index), -1);
        }
        else {
            Point2d basept(_extent.center());
            if (!getFlag(kMgSquare)) {
                int baseindex = index > 0 ? index - 1 : getHandleCount() - 1;
                while (baseindex > 0 && isHandleFixed(baseindex))
                    baseindex--;
                basept = (getHandlePoint(baseindex));
            }
            float a1 = (pt - basept).angle2();
            float a2 = (getHandlePoint(index) - basept).angle2();
            
            transform(Matrix2d::rotation(a1 - a2, basept));
        }
        return true;
    }

    return false;
}

bool MgBaseShape::_setHandlePoint(int index, const Point2d& pt, float)
{
    setPoint(index, pt);
    return true;
}

bool MgBaseShape::_setHandlePoint2(int index, const Point2d& pt, float tol, int&)
{
    return setHandlePoint(index, pt, tol);
}

bool MgBaseShape::_offset(const Vector2d& vec, int)
{
    transform(Matrix2d::translation(vec));
    return true;
}

bool MgBaseShape::_hitTestBox(const Box2d& rect) const
{
    return getExtent().isIntersect(rect);
}

bool MgBaseShape::_save(MgStorage* s) const
{
    s->writeUInt("flags", _flags);
    return true;
}

bool MgBaseShape::_load(MgShapeFactory* factory, MgStorage* s)
{
    _flags = s->readInt("flags", _flags);
    return true;
}

float MgBaseShape::linesHit(int n, const Point2d* points, bool closed,
                            const Point2d& pt, float tol, MgHitResult& res)
{
    int flags = ( (res.snapVertexEnabled() ? (1 << mglnrel::kPtAtVertex) : 0)
                 |(res.snapEdgeEnabled() ? (1 << mglnrel::kPtOnEdge) : 0) );
    return mgnear::linesHit(n, points, closed, pt, tol, res.nearpt, res.segment,
                            &res.inside, (int*)0, flags, res.ignoreHandle);
}

bool MgBaseShape::getFlag(MgShapeBit bit) const
{
    return (_flags & (1 << bit)) != 0;
}

void MgBaseShape::setFlag(MgShapeBit bit, bool on)
{
    _flags = on ? _flags | (1 << bit) : _flags & ~(1 << bit);
}
