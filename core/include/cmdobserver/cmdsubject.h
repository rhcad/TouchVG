//! \file cmdsubject.h
//! \brief 定义命令扩展目标接口 CmdSubject
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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
    
    //! 注册命名观察者
    virtual bool registerNamedObserver(const char* name, CmdObserver* observer) = 0;
    //! 查找命名观察者
    virtual CmdObserver* findNamedObserver(const char* name) = 0;
};

#endif // TOUCHVG_CMDSUBJECT_H_
