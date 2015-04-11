// mgarccross.cpp
// Copyright (c) 2004-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgarc.h"
#include "mgellipse.h"
#include "mgline.h"
#include "mglines.h"

int MgEllipse::crossCircle(Point2d& pt1, Point2d& pt2, const MgBaseShape* sp1,
                           const MgBaseShape* sp2, const Point2d& hitpt)
{
    const bool c1 = isCircle(sp1);
    const bool c2 = isCircle(sp2);
    const bool a1 = sp1->isKindOf(MgArc::Type());
    const bool a2 = sp2->isKindOf(MgArc::Type());
    Point2d cen1, cen2;
    float r1 = 0, r2 = 0;
    int n = -1;

    if (c1) {
        cen1 = ((MgEllipse*)sp1)->getCenter();
        r1 = ((MgEllipse*)sp1)->getRadiusX();
    }
    if (c2) {
        cen2 = ((MgEllipse*)sp2)->getCenter();
        r2 = ((MgEllipse*)sp2)->getRadiusX();
    }
    if (a1) {
        cen1 = ((MgArc*)sp1)->getCenter();
        r1 = ((MgArc*)sp1)->getRadius();
    }
    if (a2) {
        cen2 = ((MgArc*)sp2)->getCenter();
        r2 = ((MgArc*)sp2)->getRadius();
    }

    const bool ca1 = c1 || a1;
    const bool ca2 = c2 || a2;
    const MgBaseShape* line = ca1 ? sp2 : sp1;

    if (ca1 && ca2) {
        n = mgcurv::crossTwoCircles(pt1, pt2, cen1, r1, cen2, r2);
    } else if ((ca1 || ca2) && line->isKindOf(MgLine::Type())) {
        n = mgcurv::crossLineCircle(pt1, pt2, line->getPoint(0), line->getPoint(1),
            ca1 ? cen1 : cen2, ca1 ? r1 : r2, line->getSubType() != 2);
    } else if ((ca1 || ca2) && line->isKindOf(MgLines::Type())) {
        int edges = line->getPointCount() - (line->isClosed() ? 0 : 1);
        Point2d pt1r, pt2r;
        float dist = _FLT_MAX;
        int n2;

        for (int i = 0; i < edges; i++) {
            n2 = mgcurv::crossLineCircle(pt1r, pt2r, line->getHandlePoint(i),
                line->getHandlePoint((i + 1) % line->getPointCount()),
                ca1 ? cen1 : cen2, ca1 ? r1 : r2, line->getSubType() != 2);
            if (n2 > 0) {
                Point2d pt(pt2r.distanceTo(hitpt) < pt1r.distanceTo(hitpt) ? pt2r : pt1r);
                float d = hitpt.distanceTo(pt);

                if (dist > d) {
                    dist = d;
                    n = n2;
                    pt1 = pt1r;
                    pt2 = pt2r;
                }
            }
        }
    }

    return n;
}

int MgEllipse::crossCircle(Point2d& pt1, Point2d& pt2, const MgBaseShape* sp)
{
    int n = -1;

    if (isCircle(sp) && pt1 != pt2) {
        n = mgcurv::crossLineCircle(pt1, pt2, pt1, pt2,
            ((MgEllipse*)sp)->getCenter(),
            ((MgEllipse*)sp)->getRadiusX());
    }
    else if (sp->isKindOf(MgArc::Type()) && pt1 != pt2) {
        n = mgcurv::crossLineCircle(pt1, pt2, pt1, pt2,
            ((MgArc*)sp)->getCenter(),
            ((MgArc*)sp)->getRadius());
    }
    return n;
}
