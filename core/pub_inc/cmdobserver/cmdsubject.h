//! \file cmdsubject.h
//! \brief 定义命令扩展目标接口 CmdSubject
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMDSUBJECT_H_
#define TOUCHVG_CMDSUBJECT_H_

#include "cmdobserver.h"

//! 命令扩展目标接口
/*! \ingroup CORE_COMMAND
 */
struct CmdSubject : public CmdObserver {
    //! 注册观察者
    virtual void registerObserver(CmdObserver* observer) = 0;
    //! 注销观察者
    virtual void unregisterObserver(CmdObserver* observer) = 0;
};

#endif // TOUCHVG_CMDSUBJECT_H_
