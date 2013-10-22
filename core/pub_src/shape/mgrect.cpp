// mgrect.cpp: 实现矩形图形类 MgRect
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>

// MgBaseRect
//

MgBaseRect::MgBaseRect()
{
}

int MgBaseRect::_getPointCount() const
{
    return 4;
}

Point2d MgBaseRect::_getPoint(int index) const
{
    return _points[index];
}

void MgBaseRect::_setPoint(int index, const Point2d& pt)
{
    _points[index] = pt;
}

void MgBaseRect::_copy(const MgBaseRect& src)
{
    for (int i = 0; i < 4; i++)
        _points[i] = src._points[i];
    __super::_copy(src);
}

bool MgBaseRect::_equals(const MgBaseRect& src) const
{
    for (int i = 0; i < 4; i++) {
        if (_points[i] != src._points[i])
            return false;
    }
    return __super::_equals(src);
}

bool MgBaseRect::_isKindOf(int type) const
{
    return type == Type() || __super::_isKindOf(type);
}

void MgBaseRect::_update()
{
    float yoff = _points[2].distanceTo(_points[1]);
    _points[2] = _points[1].rulerPoint(_points[0], yoff);
    _points[3] = _points[0].rulerPoint(_points[1], -yoff);
    _extent.set(4, _points);
    if (_extent.isEmpty())
        _extent.set(_points[0], 2 * Tol::gTol().equalPoint(), 0);
    __super::_update();
}

void MgBaseRect::_transform(const Matrix2d& mat)
{
    for (int i = 0; i < 4; i++)
        _points[i] *= mat;
    Box2d rect(getRect());
    setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), rect.center());
    __super::_transform(mat);
}

void MgBaseRect::_clear()
{
    Point2d center = getCenter();
    for (int i = 0; i < 4; i++)
        _points[1] = center;
    __super::_clear();
}

Point2d MgBaseRect::getCenter() const
{
    return (_points[0] + _points[2]) * 0.5;
}

Box2d MgBaseRect::getRect() const
{
    return Box2d(getCenter(), getWidth(), getHeight());
}

float MgBaseRect::getWidth() const
{
    return _points[0].distanceTo(_points[1]);
}

float MgBaseRect::getHeight() const
{
    return _points[1].distanceTo(_points[2]);
}

float MgBaseRect::getAngle() const
{
    return (_points[1] - _points[0]).angle2();
}

bool MgBaseRect::isEmpty(float minDist) const
{
    return _points[2].distanceTo(_points[0]) < mgMax(minDist, _MGZERO);
}

bool MgBaseRect::isOrtho() const
{
    return mgEquals(_points[1].y, _points[0].y);
}

void MgBaseRect::setRect2P(const Point2d& pt1, const Point2d& pt2)
{
    setRectWithAngle(pt1, pt2, 0, pt1);
}

void MgBaseRect::setRectWithAngle(const Point2d& pt1, const Point2d& pt2,
                                  float angle, const Point2d& basept)
{
    Box2d rect(pt1, pt2);
    
    if (getFlag(kMgSquare)) {
        if (basept == pt1 && isCurve()) {
            rect.set(basept, 2 * basept.distanceTo(pt2), 0);
        }
        else {
            float len = mgMax(fabsf(pt2.x - pt1.x), fabsf(pt2.y - pt1.y));
            if (basept == pt1 && !isCurve()) {
                rect.set(pt1, pt1 + Point2d(pt2.x > pt1.x ? len : -len,
                                            pt2.y > pt1.y ? len : -len));
            } else {
                rect.set(basept, basept == pt1 ? 2 * len : len, 0);
            }
        }
    }
    
    _points[0] = rect.leftTop();
    _points[1] = rect.rightTop();
    _points[2] = rect.rightBottom();
    _points[3] = rect.leftBottom();

    if (!mgIsZero(angle))
    {
        Matrix2d mat(Matrix2d::rotation(angle, basept));
        for (int i = 0; i < 4; i++)
            _points[i] *= mat;
    }
}

void MgBaseRect::setRect4P(const Point2d points[4])
{
    for (int i = 0; i < 4; i++)
        _points[i] = points[i];
}

void MgBaseRect::setCenter(const Point2d& pt)
{
    Point2d old = getCenter();
    for (int i = 0; i < 4; i++)
        _points[i].offset(pt.x - old.x, pt.y - old.y);
}

float MgBaseRect::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    return mgnear::linesHit(4, _points, true, pt, tol, 
        res.nearpt, res.segment, &res.inside);
}

bool MgBaseRect::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 3; i++) {
        if (Box2d(_points[i], _points[i + 1]).isIntersect(rect))
            return true;
    }
    
    return false;
}

int MgBaseRect::_getHandleCount() const
{
    return 8;
}

int MgBaseRect::_getHandleType(int index) const
{
    return index >= 4 && index < 8 ? kMgHandleMidPoint : __super::_getHandleType(index);
}

Point2d MgBaseRect::_getHandlePoint(int index) const
{
    Point2d pt;
    mgnear::getRectHandle(getRect(), index, pt);
    float a = getAngle();
    pt *= Matrix2d::rotation(a, getCenter());
    return pt;
}

bool MgBaseRect::_setHandlePoint(int index, const Point2d& pt, float)
{
    if (getFlag(kMgSquare)) {
        if (isCurve() && !isEmpty(_MGZERO)) {
            float olddist = _getHandlePoint(index).distanceTo(getCenter());
            transform(Matrix2d::scaling(pt.distanceTo(getCenter()) / olddist, getCenter()));
        }
        else {
            Matrix2d mat(Matrix2d::rotation(-getAngle(), getCenter()));
            Point2d pt2(pt * mat);
            Box2d rect(getRect());
            mgnear::moveRectHandle(rect, index, pt2);
            
            if (4 == index || 6 == index) {
                rect = Box2d(rect.center(), rect.height(), rect.height());
            }
            else {
                rect = Box2d(rect.center(), rect.width(), rect.width());
            }
            setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), 
                             rect.center() * mat.inverse());
        }
    }
    else {
        int index2 = index / 4 * 4 + (index % 4 + 2) % 4;
        Point2d corner(_getHandlePoint(index2));
        Matrix2d mat(Matrix2d::rotation(-getAngle(), corner));
        
        Box2d rect(_getHandlePoint(0) * mat, _getHandlePoint(2) * mat);
        Point2d pt2(pt * mat);
        
        mgnear::moveRectHandle(rect, index, pt2);
        setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), corner);
    }
    update();
    return true;
}

bool MgBaseRect::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloatArray("points", &(_points[0].x), 8);
    return ret;
}

bool MgBaseRect::_load(MgShapeFactory* factory, MgStorage* s)
{
    return __super::_load(factory, s) && s->readFloatArray("points", &(_points[0].x), 8) == 8;
}

// MgRect
//

MG_IMPLEMENT_CREATE(MgRect)

MgRect::MgRect()
{
}

MgRect::~MgRect()
{
}

bool MgRect::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    bool ret = gs.drawPolygon(&ctx, 4, _points);
    return __super::_draw(mode, gs, ctx, segment) || ret;
}

// MgImageShape
//

#include <string.h>
#include <mgshapes.h>

MG_IMPLEMENT_CREATE(MgImageShape)

MgImageShape::MgImageShape()
{
    _name[0] = 0;
}

MgImageShape::~MgImageShape()
{
}

void MgImageShape::setName(const char* name)
{
    int len = sizeof(_name) - 1;
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    strncpy_s(_name, sizeof(_name), name, len);
#else
    strncpy(_name, name, len);
#endif
    _name[len] = 0;
}

bool MgImageShape::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    Box2d rect(getRect() * gs.xf().modelToDisplay());
    Vector2d vec((_points[1] - _points[0]) * gs.xf().modelToWorld());
    
    GiContext tmpctx(ctx);
    tmpctx.setNoFillColor();
    
    bool ret = gs.drawPolygon(&tmpctx, 4, _points);
    ret = gs.rawImage(_name, rect.center().x, rect.center().y,
        rect.width(), rect.height(), vec.angle2()) || ret;
    return __super::_draw(mode, gs, ctx, segment) || ret;
}

void MgImageShape::_copy(const MgImageShape& src)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    strcpy_s(_name, sizeof(_name), src._name);
#else
    strcpy(_name, src._name);
#endif
    __super::_copy(src);
}

bool MgImageShape::_equals(const MgImageShape& src) const
{
    return strcmp(_name, src._name) == 0 && __super::_equals(src);
}

void MgImageShape::_clear()
{
    _name[0] = 0;
    __super::_clear();
}

bool MgImageShape::_save(MgStorage* s) const
{
    s->writeString("name", _name);
    return __super::_save(s);
}

bool MgImageShape::_load(MgShapeFactory* factory, MgStorage* s)
{
    int len = sizeof(_name) - 1;
    len = s->readString("name", _name, len);
    _name[len] = 0;
    
    return __super::_load(factory, s);
}

MgShape* MgImageShape::findShapeByImageID(MgShapes* shapes, const char* name)
{
    MgShapeIterator it(shapes);
    MgShape* ret = NULL;
    
    while (MgShape* sp = it.getNext()) {
        if (sp->shape()->isKindOf(MgImageShape::Type())) {
            MgImageShape *image = (MgImageShape*)sp->shape();
            if (strcmp(name, image->getName()) == 0) {
                ret = sp;
                break;
            }
        }
    }
    
    return ret;
}

// MgDiamond
//

MG_IMPLEMENT_CREATE(MgDiamond)

MgDiamond::MgDiamond()
{
}

MgDiamond::~MgDiamond()
{
}

int MgDiamond::_getHandleCount() const
{
    return 8;
}

Point2d MgDiamond::_getHandlePoint(int index) const
{
    if (index < 4) {
        return MgBaseRect::_getHandlePoint(4 + index);
    }
    return (MgBaseRect::_getHandlePoint(4 + index % 4)
            + MgBaseRect::_getHandlePoint(4 + (index + 1) % 4)) / 2;
}

int MgDiamond::_getHandleType(int index) const
{
    return index < 4 ? kMgHandleVertext : kMgHandleMidPoint;
}

bool MgDiamond::_isHandleFixed(int index) const
{
    return index >= 4;
}

bool MgDiamond::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    if (!getFlag(kMgFixedLength)) {
        return MgBaseRect::_setHandlePoint(4 + index % 4, pt, tol);
    }
    
    Point2d cen(getCenter());
    Point2d pnt, ptup, ptside;
    
    pnt = pt * Matrix2d::rotation(-getAngle(), cen);
    mgnear::getRectHandle(getRect(), 4 + (index + 2) % 4, ptup);
    mgnear::getRectHandle(getRect(), 4 + (index + 1) % 4, ptside);
    
    float len = ptup.distanceTo(ptside);
    float dy = index % 2 == 0 ? pnt.y - ptup.y : pnt.x - ptup.x;
    float ry = mgMin(len, fabsf(dy) / 2);
    float rx = sqrtf(len * len - ry * ry);
    Box2d rect(cen, rx * 2, ry * 2);
    setRectWithAngle(rect.leftTop(), rect.rightBottom(), getAngle(), cen);
    
    return true;
}

bool MgDiamond::_rotateHandlePoint(int, const Point2d&)
{
    return false;
}

void MgDiamond::_update()
{
    __super::_update();
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    _extent.set(4, pts);
    if (_extent.isEmpty())
        _extent.set(pts[0], 2 * Tol::gTol().equalPoint(), 0);
}

float MgDiamond::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    return mgnear::linesHit(4, pts, true, pt, tol, 
        res.nearpt, res.segment, &res.inside);
}

bool MgDiamond::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 3; i++) {
        if (Box2d(_getHandlePoint(i), _getHandlePoint(i + 1)).isIntersect(rect))
            return true;
    }
    
    return false;
}

bool MgDiamond::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    Point2d pts[] = { _getHandlePoint(0), _getHandlePoint(1),
        _getHandlePoint(2), _getHandlePoint(3) };
    bool ret = gs.drawPolygon(&ctx, 4, pts);
    return __super::_draw(mode, gs, ctx, segment) || ret;
}
