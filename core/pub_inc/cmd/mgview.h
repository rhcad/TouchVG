//! \file mgview.h
//! \brief 定义视图回调接口 MgView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMDVIEW_H
#define TOUCHVG_CMDVIEW_H

#include "mgshapes.h"

class MgMotion;
struct MgCmdManager;
class MgCommand;
struct MgSnap;
struct MgActionDispatcher;
struct MgLockData;
struct CmdSubject;
struct MgSelection;
class GcShapeDoc;
class MgShapeDoc;
class GiTransform;
class GiGraphics;

typedef enum {                  //!< 手势状态
    kMgGesturePossible,         //!< 待检查手势有效性
    kMgGestureBegan,            //!< 开始
    kMgGestureMoved,            //!< 改变
    kMgGestureEnded,            //!< 结束
    kMgGestureCancel,           //!< 取消
} MgGestureState;

//! 视图回调接口
/*! \ingroup GROUP_COMMAND
    \see mgRegisterCommand(), MgShapeT<T>::registerCreator()
 */
class MgView
{
public:
    virtual ~MgView() {}
    static MgView* fromHandle(long h) { MgView* p; *(long*)&p = h; return p; } //!< 句柄转为对象
    long toHandle() { long h; *(MgView**)&h = this; return h; }       //!< 得到句柄，用于跨库转换
    
#ifndef SWIG
    virtual GcShapeDoc* document() const = 0;
#endif
    virtual MgMotion* motion() = 0;                 //!< 返回当前动作参数
    virtual MgCmdManager* cmds() const = 0;         //!< 返回命令管理器对象
    virtual GiTransform* xform() const = 0;         //!< 得到坐标系对象
    virtual GiGraphics* graph() const = 0;          //!< 得到图形显示对象
    virtual MgShapeDoc* doc() const = 0;            //!< 得到图形文档
    virtual MgShapes* shapes() const = 0;           //!< 得到图形列表
    virtual GiContext* context() const = 0;         //!< 得到当前绘图属性
    virtual Matrix2d& modelTransform() const = 0;   //!< 文档的模型变换矩阵

    virtual MgShapeFactory* getShapeFactory() = 0;  //!< 返回图形工厂对象
    virtual MgSnap* getSnap() = 0;                  //!< 返回图形特征点捕捉器
    virtual MgActionDispatcher* getAction() = 0;    //!< 返回上下文动作分发对象
    virtual MgLockData* getLockData() = 0;          //!< 返回图形文档锁定对象
    virtual CmdSubject* getCmdSubject() = 0;        //!< 返回命令扩展目标对象
    virtual MgSelection* getSelection() = 0;        //!< 返回选择集对象，可能为NULL

    virtual bool setCurrentShapes(MgShapes* shapes) = 0; //!< 设置当前图形列表
    virtual bool toSelectCommand(const MgMotion* sender) = 0; //!< 取消当前命令
    virtual int getNewShapeID() = 0;                //!< 返回新绘图形的ID
    virtual void setNewShapeID(int sid) = 0;        //!< 设置新绘图形的ID
    virtual const char* getCommandName() = 0;       //!< 得到当前命令名称
    virtual MgCommand* getCommand() = 0;                    //!< 得到当前命令
    virtual MgCommand* findCommand(const char* name) = 0;   //!< 查找命令
    virtual bool setCommand(const MgMotion* sender, const char* name) = 0;  //!< 启动命令
    virtual bool isReadOnly() const = 0;            //! 返回文档是否只读

    virtual void regenAll() = 0;                //!< 标记视图待重新构建显示
    virtual void regenAppend() = 0;             //!< 标记视图待追加显示新图形
    virtual void redraw() = 0;                  //!< 标记视图待更新显示
    
    virtual bool useFinger() = 0;               //!< 使用手指或鼠标交互
    virtual void commandChanged() = 0;          //!< 命令改变
    virtual void selectionChanged() = 0;        //!< 选择集改变的通知
    virtual void dynamicChanged() = 0;          //!< 图形动态改变的通知
    
    virtual bool shapeWillAdded(MgShape* shape) = 0;    //!< 通知将添加图形
    virtual void shapeAdded(MgShape* shape) = 0;        //!< 通知已添加图形，由视图重新构建显示
    virtual bool shapeWillDeleted(MgShape* shape) = 0;  //!< 通知将删除图形
    virtual bool removeShape(MgShape* shape) = 0;       //!< 删除图形
    virtual bool shapeCanRotated(const MgShape* shape) = 0;   //!< 通知是否能旋转图形
    virtual bool shapeCanTransform(const MgShape* shape) = 0; //!< 通知是否能对图形变形
    virtual bool shapeCanUnlock(const MgShape* shape) = 0;    //!< 通知是否能对图形解锁
    virtual bool shapeCanUngroup(const MgShape* shape) = 0;   //!< 通知是否能对成组图形解散
    virtual void shapeMoved(MgShape* shape, int segment) = 0;   //!< 通知图形已拖动
    
    virtual bool isContextActionsVisible() = 0;         //!< 返回上下文菜单是否已显示
#ifndef SWIG
    virtual bool showContextActions(int selState, const int* actions,
            const Box2d& selbox, const MgShape* shape) = 0; //!< 显示上下文菜单
    virtual bool registerCommand(const char* name, MgCommand* (*creator)()) = 0; //!< 注册命令
#endif
};

//! 触摸动作参数
/*! \ingroup GROUP_COMMAND
 */
class MgMotion
{
public:
    MgView*         view;
    int             gestureType;
    MgGestureState  gestureState;
    bool            pressDrag;
    bool            switchGesture;
    Point2d         startPt;
    Point2d         startPtM;
    Point2d         lastPt;
    Point2d         lastPtM;
    Point2d         point;
    Point2d         pointM;
    Point2d         startPt2;
    Point2d         startPt2M;
    Point2d         point2;
    Point2d         point2M;
    float           d2mgs;
    float           d2m;
    
    MgMotion() : view(NULL), gestureType(0), gestureState(kMgGesturePossible)
        , pressDrag(false), switchGesture(false), d2mgs(0), d2m(0) {}
    
    bool dragging() const {                             //!< 是否正按下拖动
        return gestureState >= kMgGestureBegan && gestureState <= kMgGestureMoved;
    }
    
    MgCmdManager* cmds() const { return view->cmds(); } //!< 返回命令管理器对象
    bool toSelectCommand() const { return view->toSelectCommand(this); }  //!< 取消当前命令

    Point2d startCenterM() const { return (startPtM + startPt2M) / 2; }
    Point2d centerM() const { return (pointM + point2M) / 2; }
    float startDistanceM() const { return startPtM.distanceTo(startPt2M); }
    float distanceM() const { return pointM.distanceTo(point2M); }

    //! 返回屏幕毫米长度对应的模型长度，在命令显示函数中使用
    float displayMmToModel(float mm, GiGraphics*) const { return d2mgs * mm; }
    //! 返回屏幕毫米长度对应的模型长度
    float displayMmToModel(float mm) const { return d2m * mm; }
};

#ifndef SWIG
//! 注册外部命令, 为NULL则取消注册
/*! \ingroup CORE_COMMAND
*/
template <class YourCmd> inline int mgRegisterCommand(MgView* view) {
    return view->registerCommand(YourCmd::Name(), YourCmd::Create) ? 1 : 0;
}
#endif

#endif // TOUCHVG_CMDVIEW_H
