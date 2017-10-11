//! \file mgcmd.h
//! \brief 定义绘图命令接口
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_H_
#define TOUCHVG_CMD_H_

#include "mgview.h"

#ifndef SWIG
struct GiTextWidthCallback;
#endif

//! 命令接口
/*! \ingroup CORE_COMMAND
    \interface MgCommand
    \see mgRegisterCommand
*/
class MgCommand
{
public:
    //! 构造函数，名称最长为30个字符
    MgCommand(const char* name) {
        unsigned i=0; for(;name[i] && i<sizeof(_name)-1;i++) {_name[i]=name[i];} _name[i]=0; }
    virtual ~MgCommand() {}
    
#ifndef SWIG
    const char* getName() const { return _name; }               //!< 返回命令名称
    virtual const MgShape* getShape(const MgMotion* sender) { return MgShape::Null(); }   //!< 当前临时图形
    virtual bool initializeWithSelection(const MgMotion* sender, MgStorage* s, const int* ids) { return initialize(sender, s); }
    virtual int getSelectedIDs(MgView* view, int* ids, int count) { return 0; }
    static float drawAngleText(const MgMotion* sender, GiGraphics* gs, float angle, void* stdstr = (void*)0);
    static float drawAngleText(MgView* view, GiGraphics* gs, float angle, const Point2d& pt,
                               int align = 1, void* stdstr = (void*)0, GiTextWidthCallback* c = (GiTextWidthCallback*)0);
#endif
    virtual void release() = 0;                                 //!< 销毁对象
    
    virtual bool cancel(const MgMotion* sender) { return !sender; } //!< 取消命令
    virtual bool initialize(const MgMotion* sender, MgStorage* s) { return sender || s; }    //!< 开始命令
    virtual bool backStep(const MgMotion* sender) { return !sender; }   //!< 回退一步
    
    virtual bool draw(const MgMotion* sender, GiGraphics* gs) = 0;  //!< 显示动态图形
    virtual bool gatherShapes(const MgMotion* sender, MgShapes* shapes) { //!< 得到动态图形
        return !sender && !shapes; }    // 实现且图完整则返回true，否则将用 draw() 生成临时图形
    
    virtual bool click(const MgMotion* sender) {    //!< 点击
        return sender->view->useFinger() && longPress(sender); }
    virtual bool doubleClick(const MgMotion* sender) { return !sender; } //!< 双击
    virtual bool longPress(const MgMotion* sender) { return !sender; }  //!< 长按
    virtual bool touchBegan(const MgMotion* sender) { return !sender; } //!< 开始滑动
    virtual bool touchMoved(const MgMotion* sender) { return !sender; } //!< 正在滑动
    virtual bool touchEnded(const MgMotion* sender) { return !sender; } //!< 滑动结束
    virtual bool mouseHover(const MgMotion* sender) { return !sender; } //!< 鼠标掠过
    virtual bool twoFingersMove(const MgMotion* sender) { return !sender; } //!< 双指触摸
    
    virtual bool isDrawingCommand() { return false; }       //!< 是否为绘图命令
    virtual bool isFloatingCommand() { return false; }      //!< 是否可嵌套在其他命令中
    virtual bool doContextAction(const MgMotion* sender, int action) {
        return !sender && !action; }                        //!< 执行上下文动作
    
private:
    char _name[31];
    MgCommand();
};

#endif // TOUCHVG_CMD_H_
