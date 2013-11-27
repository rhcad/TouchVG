//! \file mgjsonstorage.h
//! \brief 定义JSON序列化类 MgJsonStorage
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_JSONSTORAGE_H_
#define TOUCHVG_CORE_JSONSTORAGE_H_

#ifndef SWIG
#include <cstdio>
inline FILE* mgopenfile(const char* fn, const char* m) {
#if defined(_MSC_VER) && _MSC_VER >= 1400
    FILE* fp = NULL; fopen_s(&fp, fn, m); return fp;
#else
    return fopen(fn, m);
#endif
}
#endif
struct MgStorage;

//! JSON序列化类
/*! \ingroup CORE_STORAGE
 */
class MgJsonStorage
{
public:
    MgJsonStorage();
    ~MgJsonStorage();
    
    //! 给定JSON内容，返回存取接口对象以便开始读取
    MgStorage* storageForRead(const char* content);

    //! 返回存取接口对象以便开始写数据，写完可调用 stringify()
    MgStorage* storageForWrite();

#ifndef SWIG
    //! 给定JSON文件句柄，返回存取接口对象以便开始读取
    MgStorage* storageForRead(FILE* fp);

    //! 写数据到给定的文件
    bool save(FILE* fp, bool pretty = true);
#endif
    
    //! 返回JSON内容
    const char* stringify(bool pretty = true);
    
    //! 清除内存资源
    void clear();
    
    //! 返回 storageForRead() 中的解析错误，NULL表示没有错误
    const char* getParseError();
    
private:
    class Impl;
    Impl* _impl;
};

#endif // TOUCHVG_CORE_JSONSTORAGE_H_
