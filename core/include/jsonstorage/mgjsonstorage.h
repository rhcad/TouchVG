//! \file mgjsonstorage.h
//! \brief 定义JSON序列化类 MgJsonStorage
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

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

//! 打开JSON文件的辅助类
class MgJsonFile
{
public:
    MgJsonFile(const char* filename, bool forRead) {
        fp = mgopenfile(filename, forRead ? "rt" : "wt");
    }
    ~MgJsonFile() { close(); }
    bool opened() const { return !!fp; }
    
    void close() {
        if (fp) {
            fclose(fp);
            fp = (FILE*)0;
        }
    }
#ifndef SWIG
    FILE* getHandle() const { return fp; }
#endif
    
private:
    FILE* fp;
};

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
    bool save(FILE* fp, bool pretty = false);
#endif
    
    //! 给定JSON文件对象，返回存取接口对象以便开始读取
    MgStorage* storageForRead(const MgJsonFile& file) { return storageForRead(file.getHandle()); }
    
    //! 写数据到给定的文件
    bool save(const MgJsonFile& file, bool pretty = false) { return save(file.getHandle(), pretty); }
    
    //! 返回JSON内容
    const char* stringify(bool pretty = false);
    
    //! 清除内存资源
    void clear();
    
    //! 返回 storageForRead() 中的解析错误，NULL表示没有错误
    const char* getParseError();
    
    //! 设置是否使用数组模式，默认在对象中添加由名称和序号组成的子对象，true表示在数组中添加元素
    void setArrayMode(bool arr);
    
    //! 设置是否在保存数值键值时加上引号
    void saveNumberAsString(bool str);
    
    //! UTF-16/32编码的文件转换为UTF-8编码的文件，返回转换与否
    static bool toUTF8(const char* infile, const char* outfile);
    
    //! UTF-8编码的文件转换为UTF-16LE编码的文件，返回转换与否
    static bool toUTF16(const char* infile, const char* outfile);
    
#ifndef SWIG
    static bool parseInt(const char* str, int& value);
    static bool parseFloat(const char* str, float& value);
    static bool parseFloat(const char* str, double& value);
#endif

private:
    class Impl;
    Impl* _impl;
};

#endif // TOUCHVG_CORE_JSONSTORAGE_H_
