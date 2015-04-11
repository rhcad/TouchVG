// mgvec.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgvec.h"

bool Vector2d::isParallelTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector());
}

bool Vector2d::isParallelTo(const Vector2d& vec,
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector()) {
        if (mgIsZero(cosfz))
            nonzero = false;
        ret = true;
    }
    return ret;
}

bool Vector2d::isCodirectionalTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    if (cosfz < -_MGZERO)
        return false;
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= cosfz * tol.equalVector());
}

bool Vector2d::isCodirectionalTo(const Vector2d& vec,
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector()) {
        if (fabsf(cosfz) < _MGZERO) {
            nonzero = false;
            ret = true;
        }
        else {
            ret = (cosfz >= -_MGZERO);
        }
    }
    return ret;
}

bool Vector2d::isOppositeTo(const Vector2d& vec, const Tol& tol) const
{
    float cosfz = dotProduct(vec);
    if (cosfz > -_MGZERO)
        return false;
    float sinfz = crossProduct(vec);
    return (fabsf(sinfz) <= (-cosfz) * tol.equalVector());
}

bool Vector2d::isOppositeTo(const Vector2d& vec,
                            const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    
    float cosfz = dotProduct(vec);
    float sinfz = crossProduct(vec);
    
    if (fabsf(sinfz) <= fabsf(cosfz) * tol.equalVector()) {
        if (fabsf(cosfz) < _MGZERO) {
            nonzero = false;
            ret = false;
        }
        else {
            ret = cosfz < -_MGZERO;
        }
    }
    return ret;
}

bool Vector2d::isPerpendicularTo(const Vector2d& vec, const Tol& tol) const
{
    float sinfz = fabsf(crossProduct(vec));
    if (sinfz < _MGZERO)
        return false;
    float cosfz = fabsf(dotProduct(vec));
    return (cosfz <= sinfz * tol.equalVector());
}

bool Vector2d::isPerpendicularTo(const Vector2d& vec,
                                 const Tol& tol, bool& nonzero) const
{
    bool ret = false;
    nonzero = true;
    
    float sinfz = fabsf(crossProduct(vec));
    float cosfz = fabsf(dotProduct(vec));
    
    if (cosfz <= sinfz * tol.equalVector()) {
        ret = (sinfz >= _MGZERO);
        nonzero = ret;
    }
    return ret;
}

float Vector2d::distanceToVector(const Vector2d& xAxis) const
{
    float len = xAxis.length();
    if (len < _MGZERO)
        return length();
    return xAxis.crossProduct(*this) / len;
}

float Vector2d::projectScaleToVector(const Vector2d& xAxis) const
{
    float d2 = xAxis.lengthSquare();
    if (d2 < _MGZERO)
        return 0.f;
    return dotProduct(xAxis) / d2;
}

float Vector2d::projectResolveVector(const Vector2d& xAxis,
                                     Vector2d& proj, Vector2d& perp) const
{
    float s = projectScaleToVector(xAxis);
    proj = xAxis * s;
    perp = *this - proj;
    return s;
}

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
