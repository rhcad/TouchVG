// mgrdrect.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgrdrect.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgRoundRect)

MgRoundRect::MgRoundRect() : _rx(0), _ry(0)
{
}

MgRoundRect::~MgRoundRect()
{
}

void MgRoundRect::setRadius(float rx, float ry)
{
    _rx = fabsf(rx);
    _ry = fabsf(ry);
    if (_ry < _MGZERO)
        _ry = _rx;
}

bool MgRoundRect::isCurve() const
{
    return fabsf(_rx) > getWidth() / 6 || fabsf(_ry) > getHeight() / 6;
}

void MgRoundRect::_copy(const MgRoundRect& src)
{
    _rx = src._rx;
    _ry = src._ry;
    __super::_copy(src);
}

bool MgRoundRect::_equals(const MgRoundRect& src) const
{
    return mgEquals(_rx, src._rx)
        && mgEquals(_ry, src._ry)
        && __super::_equals(src);
}

void MgRoundRect::_clear()
{
    _rx = _ry = 0.0;
    __super::_clear();
}

float MgRoundRect::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    float dist;

    if (isOrtho())
    {
        dist = mgnear::roundRectHit(Box2d(_points[0], _points[2]),
            _rx, _ry, pt, tol, res.nearpt, res.segment);
    }
    else
    {
        Matrix2d mat(Matrix2d::rotation(getAngle(), getCenter()));
        Box2d rect(Box2d(pt, 2 * tol, 2 * tol) * mat.inverse());

        dist = mgnear::roundRectHit(getRect(), _rx, _ry, 
            rect.center(), rect.width(), res.nearpt, res.segment);
        if (dist < 1e10f)
            res.nearpt *= mat;
    }

    return dist;
}

void MgRoundRect::_output(MgPath& path) const
{
    MgPath tmppath;
    
    tmppath.genericRoundLines(4, _points, _rx, true);
    path.append(tmppath);
}

bool MgRoundRect::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloat("rx", _rx);
    s->writeFloat("ry", _ry);
    return ret;
}

bool MgRoundRect::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    _rx = s->readFloat("rx", 0);
    _ry = s->readFloat("ry", 0);
    return ret;
}
