//! \file mgstoragebs.h
//! \brief 定义供SWIG输出给其他语言用的序列化基类 MgStorageBase
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_MGSTORAGEBASE_H_
#define TOUCHVG_MGSTORAGEBASE_H_

#include "mgstorage.h"
#include "mgvector.h"

//! 供SWIG输出给其他语言用的序列化基类
/*! \ingroup CORE_STORAGE
 */
class MgStorageBase : public MgStorage
{
public:
    MgStorageBase() {}
    virtual ~MgStorageBase() {}
    
    virtual int readFloatArray(const char* name, mgvector<float>& values) = 0;
    virtual void writeFloatArray(const char* name, const mgvector<float>& values) = 0;
    virtual int readString(const char* name, mgvector<char>& value) = 0;

private:
    virtual int readFloatArray(const char* name, float* values, int count) {
        mgvector<float> arr(values, count);
        int n = readFloatArray(name, arr);
        if (values && count > 0) {
            n = n < count ? n : count;
            for (int i = 0; i < n; i++)
                values[i] = arr.get(i);
        }
        return n;
    }
    virtual int readString(const char* name, char* value, int count) {
        mgvector<char> arr(value, count);
        int n = readString(name, arr);
        if (value && count > 0) {
            n = n < count ? n : count;
            for (int i = 0; i < n; i++)
                value[i] = arr.get(i);
        }
        return n;
    }
    virtual void writeFloatArray(const char* name, const float* values, int count) {
        mgvector<float> arr(values, count);
        writeFloatArray(name, arr);
    }
};

#endif // TOUCHVG_MGSTORAGEBASE_H_
