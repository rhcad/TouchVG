// mgrect.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgrect.h"
#include "mgshape_.h"

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
    return (index < 0 ? Point2d() : _points[index < 4 ? index : index % 4]);
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
    mat.transformPoints(4, _points);
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

float MgBaseRect::getDiagonalLength() const
{
    return mgHypot(getWidth(), getHeight());
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

    if (!mgIsZero(angle)) {
        Matrix2d mat(Matrix2d::rotation(angle, basept));
        mat.transformPoints(4, _points);
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
    return linesHit(4, _points, true, pt, tol, res);
}

bool MgBaseRect::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 4; i++) {
        if (Box2d(_points[i], _points[(i + 1) % 4]).isIntersect(rect))
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

static inline bool isBeside(int i1, int i2)
{
    return ((0 == i1 && 1 == i2) || (1 == i1 && 2 == i2) || (2 == i1 && 3 == i2) || (0 == i1 && 3 == i2));
}

bool MgBaseRect::transformWith2P(const Point2d& pt1, int i1, const Point2d& pt2, int i2)
{
    bool ret = (isBeside(mgMin(i1, i2), mgMax(i1, i2)) && pt1 != pt2);
    if (ret) {
        transform(Matrix2d::transformWith2P(_points[i1], _points[i2], pt1, pt2));
    }
    
    return ret;
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

void MgBaseRect::_output(MgPath& path) const
{
    path.moveTo(_points[0]);
    path.linesTo(3, _points + 1);
    path.closeFigure();
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
