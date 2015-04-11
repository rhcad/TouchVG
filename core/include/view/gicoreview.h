//! \file gicoreview.h
//! \brief 定义内核视图分发器类 GiCoreView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_VIEWDISPATCHER_H
#define TOUCHVG_CORE_VIEWDISPATCHER_H

#include "gigesture.h"
#include "giview.h"
#include "mgcoreview.h"

class GiCanvas;
class GiCoreViewImpl;
struct MgView;

//! 获取配置项的回调接口
/*! \ingroup CORE_VIEW
    \interface MgOptionCallback
 */
struct MgOptionCallback {
    virtual ~MgOptionCallback() {}
    
    virtual void onGetOptionBool(const char* name, bool value) = 0;     //!< 布尔选项值
    virtual void onGetOptionInt(const char* name, int value) = 0;       //!< 整型选项值
    virtual void onGetOptionFloat(const char* name, float value) = 0;   //!< 浮点型选项值
    virtual void onGetOptionString(const char* name, const char* text) = 0; //!< 文本选项值
};

//! 避免重复触发 regenAll/redraw 的辅助类
class MgRegenLocker
{
public:
    MgRegenLocker(MgView* view);
    ~MgRegenLocker();
private:
    void* obj;
};

//! 内核视图分发器类
/*! 本对象拥有图形文档对象，负责显示和手势动作的分发。
    \ingroup CORE_VIEW
 */
class GiCoreView : public MgCoreView
{
public:
    enum { kNoCmdType = -1, kTestType = 0, kNormalType = 1 };
    static GiCoreView* createView(GiView* view, int type = kNormalType);    //!< 创建内核视图
    static GiCoreView* createMagnifierView(GiView* newview, GiCoreView* mainView,
                                           GiView* mainDevView);    //!< 创建放大镜视图
    void destoryView(GiView* view);                                 //!< 销毁内核视图
    
    long acquireGraphics(GiView* view);                             //!< 获取前端 GiGraphics 的句柄
    void releaseGraphics(long gs);                                  //!< 释放 GiGraphics 句柄
    int acquireFrontDocs(mgvector<long>& docs);                     //!< 获取前端图形文档的句柄
    static void releaseDocs(const mgvector<long>& docs);            //!< 释放文档句柄
    int getSkipDrawIds(mgvector<int>& ids);                         //!< 获取不在静态图形中显示的图形ID
    int acquireDynamicShapesArray(mgvector<long>& shapes);          //!< 获取前端图形列表的句柄
    static void releaseShapesArray(const mgvector<long>& shapes);   //!< 释放图形列表句柄
    
    int drawAll(long doc, long gs, GiCanvas* canvas);               //!< 显示所有图形
    int drawAll(const mgvector<long>& docs, long gs, GiCanvas* canvas);  //!< 显示所有图形
    int drawAll(const mgvector<long>& docs, long gs, GiCanvas* canvas,
                const mgvector<int>& ignoreIds);                    //!< 显示除特定ID外的图形
    int drawAppend(long doc, long gs, GiCanvas* canvas, int sid);   //!< 显示新图形
    int dynDraw(long shapes, long gs, GiCanvas* canvas);            //!< 显示动态图形
    int dynDraw(const mgvector<long>& shapes, long gs, GiCanvas* canvas); //!< 显示动态图形
    
    int drawAll(GiView* view, GiCanvas* canvas);                    //!< 显示所有图形，主线程中用
    int drawAppend(GiView* view, GiCanvas* canvas, int sid);        //!< 显示新图形，主线程中用
    int dynDraw(GiView* view, GiCanvas* canvas);                    //!< 显示动态图形，主线程中用
    
    int setBkColor(GiView* view, int argb);                         //!< 设置背景颜色
    static void setScreenDpi(int dpi, float factor = 1.f);          //!< 设置屏幕的点密度和UI放缩系数
    void onSize(GiView* view, int w, int h);                        //!< 设置视图的宽高
    void setViewScaleRange(GiView* view, float minScale, float maxScale);   //!< 设置显示比例范围
    void setPenWidthRange(GiView* view, float minw, float maxw);    //!< 设置画笔宽度范围
    
    void setGestureVelocity(GiView* view, float vx, float vy);      //!< 设置移动速度，每秒点数
    //! 传递单指触摸手势消息
    bool onGesture(GiView* view, GiGestureType type,
            GiGestureState state, float x, float y, bool switchGesture = false);
    //! 传递双指移动手势(可放缩旋转)
    bool twoFingersMove(GiView* view, GiGestureState state,
            float x1, float y1, float x2, float y2, bool switchGesture = false);
    
    bool submitBackDoc(GiView* view, bool changed);                 //!< 提交静态图形到前端，在UI的regen回调中用
    bool submitDynamicShapes(GiView* view);                         //!< 提交动态图形到前端，需要并发保护
    
    float calcPenWidth(GiView* view, float lineWidth);              //!< 计算画笔的像素宽度
    GiGestureType getGestureType();                                 //!< 得到当前手势类型
    GiGestureState getGestureState();                               //!< 得到当前手势状态
    static int getVersion();                                        //!< 得到内核版本号
    bool isZoomEnabled(GiView* view);                               //!< 是否允许放缩显示
    void setZoomEnabled(GiView* view, bool enabled);                //!< 设置是否允许放缩显示
    
    int exportSVG(long doc, long gs, const char* filename);         //!< 导出图形到SVG文件
    int exportSVG(GiView* view, const char* filename);              //!< 导出图形到SVG文件，主线程中用
    bool startRecord(const char* path, long doc,
                     bool forUndo, long curTick,
                     MgStringCallback* c = (MgStringCallback*)0);   //!< 开始录制图形，自动释放，在主线程用
    void stopRecord(bool forUndo);                                  //!< 停止录制图形
    bool recordShapes(bool forUndo, long tick, long changeCount, long doc, long shapes); //!< 录制图形，自动释放
    bool recordShapes(bool forUndo, long tick, long changeCount, long doc,
                      long shapes, const mgvector<long>* exts,
                      MgStringCallback* c = (MgStringCallback*)0);  //!< 录制图形，自动释放
    bool undo(GiView* view);                                        //!< 撤销, 需要并发访问保护
    bool redo(GiView* view);                                        //!< 重做, 需要并发访问保护
    bool onPause(long curTick);                                     //!< 暂停
    bool onResume(long curTick);                                    //!< 继续
    bool restoreRecord(int type, const char* path, long doc, long changeCount,
                       int index, int count, int tick, long curTick);   //!< 恢复录制
    
    void traverseOptions(MgOptionCallback* c);                      //!< 遍历选项
    void setOptionBool(const char* name, bool value);               //!< 设置或清除布尔选项值
    void setOptionInt(const char* name, int value);                 //!< 设置或清除整型选项值
    void setOptionFloat(const char* name, float value);             //!< 设置或清除浮点型选项值
    void setOptionString(const char* name, const char* value);      //!< 设置或清除文本选项值
    
// MgCoreView
#ifndef SWIG
public:
    void release();
    void addRef();
    bool isPressDragging();
    bool isDrawingCommand();
    bool isCommand(const char* name);
    long viewDataHandle();
    long viewAdapterHandle();
    long backDoc();
    long backShapes();
    long acquireFrontDoc();
    long acquireFrontDoc(long playh);
    long acquireDynamicShapes();
    bool isDrawing();
    bool isZooming();
    bool isStopping();
    int stopDrawing(bool stop = true);
    bool isUndoRecording() const;
    bool isRecording() const;
    bool isPlaying() const;
    bool isPaused() const;
    long getRecordTick(bool forUndo, long curTick);
    bool isUndoLoading() const;
    bool canUndo() const;
    bool canRedo() const;
    int getRedoIndex() const;
    int getRedoCount() const;
    long getFrameTick();
    int getFrameFlags();
    int getFrameIndex() const;
    const char* getCommand() const;
    bool setCommand(const char* name, const char* params = "");
    bool switchCommand();
    bool doContextAction(int action);
    void clearCachedData();
    int addShapesForTest(int n = 1000);
    int getShapeCount();
    int getShapeCount(long doc);
    int getUnlockedShapeCount(int type = 0);
    int getVisibleShapeCount(int type = 0);
    long getChangeCount();
    long getDrawCount() const;
    int getSelectedShapeCount();
    int getSelectedShapeType();
    int getSelectedShapeID();
    int getSelectedHandle();
    void getSelectedShapeIDs(mgvector<int>& ids);
    void setSelectedShapeIDs(const mgvector<int>& ids);
    void clear();
    bool loadFromFile(const char* vgfile, bool readOnly = false);
    bool saveToFile(long doc, const char* vgfile, bool pretty = false);
    bool loadShapes(MgStorage* s, bool readOnly = false);
    bool saveShapes(long doc, MgStorage* s);
    const char* getContent(long doc);
    void freeContent();
    bool setContent(const char* content, bool readOnly = false);
    bool zoomToInitial();
    bool zoomToExtent(float margin = 2);
    bool zoomToModel(float x, float y, float w, float h, float margin = 2);
    bool zoomPan(float dxPixel, float dyPixel, bool adjust=true);
    GiContext& getContext(bool forChange);
    void setContext(const GiContext& ctx, int mask, int apply);
    void setContext(int mask);
    bool getShapeFlag(int sid, int bit);
    bool setShapeFlag(int sid, int bit, bool on);
    void setContextEditing(bool editing);
    int addImageShape(const char* name, float width, float height);
    int addImageShape(const char* name, float xc, float yc, float w, float h, int tag);
    bool getImageSize(mgvector<float>& info, int sid);
    bool hasImageShape(long doc);
    int findShapeByImageID(long doc, const char* name);
    int findShapeByTag(long doc, int tag);
    int traverseImageShapes(long doc, MgFindImageCallback* c);
    bool getViewModelBox(mgvector<float>& box);
    bool getModelBox(mgvector<float>& box);
    bool getModelBox(mgvector<float>& box, int shapeId);
    bool getHandlePoint(mgvector<float>& xy, int shapeId, int index);
    bool getDisplayExtent(mgvector<float>& box);
    bool getDisplayExtent(long doc, long gs, mgvector<float>& box);
    bool getBoundingBox(mgvector<float>& box);
    bool getBoundingBox(mgvector<float>& box, int shapeId);
    bool getBoundingBox(long doc, long gs, mgvector<float>& box, int shapeId);
    bool displayToModel(mgvector<float>& d);
    int importSVGPath(long shapes, int sid, const char* d);
    int exportSVGPath(long shapes, int sid, char* buf, int size);
#endif // SWIG

private:
    GiCoreView(GiCoreView* mainView = (GiCoreView*)0);
    GiCoreView(GiView* view, int type);
    virtual ~GiCoreView();
    void createView_(GiView* view, int type = 1);                   //!< 创建内核视图
    void createMagnifierView_(GiView* newview, GiView* mainView);   //!< 创建放大镜视图
    
    GiCoreViewImpl* impl;
    volatile long refcount;
};

#ifndef DOXYGEN
#include "gicontxt.h"
#endif

#endif // TOUCHVG_CORE_VIEWDISPATCHER_H
