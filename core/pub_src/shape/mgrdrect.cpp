// mgrdrect.cpp: 实现圆角矩形类 MgRoundRect
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>

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

bool MgRoundRect::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    bool ret = false;

    if (isOrtho())
    {
        ret = gs.drawRoundRect(&ctx, Box2d(_points[0], _points[2]), _rx, _ry);
    }
    else
    {
        GiSaveModelTransform xf(&gs.xf(), Matrix2d::rotation(getAngle(), getCenter()));
        ret = gs.drawRoundRect(&ctx, getRect(), _rx, _ry);
    }

    return __super::_draw(mode, gs, ctx, segment) || ret;
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
