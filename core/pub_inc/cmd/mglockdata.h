//! \file mglockdata.h
//! \brief 定义图形文档锁定接口 MgLockData
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_SHAPEDOC_LOCKDATA_H_
#define TOUCHVG_SHAPEDOC_LOCKDATA_H_

class MgShapeDoc;

//! 图形文档锁定接口
/*! \ingroup CORE_SHAPE
    \interface MgLockData
*/
struct MgLockData
{
#ifndef SWIG
    typedef void (*DocLocked)(MgShapeDoc* doc, void* obj, bool locked);
    virtual void registerDocObserver(DocLocked func, void* obj) = 0;
    virtual void unregisterDocObserver(DocLocked func, void* obj) = 0;
#endif
    virtual bool lockData(int flags, int timeout) = 0;
    virtual long unlockData(bool forWrite) = 0;
    virtual void afterChanged() = 0;
    virtual bool lockedForRead() const = 0;
    virtual bool lockedForWrite() const = 0;
    virtual int getEditFlags() const = 0;
    virtual void resetEditFlags() = 0;

    virtual bool lockDynData(bool forWrite, int timeout) = 0;
    virtual long unlockDynData(bool forWrite) = 0;
    virtual bool lockedForReadDyn() const = 0;
    virtual bool lockedForWriteDyn() const = 0;
};

#endif // TOUCHVG_SHAPEDOC_LOCKDATA_H_
