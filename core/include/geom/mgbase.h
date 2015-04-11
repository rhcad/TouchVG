//! \file mgbase.h
//! \brief 定义基本计算功能函数
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_BASIC_H_
#define TOUCHVG_BASIC_H_

#include "mgdef.h"

//! 基本计算功能函数
/*! \ingroup GEOMAPI
*/
struct mgbase {

//! 将数转换到数值范围[tmin, tmax)内
/*! 转换后的数和原来的数相差(tmax - tmin)的整数倍。
    \param[in] value 给定的任意数
    \param[in] tmin 数值范围的下限
    \param[in] tmax 数值范围的上限，必须大于tmin
    \return 转换后的数，大于等于tmin且小于tmax
*/
static float toRange(float value, float tmin, float tmax);

//! 使角度在[0, 2PI)之间
/*! 转换后的角度和原来的角度相差2PI的整数倍
    \param[in] angle 角度，单位为弧度
    \return 角度，弧度，范围在[0，2PI)之间
    \see toRange, toPI, _M_2PI
*/
static float to0_2PI(float angle);

//! 使角度在[-PI, PI)之间
/*! 转换后的角度和原来的角度相差2PI的整数倍
    \param[in] angle 角度，单位为弧度
    \return 角度，弧度，范围在[-PI, PI)之间
    \see toRange, to0_2PI, _M_PI
*/
static float toPI(float angle);

//! 角度从度转换到弧度
/*! 内部是乘以一个常数进行转换
    \param[in] deg 以度为单位的角度
    \return 以弧度为单位的角度
    \see rad2Deg, dms2Deg, _M_D2R
*/
static float deg2Rad(float deg);

//! 角度从弧度转换到度
/*! 内部是乘以一个常数进行转换
    \param[in] rad 以弧度为单位的角度
    \return 以度为单位的角度
    \see deg2Rad, dms2Deg, _M_R2D
*/
static float rad2Deg(float rad);

//! 度分秒转换到角度(度)
/*! 度分秒形式的角度的整数部分为度，十分位和百分位为分，
    千分位和万分位及其他小数位为秒。例如32.251242表示32°25'12.42"。
    \param[in] angle 度分秒形式的角度
    \return 以度为单位的角度
    \see deg2Dms, deg2Rad
*/
static float dms2Deg(float angle);

//! 角度(度)转换到度分秒
/*! 度分秒形式的角度的整数部分为度，十分位和百分位为分，
    千分位和万分位及其他小数位为秒。例如32.251242表示32°25'12.42"。
    \param[in] angle 以度为单位的角度
    \return 度分秒形式的角度
    \see dms2Deg, deg2Rad
*/
static float deg2Dms(float angle);

//! 求两个角度的角平分线角度, [0, 2PI)
/*! 函数计算从起始角度到终止角度按逆时针方向所转过角度的角平分线角度。
    如果终止角度小于起始角度，则终止角度自动加上2PI以便按逆时针方向能转到终止角度
    \param[in] fromAngle 起始角度，弧度
    \param[in] toAngle 终止角度，弧度
    \return 角平分线角度，弧度，范围在[0，2PI)之间
*/
static float getMidAngle(float fromAngle, float toAngle);

//! 求两个角度(不区分起始角度和终止角度)的角平分线角度, [0, 2PI)
static float getMidAngle2(float angle1, float angle2);

//! 求两个角度的夹角, [-PI, PI)
/*! 函数计算从起始角度到终止角度按逆时针方向所转过角度，并转换到[-PI，PI)范围
    \param[in] fromAngle 起始角度，弧度
    \param[in] toAngle 终止角度，弧度
    \return 夹角，弧度，范围在[-PI，PI)之间
*/
static float getDiffAngle(float fromAngle, float toAngle);

//! 计算最大公约数
/*! 最大公约数就是能被给定的两个正整数整除、且不大于这两个数的正整数
    \param[in] x 第一个正整数
    \param[in] y 第二个正整数
    \return 最大公约数，正整数
*/
static int getGcd(int x, int y);

//! 四舍五入为浮点数
/*! 例如 roundReal(1.25, 1)=1.3
    \param[in] value 要四舍五入的数
    \param[in] decimal 要保留的小数位数，范围为-6到7，正数表示有小数位，
        0相当于取整，负数表示取整并使某些低位数字为零
    \return 四舍五入后的浮点数
    \see round
*/
static float roundReal(float value, int decimal);
};

#endif // TOUCHVG_BASIC_H_
