//! \file mgtol.h
//! \brief 定义容差类 Tol
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGTOL_H_
#define TOUCHVG_MGTOL_H_

#include "mgdef.h"

//! 容差类
/*!
    \ingroup GEOM_CLASS
    容差类含有长度容差和矢量容差。\n
    长度容差表示长度小于该值就认为是零长度，或两点距离小于该值就认为重合。\n
    矢量容差表示两个弧度角度小于该值就认为是相等，由于矢量容差一般很小，
    故对于矢量容差a，有a≈sin(a)≈tan(a)，cos(a)≈1。
*/
class Tol
{
public:
    //! 全局缺省容差
    /*! 该容差是数学几何库中很多函数的默认容差，可以修改该对象的容差值
    */
    static Tol gTol() {
        static Tol tol;
        return tol;
    }
    
    //! 最小容差
    /*! 该容差的长度容差值和矢量容差值都为 _MGZERO
    */
    static Tol minTol() {
        static const Tol tol(0, 0);
        return tol;
    }
    
    //! 构造默认容差
    /*! 默认构造函数构造出的长度容差值为_MGZERO*2，矢量容差值为1e-4f
    */
    Tol() : mTolPoint(_MGZERO*2), mTolVector(1e-4f) {}
    
    //! 给定容差构造
    /*! 如果给定容差值小于 _MGZERO，将取最小容差值
        \param tolPoint 长度容差值，正数
        \param tolVector 矢量容差值，正数，一般取小于0.1的数
    */
    Tol(float tolPoint, float tolVector = 1e-4f) : mTolPoint(_MGZERO*2) {
        setEqualPoint(tolPoint);
        setEqualVector(tolVector);
    }
    
    //! 返回长度容差
    float equalPoint() const { return mTolPoint; }
    
    //! 返回矢量容差
    float equalVector() const { return mTolVector; }
    
    //! 设置长度容差
    /*! 如果给定容差值小于 _MGZERO，将取最小容差值
        \param tol 长度容差值，正数
    */
    void setEqualPoint(float tol) {
        if (tol < _MGZERO)
            tol = _MGZERO;
        if (!mgEquals(mTolPoint, tol))
            mTolPoint = tol;
    }
    
    //! 设置矢量容差
    /*! 如果给定容差值小于 _MGZERO，将取最小容差值
        \param tol 矢量容差值，正数，一般取小于0.1的数
    */
    void setEqualVector(float tol) {
        if (tol < _MGZERO)
            tol = _MGZERO;
        mTolVector = tol;
    }
    
private:
    float  mTolPoint;      //!< 长度容差
    float  mTolVector;     //!< 矢量容差
};

#endif // TOUCHVG_MGTOL_H_
