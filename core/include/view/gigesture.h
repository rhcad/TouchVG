//! \file gigesture.h
//! \brief 定义手势类型 GiGestureType
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_GESTURE_H
#define TOUCHVG_CORE_GESTURE_H

typedef enum {                  //!< 手势类型
    kGiGestureUnknown,          //!< 未知的手势
    kGiGesturePan,              //!< 单指滑动
    kGiGestureTap,              //!< 单指单击
    kGiGestureDblTap,           //!< 单指双击
    kGiGesturePress,            //!< 单指长按
    kGiTwoFingersMove,          //!< 双指移动(可放缩旋转)
} GiGestureType;

typedef enum {                  //!< 手势状态
    kGiGesturePossible,         //!< 待检查手势有效性
    kGiGestureBegan,            //!< 开始
    kGiGestureMoved,            //!< 改变
    kGiGestureEnded,            //!< 结束
    kGiGestureCancel,           //!< 取消
} GiGestureState;

#endif // TOUCHVG_CORE_GESTURE_H
