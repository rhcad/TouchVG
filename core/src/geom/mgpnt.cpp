// mgpnt.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgpnt.h"

#if !defined(NAN) && defined(_WIN32)
static const unsigned long __nan[2] = {0xffffffff, 0x7fffffff};
#define NAN (*(const float *) __nan)
#endif

const Point2d Point2d::kInvalid() { return Point2d(NAN, NAN); }

Point2d Point2d::rulerPoint(const Point2d& dir, float yoff) const
{
    float len = distanceTo(dir);
    if (len < _MGZERO) {
        return Point2d(x, y + yoff);
    }
    yoff /= len;
    return Point2d(x - (dir.y - y) * yoff, y + (dir.x - x) * yoff);
}

Point2d Point2d::rulerPoint(const Point2d& dir, float xoff, float yoff) const
{
    float len = distanceTo(dir);
    if (len < _MGZERO)
        return Point2d(x + xoff, y + yoff);
    float dcos = (dir.x - x) / len;
    float dsin = (dir.y - y) / len;
    return Point2d(x + xoff * dcos - yoff * dsin,
                   y + xoff * dsin + yoff * dcos);
}
