//! \file giplaying.h
//! \brief 定义图形播放项类 GiPlaying
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_PLAYING_H
#define TOUCHVG_CORE_PLAYING_H

struct MgCoreView;
class MgShapeDoc;
class MgShapes;

//! 图形播放项
/*!
    \ingroup CORE_VIEW
 */
class GiPlaying
{
public:
    static GiPlaying* fromHandle(long h) { GiPlaying* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() const { long h; *(const GiPlaying**)&h = this; return h; }   //!< 得到句柄
    
    enum { kDrawingTag = -1, kPlayingTag = -2 };
    static GiPlaying* create(MgCoreView* v, int tag, bool doubleSided = true);   //!< 创建播放项
    
    void release(MgCoreView* v);                //!< 销毁播放项
    void clear();                               //!< 清除图形
    int getTag() const;                         //!< 得到标识号
    
    long acquireFrontDoc();                     //!< 得到显示用的图形文档句柄，需要并发保护
    static void releaseDoc(long doc);           //!< 释放 acquireDoc() 返回的句柄
    MgShapeDoc* getBackDoc();                   //!< 得到修改图形用的图形文档
    void submitBackDoc();                       //!< 提交图形文档结果，需要并发保护
    
    long acquireFrontShapes();                  //!< 得到显示用的图形列表句柄，需要并发保护
    static void releaseShapes(long shapes);     //!< 释放 acquireShapes() 返回的句柄
    long getBackShapesHandle(bool needClear);   //!< 得到修改图形用的动态图形列表句柄
    MgShapes* getBackShapes(bool needClear);    //!< 得到修改图形用的动态图形列表
    void submitBackShapes();                    //!< 提交动态图形列表结果，需要并发保护
    
    void stop();                                //!< 标记需要停止
    bool isStopping() const;                    //!< 返回是否待停止
    
private:
    GiPlaying(int tag, bool doubleSided);
    ~GiPlaying();
    
    struct Impl;
    Impl* impl;
};

#endif // TOUCHVG_CORE_PLAYING_H
