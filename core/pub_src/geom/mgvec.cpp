// mgvec.cpp: 实现二维矢量类Vector2d
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgpnt.h"

// 移动直尺法，以本点向dir方向点沿线为直尺边，在垂直沿线方向偏移yoff
Point2d Point2d::rulerPoint(const Point2d& dir, float yoff) const
{
    float len = distanceTo(dir);
    if (len < _MGZERO)
        return Point2d(x, y + yoff);
    else
    {
        yoff /= len;
        return Point2d(x - (dir.y - y) * yoff, y + (dir.x - x) * yoff);
    }
}

// 移动直尺法，dir方向点，在沿线方向偏移xoff，垂直沿线方向偏移yoff
Point2d Point2d::rulerPoint(const Point2d& dir, float xoff, float yoff) const
{
    float len = distanceTo(dir);
    if (len < _MGZERO)
        return Point2d(x + xoff, y + yoff);
    else
    {
        float dcos = (dir.x - x) / len;
        float dsin = (dir.y - y) / len;
        return Point2d(x + xoff * dcos - yoff * dsin, 
            y + xoff * dsin + yoff * dcos);
    }
}

// 判断两个矢量是否平行
bool Vector2d::isParallelTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector());
}

// 判断两个矢量是否平行
bool Vector2d::isParallelTo(const Vector2d& vec, 
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector())
    {
        if (mgIsZero(cosfz))
            nonzero = false;
        ret = true;
    }
    return ret;
}

// 判断两个矢量是否同向
bool Vector2d::isCodirectionalTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    if (cosfz < -_MGZERO)          // 同向则cos(夹角)接近1.0
        return false;
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= cosfz * tol.equalVector());
}

// 判断两个矢量是否同向
bool Vector2d::isCodirectionalTo(const Vector2d& vec, 
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector())
    {
        if (fabsf(cosfz) < _MGZERO)
        {
            nonzero = false;
            ret = true;
        }
        else
            ret = (cosfz >= -_MGZERO);
    }
    return ret;
}

// 判断两个矢量是否反向
bool Vector2d::isOppositeTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    if (cosfz > -_MGZERO)          // 反向则cos(夹角)接近-1.0
        return false;
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= (-cosfz) * tol.equalVector());
}

// 判断两个矢量是否反向
bool Vector2d::isOppositeTo(const Vector2d& vec, 
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector())
    {
        if (fabsf(cosfz) < _MGZERO)
        {
            nonzero = false;
            ret = false;
        }
        else
            ret = cosfz < -_MGZERO;
    }
    return ret;
}

// 判断两个矢量是否垂直
bool Vector2d::isPerpendicularTo(const Vector2d& vec, const Tol& tol) const
{
    float sinfz = fabsf(crossProduct(vec));
    if (sinfz < _MGZERO)
        return false;
    float cosfz = fabsf(dotProduct(vec));
    return (cosfz <= sinfz * tol.equalVector());
}

// 判断两个矢量是否垂直
bool Vector2d::isPerpendicularTo(const Vector2d& vec, 
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    float sinfz = fabsf(crossProduct(vec));
    float cosfz = fabsf(dotProduct(vec));
    if (cosfz <= sinfz * tol.equalVector())
    {
        ret = (sinfz >= _MGZERO);
        nonzero = ret;
    }
    return ret;
}

// 求本矢量投影到矢量xAxis上的垂直距离
// 在xAxis的逆时针方向时返回正值，顺时针则返回负值
float Vector2d::distanceToVector(const Vector2d& xAxis) const
{
    float len = xAxis.length();
    if (len < _MGZERO)
        return length();
    return xAxis.crossProduct(*this) / len;
}

// 求本矢量在矢量xAxis上的投影比例, 投影矢量 = xAxis * 投影比例
float Vector2d::projectScaleToVector(const Vector2d& xAxis) const
{
    float d2 = xAxis.lengthSqrd();
    if (d2 < _MGZERO)
        return 0.f;
    return dotProduct(xAxis) / d2;
}

// 求本矢量在矢量xAxis上的投影矢量和垂直矢量
float Vector2d::projectResolveVector(const Vector2d& xAxis, 
                                      Vector2d& proj, Vector2d& perp) const
{
    float s = projectScaleToVector(xAxis);
    proj = xAxis * s;
    perp = *this - proj;
    return s;
}

// 将本矢量在两个不共线的非零矢量上进行矢量分解, vec = u*uAxis+v*vAxis
bool Vector2d::resolveVector(const Vector2d& uAxis, const Vector2d& vAxis, 
                             Vector2d& uv) const
{
    float denom = uAxis.crossProduct(vAxis);
    if (mgIsZero(denom)) {
        uv.x = 0.f; uv.y = 0.f;
        return false;
    }
    float c = uAxis.crossProduct(*this);
    uv.x = crossProduct(vAxis) / denom;
    uv.y = c / denom;
    return true;
}

bool Vector2d::resolveVector(const Vector2d& uAxis, const Vector2d& vAxis)
{
    return resolveVector(uAxis, vAxis, *this);
}
