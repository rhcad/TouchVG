// mgsplines.cpp: 实现三次参数样条曲线类 MgSplines
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbasicsp.h"
#include <mgshape_.h>

MG_IMPLEMENT_CREATE(MgSplines)

MgSplines::MgSplines()
{
}

MgSplines::~MgSplines()
{
}

float MgSplines::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    return __super::_hitTest(pt, tol, res);
    //return mgnear::cubicSplinesHit(_count, _points, _knotvs, isClosed(),
    //    pt, tol, res.nearpt, res.segment);
}

bool MgSplines::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    return true;//mgnear::cubicSplinesIntersectBox(rect, _count, _points, _knotvs, isClosed());
}

bool MgSplines::_draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const
{
    bool ret = (_count == 2 ? gs.drawLine(&ctx, _points[0], _points[1])
                : gs.drawQuadSplines(&ctx, _count, _points));
    return __super::_draw(mode, gs, ctx, segment) || ret;
}
