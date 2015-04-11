// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGDBLPT_H
#define TOUCHVG_MGDBLPT_H

#include <math.h>

//! Double typed point
struct point_t {
    double x, y;

    point_t() : x(0), y(0) {}
    point_t(double x, double y) : x(x), y(y) {}
    point_t(const point_t& p) : x(p.x), y(p.y) {}
    point_t& operator=(const point_t& p) { x = p.x; y = p.y; return *this; }
    
    double length() const {
        return sqrt(x*x + y*y);
    }
    double lengthSquare() const {
        return x*x + y*y;
    }
    double distanceSquare(const point_t& v) const {
        return (x - v.x) * (x - v.x) + (y - v.y) * (y - v.y);
    }
    double distanceTo(const point_t& v) const {
        return sqrt(distanceSquare(v));
    }
    double dotProduct(const point_t& v) const {
        return (x * v.x + y * v.y);
    }
    double crossProduct(const point_t& v) const {
        return (x * v.y - y * v.x);
    }
    bool isDegenerate() const {
        return isnan(x) || isnan(y);
    }
    point_t normalized() const {
        double oldlen = length();
        return oldlen != 0.0 ? point_t(x / oldlen, y / oldlen) : *this;
    }
    void normalize() {
        double oldlen = length();
        
        if (oldlen != 0.0) {
            x /= oldlen;
            y /= oldlen;
        }
    }
    point_t scaledVector(double newLength) const {
        double oldlen = length();
        double scale = 1.0;
        
        if (oldlen != 0.0) {
            scale = newLength / oldlen;
        }
        return point_t(x * scale, y * scale);
    }

    point_t operator-(const point_t& v) const {
        return point_t(x - v.x, y - v.y);
    }
    point_t operator+(const point_t& v) const {
        return point_t(x + v.x, y + v.y);
    }
    point_t operator*(double d) const {
        return point_t(x * d, y * d);
    }
    friend point_t operator*(double d, const point_t& v) {
        return point_t(v.x * d, v.y * d);
    }
    point_t operator/(double d) const {
        return point_t(x / d, y / d);
    }
    friend point_t operator/(double d, const point_t& v) {
        return point_t(v.x / d, v.y / d);
    }
};

#endif // TOUCHVG_MGDBLPT_H
