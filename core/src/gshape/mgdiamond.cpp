// mgdiamond.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgdiamond.h"
#include "mgshape_.h"

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
    return index < 4 ? kMgHandleVertex : kMgHandleMidPoint;
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
    return linesHit(4, pts, true, pt, tol, res);
}

bool MgDiamond::_hitTestBox(const Box2d& rect) const
{
    if (!MgBaseShape::_hitTestBox(rect))
        return false;
    
    for (int i = 0; i < 4; i++) {
        if (Box2d(_getHandlePoint(i), _getHandlePoint((i + 1) % 4)).isIntersect(rect))
            return true;
    }
    
    return false;
}

void MgDiamond::_output(MgPath& path) const
{
    path.moveTo(_getHandlePoint(0));
    path.lineTo(_getHandlePoint(1));
    path.lineTo(_getHandlePoint(2));
    path.lineTo(_getHandlePoint(3));
    path.closeFigure();
}
