//! \file mgobject.h
//! \brief 定义图形对象基类 MgObject
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGOBJECT_H_
#define TOUCHVG_MGOBJECT_H_

//! 图形对象基类
/*! \ingroup CORE_SHAPE
*/
class MgObject
{
public:
    //! 复制出一个新对象
    virtual MgObject* clone() const = 0;

    //! 复制对象数据
    virtual void copy(const MgObject& src) = 0;

    //! 释放引用计数，为0时销毁对象
    virtual void release() = 0;
    
    //! 添加引用计数
    virtual void addRef() = 0;

    //! 比较与另一同类对象是否相同
    virtual bool equals(const MgObject& src) const = 0;

    //! 返回对象类型
    virtual int getType() const = 0;

    //! 返回是否能转化为指定类型的对象，即本类为指定类或其派生类
    virtual bool isKindOf(int type) const = 0;

#ifndef SWIG
    MgObject& operator=(const MgObject& src) { copy(src); return *this; }
    bool operator==(const MgObject& src) const { return equals(src); }
    bool operator!=(const MgObject& src) const { return !equals(src); }
    
    template <class T> static void release_pointer(T*& p) { if (p) { p->release(); p = (T*)0; } }
#endif // SWIG

protected:
    virtual ~MgObject() {}
};

#endif // TOUCHVG_MGOBJECT_H_
