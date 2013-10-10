// mgbase.cpp: 实现基本计算功能函数
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#include "mgbase.h"

// 将数转换到数值范围[tmin, tmax)内
float mgbase::toRange(float value, float tmin, float tmax)
{
    while (value < tmin)
        value += tmax - tmin;
    while (value >= tmax)
        value -= tmax - tmin;
    return value;
}

// 使角度在[0, 2PI)之间
float mgbase::to0_2PI(float angle)
{
    return mgbase::toRange(angle, 0.f, _M_2PI);
}

// 使角度在[-PI, PI)之间
float mgbase::toPI(float angle)
{
    return mgbase::toRange(angle, -_M_PI, _M_PI);
}

// 角度从度转换到弧度
float mgbase::deg2Rad(float deg)
{
    return deg * _M_D2R;
}

// 角度从弧度转换到度
float mgbase::rad2Deg(float rad)
{
    return rad * _M_R2D;
}

// 度分秒转换到角度(度)
float mgbase::dms2Deg(float angle)
{
    int nDeg, nMin;
    int sign = 1;
    
    if (angle < 0)
    {
        sign = -1;
        angle = -angle;
    }
    else if (angle == 0)
        return 0.f;
    
    nDeg = static_cast<int>(angle);
    angle -= nDeg;
    angle += 1e-13f;
    angle -= 7e-14f;
    angle *= 100.f;
    nMin = static_cast<int>(angle);
    angle -= nMin;
    angle *= 100.f;
    
    return sign * (nDeg + nMin / 60.f + angle / 3600.f);
}

// 角度(度)转换到度分秒
float mgbase::deg2Dms(float angle)
{
    int nDeg, nMin;
    float dSecond;
    int sign = 1;
    
    if (angle < 0)
    {
        sign = -1;
        angle = -angle;
    }
    else if (angle == 0)
        return 0.f;
    
    nDeg = static_cast<int>(angle);
    angle -= nDeg;
    nMin = static_cast<int>(angle * 60.f);
    angle -= nMin / 60.f;
    if (nMin == 60)
    {
        nDeg++;
        nMin = 0;
    }
    dSecond = angle * 3600.f;
    if (dSecond >= 60.f)
    {
        dSecond = 0.f;
        nMin++;
        if (nMin == 60)
        {
            nDeg++;
            nMin = 0;
        }
    }
    return sign * (nDeg + nMin / 100.f + dSecond / 10000.f);
}

// 求两个角度的角平分线角度, [0, 2PI)
float mgbase::getMidAngle(float fromAngle, float toAngle)
{
    fromAngle = mgbase::to0_2PI(fromAngle);
    toAngle = mgbase::to0_2PI(toAngle);
    if (!mgEquals(fromAngle, toAngle))
    {
        if (toAngle < fromAngle)
            return mgbase::to0_2PI((fromAngle + toAngle + _M_2PI) / 2);
    }
    return (fromAngle + toAngle) / 2;
}

// 求两个角度的夹角, [-PI, PI)
float mgbase::getDiffAngle(float fromAngle, float toAngle)
{
    fromAngle = mgbase::to0_2PI(fromAngle);
    toAngle = mgbase::to0_2PI(toAngle);
    if (mgEquals(fromAngle, toAngle))
        return 0.f;
    if (toAngle < fromAngle)
        toAngle += _M_2PI;
    return mgbase::toPI(toAngle - fromAngle);
}

// 计算最大公约数
int mgbase::getGcd(int x, int y)
{
    while (x != y)
    {
        if (x > y) x -= y;
        else y -= x;
    }
    return x;
}

// 四舍五入. 小数位[-6，7]. eg: mgbase::roundReal(1.25, 1)=1.3
float mgbase::roundReal(float value, int decimal)
{
    if (decimal < -6) decimal = -6;
    else if (decimal > 7) decimal = 7;
    float e10n = powf(10.f, (float)decimal);
    float l = floorf(e10n * value + 0.5f);
    return static_cast<float>(l / e10n);
}
