//! \file mgcoreview.h
//! \brief 定义内核视图接口 MgCoreView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_VIEW_INTERFACE_H
#define TOUCHVG_CORE_VIEW_INTERFACE_H

#include "mgvector.h"
#include "mgstrcallback.h"

class GiContext;
struct MgStorage;

//! 遍历图像对象的回调接口
/*! \ingroup CORE_VIEW
    \interface MgFindImageCallback
 */
struct MgFindImageCallback {
    virtual ~MgFindImageCallback() {}
    //! 找到一个图形ID对应的图像名称
    virtual void onFindImage(int sid, const char* name) = 0;
};

//! 内核视图接口
/*! \ingroup CORE_VIEW
    \interface MgCoreView
 */
struct MgCoreView {
    enum {
        kMgSquare,          //!< 方形
        kMgClosed,          //!< 闭合
        kMgFixedLength,     //!< 边长固定
        kMgFixedSize,       //!< 大小固定，只能旋转和移动
        kMgRotateDisnable,  //!< 不能旋转
        kMgLocked,          //!< 锁定形状
        kMgNoSnap,          //!< 禁止捕捉
        kMgNoAction,        //!< 禁止上下文按钮
        kMgNoClone,         //!< 禁止克隆
        kMgHideContent,     //!< 隐藏内容
        kMgNoDel,           //!< 禁止删除
        kMgCanSelLocked,    //!< 允许选中，即使锁定
        kMgNotAddRel,       //!< 不自动加约束
        kMgNotShowSnap,     //!< 不显示捕捉提示
        kMgCanAddVertex,    //!< 允许增删顶点
    };
    
    virtual ~MgCoreView() {}
    static MgCoreView* fromHandle(long h) { MgCoreView* p; *(long*)&p = h; return p; } //!< 转为对象
    long toHandle() const { long h; *(const MgCoreView**)&h = this; return h; }   //!< 得到句柄，用于跨库转换
    
    virtual void release() = 0;                     //!< 释放引用计数，为0时销毁对象
    virtual void addRef() = 0;                      //!< 添加引用计数
    virtual long viewDataHandle() = 0;              //!< 内部数据句柄, 可转换为 GiCoreViewData 指针
    virtual long viewAdapterHandle() = 0;           //!< 命令视图回调适配器的句柄, 可转换为 MgView 指针
    virtual long backDoc() = 0;                     //!< 图形文档的句柄, 用 MgShapeDoc::fromHandle() 转换
    virtual long backShapes() = 0;                  //!< 当前图形列表的句柄, 用 MgShapes::fromHandle() 转换
    
    virtual long acquireFrontDoc() = 0;             //!< 获取前端图形文档的句柄, 需要并发访问保护
    virtual long acquireFrontDoc(long playh) = 0;   //!< 获取指定数据源的前端图形文档的句柄
    static void releaseDoc(long doc);               //!< 释放 acquireFrontDoc() 得到的文档句柄
    virtual long acquireDynamicShapes() = 0;        //!< 获取动态图形列表的句柄, 需要并发访问保护
    static void releaseShapes(long shapes);         //!< 释放 acquireDynamicShapes() 得到的图形列表句柄
    
    virtual bool isDrawing() = 0;                   //!< 返回是否正在绘制静态图形
    virtual bool isZooming() = 0;                   //!< 返回是否正在动态放缩显示
    virtual bool isStopping() = 0;                  //!< 返回是否需要停止绘图
    virtual int stopDrawing(bool stop = true) = 0;  //!< 标记需要停止绘图

    virtual bool isUndoRecording() const = 0;       //!< 是否正在Undo录制
    virtual bool isRecording() const = 0;           //!< 是否正在录屏
    virtual bool isPlaying() const = 0;             //!< 是否处于播放模式
    virtual bool isPaused() const = 0;              //!< 返回是否已暂停
    virtual long getRecordTick(bool forUndo, long curTick) = 0;    //!< 得到已开始的相对毫秒时刻
    virtual bool isUndoLoading() const = 0;         //!< 是否正加载文档
    virtual bool canUndo() const = 0;               //!< 能否撤销
    virtual bool canRedo() const = 0;               //!< 能否重做
    virtual int getRedoIndex() const = 0;           //!< 得到当前Undo位置
    virtual int getRedoCount() const = 0;           //!< 得到Undo文件数
    
    long getPlayingTick(long curTick) { return getRecordTick(false, curTick); }  //!< 得到已播放的毫秒数
    virtual long getFrameTick() = 0;                //!< 得到当前帧的相对毫秒时刻
    virtual int getFrameFlags() = 0;                //!< 得到当前帧的改动标志
    virtual int getFrameIndex() const = 0;          //!< 得到已播放的帧数
    
    virtual bool isPressDragging() = 0;             //!< 是否按下并拖动
    virtual bool isDrawingCommand() = 0;            //!< 当前是否为绘图命令
    virtual bool isCommand(const char* name) = 0;   //!< 当前是否为指定名称的命令
#ifndef SWIG
    virtual const char* getCommand() const = 0;     //!< 返回当前命令名称
#endif
    void getCommand(MgStringCallback* c) { c->onGetString(getCommand()); }  //!< 得到当前命令名称
    virtual bool setCommand(const char* name, const char* params = "") = 0; //!< 启动命令
    virtual bool switchCommand() = 0;               //!< 切换到下一命令
    virtual bool doContextAction(int action) = 0;   //!< 执行上下文动作
    
    virtual void clearCachedData() = 0;             //!< 释放临时数据内存，未做线程保护
    virtual int addShapesForTest(int n = 1000) = 0; //!< 添加测试图形
    
    virtual int getShapeCount() = 0;                //!< 返回后端文档的图形总数
    virtual int getShapeCount(long doc) = 0;        //!< 返回前端文档的图形总数
    virtual int getUnlockedShapeCount(int type = 0) = 0;    //!< 返回未锁定的可见图形的个数
    virtual int getVisibleShapeCount(int type = 0) = 0;     //!< 返回可见图形的个数
    virtual long getChangeCount() = 0;              //!< 返回静态图形改变次数，可用于检查是否需要保存
    virtual long getDrawCount() const = 0;          //!< 返回已绘制次数，可用于录屏
    virtual int getSelectedShapeCount() = 0;        //!< 返回选中的图形个数
    virtual int getSelectedShapeType() = 0;         //!< 返回选中的图形的类型, MgShapeType
    virtual int getSelectedShapeID() = 0;           //!< 返回当前选中的图形的ID，选中多个时只取第一个
    virtual int getSelectedHandle() = 0;            //!< 返回当前线性图形中当前控制点序号
    virtual void getSelectedShapeIDs(mgvector<int>& ids) = 0;   //!< 返回当前选中的图形的ID
    virtual void setSelectedShapeIDs(const mgvector<int>& ids) = 0; //!< 选中指定ID的图形

    virtual void clear() = 0;                       //!< 删除所有图形，包括锁定的图形
    virtual bool loadFromFile(const char* vgfile, bool readOnly = false) = 0;       //!< 从文件或JSON串中加载
    virtual bool saveToFile(long doc, const char* vgfile, bool pretty = false) = 0; //!< 保存图形
    bool saveToFile(const char* vgfile, bool pretty = false);           //!< 保存图形，主线程中用
    
    virtual bool loadShapes(MgStorage* s, bool readOnly = false) = 0;   //!< 从数据源中加载图形
    virtual bool saveShapes(long doc, MgStorage* s) = 0;                //!< 保存图形到数据源
    bool saveShapes(MgStorage* s);                      //!< 保存图形到数据源，主线程中用

#ifndef SWIG
    virtual const char* getContent(long doc) = 0;       //!< 得到图形的JSON内容，需要调用 freeContent()
    const char* getContent();                           //!< 得到图形内容，需调用 freeContent()，主线程中用
#endif
    void getContent(long doc, MgStringCallback* c);     //!< 得到图形的JSON内容，自动 freeContent()
    void getContent(MgStringCallback* c);               //!< 得到图形的JSON内容，主线程中用
    virtual void freeContent() = 0;                     //!< 释放 getContent() 产生的缓冲资源
    virtual bool setContent(const char* content, bool readOnly = false) = 0;    //!< 从JSON内容中加载图形

    virtual bool zoomToInitial() = 0;                   //!< 放缩到文档初始状态
    virtual bool zoomToExtent(float margin = 2) = 0;    //!< 放缩显示全部内容到视图区域
    virtual bool zoomToModel(float x, float y, float w, float h, float margin = 2) = 0; //!< 放缩显示指定范围到视图区域
    virtual bool zoomPan(float dxPixel, float dyPixel, bool adjust = true) = 0; //!< 平移显示
    
    virtual GiContext& getContext(bool forChange) = 0;  //!< 当前绘图属性，可用 calcPenWidth() 计算线宽
    virtual void setContext(int mask) = 0;              //!< 绘图属性改变后提交更新
    virtual bool getShapeFlag(int sid, int bit) = 0;    //!< 返回图形标志, kMgNoSnap 等位值
    virtual bool setShapeFlag(int sid, int bit, bool on) = 0;   //!< 设置图形标志

    //! 绘图属性改变后提交更新
    /*! 在 getContext(true) 后调用本函数。
        \param ctx 绘图属性
        \param mask 需要应用哪些属性(GiContextBits)，-1表示全部属性，0则不修改，按位组合值见 GiContextBits
        \param apply 0表示还要继续动态修改属性，1表示结束动态修改并提交，-1表示结束动态修改并放弃改动
     */
    virtual void setContext(const GiContext& ctx, int mask, int apply) = 0;

    //! 设置线条属性是否正在动态修改. getContext(false)将重置为未动态修改.
    virtual void setContextEditing(bool editing) = 0;
    
    //! 添加一个容纳图像的矩形图形
    /*!
        \param name 图像的标识名称，例如可用无路径的文件名
        \param width 图像的原始宽度，正数，单位为点
        \param height 图像的原始高度，正数，单位为点
        \return 新图形的ID，0表示失败
     */
    virtual int addImageShape(const char* name, float width, float height) = 0;
    
    //! 添加一个容纳图像的矩形图形
    virtual int addImageShape(const char* name, float xc, float yc, float w, float h, int tag) = 0;
    
    //! 返回是否有容纳图像的图形对象
    virtual bool hasImageShape(long doc) = 0;
    
    //! 获取图像(MgImageShape)的原始图像宽高、显示宽高、角度(orgw, orgh, w, h, angle)
    virtual bool getImageSize(mgvector<float>& info, int sid) = 0;
    
    //! 查找指定名称的图像对应的图形对象ID
    virtual int findShapeByImageID(long doc, const char* name) = 0;
    
    //! 查找指定Tag的图形对象ID
    virtual int findShapeByTag(long doc, int tag) = 0;
    
    //! 遍历有容纳图像的图形对象
    virtual int traverseImageShapes(long doc, MgFindImageCallback* c) = 0;
    
    //! 返回当前视图区域的模型坐标范围，模型坐标(left, top, right, bottom)
    virtual bool getViewModelBox(mgvector<float>& box) = 0;

    //! 返回后端文档的模型坐标范围，模型坐标(left, top, right, bottom)
    virtual bool getModelBox(mgvector<float>& box) = 0;
    
    //! 返回后端文档中指定ID的模型坐标范围，模型坐标(left, top, right, bottom)
    virtual bool getModelBox(mgvector<float>& box, int shapeId) = 0;
    
    //! 得到指定序号的控制点的模型坐标
    virtual bool getHandlePoint(mgvector<float>& xy, int shapeId, int index) = 0;
    
    //! 返回后端文档的图形显示范围，四个点单位坐标(left, top, right, bottom)
    virtual bool getDisplayExtent(mgvector<float>& box) = 0;
    
    //! 返回前端文档的图形显示范围，四个点单位坐标(left, top, right, bottom)
    virtual bool getDisplayExtent(long doc, long gs, mgvector<float>& box) = 0;

    //! 返回选择包络框，四个点单位坐标(left, top, right, bottom)
    virtual bool getBoundingBox(mgvector<float>& box) = 0;
    
    //! 返回后端文档中指定ID的图形的包络框，四个点单位坐标(left, top, right, bottom)
    virtual bool getBoundingBox(mgvector<float>& box, int shapeId) = 0;
    
    //! 返回前端文档中指定ID的图形的包络框，四个点单位坐标(left, top, right, bottom)
    virtual bool getBoundingBox(long doc, long gs, mgvector<float>& box, int shapeId) = 0;
    
    //! 视图坐标转为模型坐标，可传入2或4个分量
    virtual bool displayToModel(mgvector<float>& d) = 0;
    
    //! 用SVG路径的d坐标序列创建或设置图形形状，成功返回图形ID(未重新显示)，失败返回0
    virtual int importSVGPath(long shapes, int sid, const char* d) = 0;
    
    //! 输出SVG路径的d坐标序列，返回复制的长度或应分配的长度(不含结束符)
    virtual int exportSVGPath(long shapes, int sid, char* buf, int size) = 0;
    
    //! 输出SVG路径的d坐标序列
    bool exportSVGPath2(MgStringCallback* c, long shapes, int sid) {
        int size = exportSVGPath(shapes, sid, (char*)0, 0);
        if (size > 0) {
            char* buf = new char[1 + size];
            exportSVGPath(shapes, sid, buf, size);
            buf[size] = 0;
            c->onGetString(buf);
            delete[] buf;
        }
        return size > 0;
    }
};

inline bool MgCoreView::saveToFile(const char* vgfile, bool pretty) {
    long doc = acquireFrontDoc();
    bool ret = saveToFile(doc, vgfile, pretty);
    releaseDoc(doc);
    return ret;
}
inline bool MgCoreView::saveShapes(MgStorage* s) {
    long doc = acquireFrontDoc();
    bool ret = saveShapes(doc, s);
    releaseDoc(doc);
    return ret;
}
inline const char* MgCoreView::getContent() {
    long doc = acquireFrontDoc();
    const char* ret = getContent(doc);
    releaseDoc(doc);
    return ret;
}
inline void MgCoreView::getContent(long doc, MgStringCallback* c) {
    c->onGetString(getContent(doc));
    freeContent();
}
inline void MgCoreView::getContent(MgStringCallback* c) {
    c->onGetString(getContent());
    freeContent();
}

#endif // TOUCHVG_CORE_VIEW_INTERFACE_H
