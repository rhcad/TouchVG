// mgdblpt.h: 定义双精度点和辅助函数
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGDBLPT_H
#define TOUCHVG_MGDBLPT_H

#include <math.h>

struct point_t {
    double x, y;

    double length() const {
        return sqrt(x*x + y*y);
    }
    double lengthSquare() const {
        return x*x + y*y;
    }
    double distanceSquare(const point_t& v) const {
        return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y);
    }
    double distance(const point_t& v) const {
        return sqrt(distanceSquare(v));
    }
    double dotProduct(const point_t& v) const {
        return (x * v.x + y * v.y);
    }
    double crossProduct(const point_t& v) const {
        return (x * v.y - y * v.x);
    }

    point_t operator-(const point_t& v) const
    {
        point_t ret = { x - v.x, y - v.y };
        return ret;
    }
    point_t operator+(const point_t& v) const
    {
        point_t ret = { x + v.x, y + v.y };
        return ret;
    }
    point_t operator*(double d) const
    {
        point_t ret = { x * d, y * d };
        return ret;
    }
    friend point_t operator*(double d, const point_t& v)
    {
        point_t ret = { v.x * d, v.y * d };
        return ret;
    }
};

#endif // TOUCHVG_MGDBLPT_H
