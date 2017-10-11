//! \file mgview.h
//! \brief 定义视图回调接口 MgView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMDVIEW_H
#define TOUCHVG_CMDVIEW_H

#include "mgshapes.h"

class MgMotion;
struct MgCmdManager;
class MgCommand;
struct MgSnap;
struct MgActionDispatcher;
struct CmdSubject;
struct MgSelection;
class GcShapeDoc;
class MgShapeDoc;
class GiTransform;
class GiGraphics;
struct MgStringCallback;

typedef enum {                  //!< 手势状态
    kMgGesturePossible,         //!< 待检查手势有效性
    kMgGestureBegan,            //!< 开始
    kMgGestureMoved,            //!< 改变
    kMgGestureEnded,            //!< 结束
    kMgGestureCancel,           //!< 取消
} MgGestureState;

//! 视图回调接口，供命令使用
/*! \ingroup GROUP_COMMAND
    \see mgRegisterCommand(), MgShapeT<T>::registerCreator()
    \interface MgView
 */
struct MgView
{
    virtual ~MgView() {}
    static MgView* fromHandle(long h) { MgView* p; *(long*)&p = h; return p; } //!< 句柄转为对象
    long toHandle() const { long h; *(const MgView**)&h = this; return h; }       //!< 得到句柄，用于跨库转换
    
#ifndef SWIG
    virtual GcShapeDoc* document() const = 0;                   //!< 返回内核的内部文档对象
    virtual void* createRegenLocker() = 0;                      //!< 为 MgRegenLocker 创建内部对象
#endif
    virtual MgMotion* motion() = 0;                             //!< 返回当前动作参数
    virtual MgCmdManager* cmds() const = 0;                     //!< 返回命令管理器对象
    virtual GiTransform* xform() const = 0;                     //!< 得到坐标系对象
    virtual MgShapeDoc* doc() const = 0;                        //!< 得到可编辑的图形文档
    virtual MgShapes* shapes() const = 0;                       //!< 得到可编辑的图形列表
    virtual GiContext* context() const = 0;                     //!< 得到当前绘图属性
    virtual Matrix2d& modelTransform() const = 0;               //!< 文档的模型变换矩阵

    virtual MgShapeFactory* getShapeFactory() = 0;              //!< 返回图形工厂对象
    virtual MgShape* createShapeCtx(int type, const GiContext* ctx = (const GiContext*)0) = 0; //!< 根据类型号创建图形对象
    virtual MgSnap* getSnap() = 0;                              //!< 返回图形特征点捕捉器
    virtual MgActionDispatcher* getAction() = 0;                //!< 返回上下文动作分发对象
    virtual CmdSubject* getCmdSubject() = 0;                    //!< 返回命令扩展目标对象
    virtual MgSelection* getSelection() = 0;                    //!< 返回选择集对象，可能为NULL

    virtual bool setCurrentShapes(MgShapes* shapes) = 0;        //!< 设置当前图形列表
    virtual bool toSelectCommand() = 0;                         //!< 取消当前命令
    virtual int getNewShapeID() = 0;                            //!< 返回新绘图形的ID
    virtual void setNewShapeID(int sid) = 0;                    //!< 设置新绘图形的ID
    virtual MgCommand* getCommand() = 0;                        //!< 得到当前命令
    virtual MgCommand* findCommand(const char* name) = 0;       //!< 查找命令
    virtual bool setCommand(const char* name, const char* params = "") = 0; //!< 启动命令
    virtual bool isReadOnly() const = 0;                        //!< 返回文档是否只读
    virtual bool isCommand(const char* name) = 0;               //!< 当前是否为指定名称的命令
    
    virtual void regenAll(bool changed) = 0;                    //!< 标记视图待重新构建显示
    virtual void regenAppend(int sid, long playh = 0) = 0;      //!< 标记视图待追加显示新图形
    virtual void redraw(bool changed = true) = 0;               //!< 标记视图待更新显示
    
    virtual bool useFinger() = 0;                               //!< 使用手指或鼠标交互
    virtual void commandChanged() = 0;                          //!< 命令改变
    virtual void selectionChanged() = 0;                        //!< 选择集改变的通知
    virtual void dynamicChanged() = 0;                          //!< 图形动态拖拉改变的通知
    
    virtual bool shapeWillAdded(MgShape* shape) = 0;            //!< 通知将添加图形
    virtual void shapeAdded(MgShape* shape) = 0;                //!< 通知已添加图形，由视图重新构建显示
    virtual bool shapeWillDeleted(const MgShape* shape) = 0;    //!< 通知将删除图形
    virtual int removeShape(const MgShape* shape) = 0;          //!< 删除图形
    virtual bool shapeCanRotated(const MgShape* shape) = 0;     //!< 通知是否能旋转图形
    virtual bool shapeCanTransform(const MgShape* shape) = 0;   //!< 通知是否能对图形变形
    virtual bool shapeCanUnlock(const MgShape* shape) = 0;      //!< 通知是否能对图形解锁
    virtual bool shapeCanUngroup(const MgShape* shape) = 0;     //!< 通知是否能对成组图形解散
    virtual bool shapeCanMovedHandle(const MgShape* shape, int index) = 0;  //!< 通知是否能移动点
    virtual void shapeMoved(MgShape* shape, int segment) = 0;   //!< 通知图形已拖动
    virtual bool shapeWillChanged(MgShape* shape, const MgShape* oldsp) = 0; //!< 通知将修改图形
    virtual void shapeChanged(MgShape* shape) = 0;              //!< 通知已拖动图形
    virtual bool shapeDblClick(const MgShape* shape) = 0;       //!< 通知图形双击编辑
    
    //! 图形点击的通知，返回false继续显示上下文按钮
    virtual bool shapeClicked(const MgShape* shape, float x, float y) = 0;
    virtual void showMessage(const char* text) = 0;             //!< 显示提示文字
    //! 得到本地化文字内容(可用封装函数 MgLocalized::getString)
    virtual void getLocalizedString(const char* name, MgStringCallback* c) = 0;
    
    virtual bool isContextActionsVisible() = 0;                 //!< 返回上下文菜单是否已显示
    virtual void hideContextActions() = 0;                      //!< 隐藏上下文操作菜单
#ifndef SWIG
    virtual bool showContextActions(int selState, const int* actions,
            const Box2d& selbox, const MgShape* shape) = 0;     //!< 显示上下文菜单(MgContextAction)
    virtual bool registerCommand(const char* name, MgCommand* (*creator)()) = 0; //!< 注册命令
    virtual const char* getCommandName() = 0;                   //!< 得到当前命令名称
    virtual const char* getOptionString(const char* name) = 0;  //!< 文本选项值
#endif
    
    virtual bool getOptionBool(const char* name, bool defValue) = 0;     //!< 布尔选项值
    virtual int getOptionInt(const char* name, int defValue) = 0;        //!< 整型选项值
    virtual float getOptionFloat(const char* name, float defValue) = 0;  //!< 浮点型选项值
    virtual void setOptionBool(const char* name, bool value) = 0;        //!< 设置布尔选项值
    virtual void setOptionInt(const char* name, int value) = 0;          //!< 设置整型选项值
    virtual void setOptionFloat(const char* name, float value) = 0;      //!< 设置浮点型选项值
    virtual void setOptionString(const char* name, const char* text) = 0; //!< 设置文本选项值
};

//! 触摸动作参数
/*! \ingroup GROUP_COMMAND
 */
class MgMotion
{
public:
    MgView*         view;               //!< 视图回调对象
    int             gestureType;        //!< 当前手势类型, GiGestureType
    MgGestureState  gestureState;       //!< 当前手势状态
    bool            pressDrag;          //!< 是否正在长按并拖动：长按手势结束前的状态
    bool            switchGesture;      //!< 是否处于单指手势和双指手势切换之间
    Vector2d        velocity;           //!< 移动速度，每秒点数
    Point2d         startPt;            //!< 按下的位置，显示坐标
    Point2d         startPtM;           //!< 按下的位置，模型坐标
    Point2d         lastPt;             //!< 移动中上一次的位置，显示坐标
    Point2d         lastPtM;            //!< 移动中上一次的位置，模型坐标
    Point2d         point;              //!< 当前位置，显示坐标
    Point2d         pointM;             //!< 当前位置，模型坐标
    Point2d         startPt2;           //!< 双指按下的第二个手指位置，显示坐标
    Point2d         startPt2M;          //!< 双指按下的第二个手指位置，模型坐标
    Point2d         point2;             //!< 第二个手指的当前位置，显示坐标
    Point2d         point2M;            //!< 第二个手指的当前位置，模型坐标
    float           d2mgs;              //!< for displayMmToModel()
    float           d2m;                //!< for displayMmToModel()
    
    MgMotion() : view((MgView*)0), gestureType(0), gestureState(kMgGesturePossible)
        , pressDrag(false), switchGesture(false), d2mgs(0), d2m(0) {}
    
    bool dragging() const {                             //!< 是否正按下拖动
        return gestureState >= kMgGestureBegan && gestureState <= kMgGestureMoved;
    }
    
    MgCmdManager* cmds() const { return view->cmds(); } //!< 返回命令管理器对象

    //! 双指起始位置的中心点，模型坐标
    Point2d startCenterM() const { return (startPtM + startPt2M) / 2; }
    //! 双指中心点，模型坐标
    Point2d centerM() const { return (pointM + point2M) / 2; }
    //! 双指起始位置的距离，模型坐标
    float startDistanceM() const { return startPtM.distanceTo(startPt2M); }
    //! 双指距离，模型坐标
    float distanceM() const { return pointM.distanceTo(point2M); }

    //! 返回屏幕毫米长度对应的模型长度，在命令显示函数中使用
    float displayMmToModel(float mm, GiGraphics*) const { return d2mgs * mm; }
    //! 返回屏幕毫米长度对应的模型长度
    float displayMmToModel(float mm) const { return d2m * mm; }
    //! 返回屏幕毫米长度对应的模型长度，优先取配置值
    float displayMmToModel(const char* name, float mm) const {
        return d2m * view->getOptionFloat(name, mm);
    }
    //! 返回屏幕毫米宽度的正方形区域
    Box2d displayMmToModelBox(float mm) const {
        return Box2d(pointM, displayMmToModel(mm), 0);
    }
    //! 返回屏幕毫米宽度的正方形区域，优先取配置值
    Box2d displayMmToModelBox(const char* name, float mm) const {
        return Box2d(pointM, displayMmToModel(name, mm), 0);
    }
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
