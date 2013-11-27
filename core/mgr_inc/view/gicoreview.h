//! \file gicoreview.h
//! \brief 定义内核视图分发器类 GiCoreView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_VIEWDISPATCHER_H
#define TOUCHVG_CORE_VIEWDISPATCHER_H

#include "gigesture.h"
#include "giview.h"
#include "mgcoreview.h"

class GiCanvas;
class GiCoreViewImpl;
class MgView;

//! 内核视图分发器类
/*! 本对象拥有图形文档对象，负责显示和手势动作的分发。
    \ingroup CORE_VIEW
 */
class GiCoreView : public MgCoreView
{
public:
    //! 构造函数，传入NULL构造主视图，传入主视图构造辅助视图
    GiCoreView(GiCoreView* mainView = (GiCoreView*)0);
    
    //! 析构函数
    virtual ~GiCoreView();
    
    //! 创建内核视图
    void createView(GiView* view, int type = 1);
    
    //! 创建放大镜视图
    void createMagnifierView(GiView* newview, GiView* mainView);
    
    //! 销毁内核视图
    void destoryView(GiView* view);
    
    //! 返回是否正在绘制静态图形
    bool isDrawing(GiView* view);
    
    //! 标记需要停止绘图
    void stopDrawing(GiView* view);
    
    //! 显示所有图形
    int drawAll(GiView* view, GiCanvas* canvas);

    //! 显示新图形，在 GiView.regenAppend() 后调用
    int drawAppend(GiView* view, GiCanvas* canvas);
    
    //! 显示动态图形
    void dynDraw(GiView* view, GiCanvas* canvas);
    
    //! 设置背景颜色
    int setBkColor(GiView* view, int argb);

    //! 设置屏幕的点密度
    static void setScreenDpi(int dpi);
    
    //! 设置视图的宽高
    void onSize(GiView* view, int w, int h);
    
    //! 传递单指触摸手势消息
    bool onGesture(GiView* view, GiGestureType type,
            GiGestureState state, float x, float y, bool switchGesture = false);

    //! 传递双指移动手势(可放缩旋转)
    bool twoFingersMove(GiView* view, GiGestureState state,
            float x1, float y1, float x2, float y2, bool switchGesture = false);
    
    GiGestureType getGestureType();     //!< 得到当前手势类型
    GiGestureState getGestureState();   //!< 得到当前手势状态
    
public:
    bool isPressDragging();
    MgView* viewAdapter();
    long viewAdapterHandle();
    long docHandle();
    long shapesHandle();
    const char* getCommand() const;
    bool setCommand(GiView* view, const char* name, const char* params = "");
    bool setCommand(const char* name, const char* params = "");
    bool doContextAction(int action);
    void clearCachedData();
    int addShapesForTest();
    int getShapeCount();
    long getChangeCount();
    long getDrawCount() const;
    int getSelectedShapeCount();
    int getSelectedShapeType();
    int getSelectedShapeID();
    void clear();
    bool loadFromFile(const char* vgfile, bool readOnly = false, bool needLock = true);
    bool saveToFile(const char* vgfile, bool pretty = true);
    bool loadShapes(MgStorage* s, bool readOnly = false, bool needLock = true);
    bool saveShapes(MgStorage* s);
    bool loadDynamicShapes(MgStorage* s);
    const char* getContent();
    void freeContent();
    bool setContent(const char* content);
    bool exportSVG(const char* filename);
    bool zoomToExtent();
    bool zoomToModel(float x, float y, float w, float h);
    float calcPenWidth(float lineWidth);
    GiContext& getContext(bool forChange);
    void setContext(const GiContext& ctx, int mask, int apply);
    void setContext(int mask);
    void setContextEditing(bool editing);
    int addImageShape(const char* name, float width, float height);
    int addImageShape(const char* name, float xc, float yc, float w, float h);
    bool getBoundingBox(mgvector<float>& box);
    bool getBoundingBox(mgvector<float>& box, int shapeId);

private:
    GiCoreViewImpl* impl;
};

#ifndef DOXYGEN
#include "gicontxt.h"
#endif

#endif // TOUCHVG_CORE_VIEWDISPATCHER_H
