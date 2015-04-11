//! \file mgcmdselect.h
//! \brief 定义选择命令类 MgCmdSelect
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CMD_SELECT_H_
#define TOUCHVG_CMD_SELECT_H_

#include "mgcmd.h"
#include "mgselect.h"
#include <vector>

//! 选择命令类
/*! \ingroup CORE_COMMAND
*/
class MgCmdSelect : public MgCommand, public MgSelection
{
public:
    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCmdSelect; }
    bool dynamicChangeEnded(MgView* view, bool apply);
    bool applyTransform(const MgMotion* sender, MgStorage* s);
    
public:
    virtual int getSelection(MgView* view, int count, const MgShape** shapes);
    virtual int getSelectionForChange(MgView* view, int count, MgShape** shapes);
    virtual MgSelState getSelectState(MgView* view);
    virtual int getSelectType(MgView* view);
    virtual int getSelectedHandle(const MgMotion* sender);
    virtual long getSelectedShapeHandle(const MgMotion* sender);
    virtual bool selectAll(const MgMotion* sender);
    virtual bool deleteSelection(const MgMotion* sender);
    virtual bool cloneSelection(const MgMotion* sender);
    virtual bool groupSelection(const MgMotion* sender);
    virtual bool ungroupSelection(const MgMotion* sender);
    virtual void resetSelection(const MgMotion* sender);
    virtual bool addSelection(const MgMotion* sender, int shapeID);
    virtual bool deleteVertex(const MgMotion* sender);
    virtual bool insertVertex(const MgMotion* sender);
    virtual bool switchClosed(const MgMotion* sender);
    virtual bool isFixedLength(MgView* view);
    virtual bool setFixedLength(const MgMotion* sender, bool fixed);
    virtual bool isLocked(MgView* view);
    virtual bool setLocked(const MgMotion* sender, bool locked);
    virtual bool isEditMode(MgView* view);
    virtual bool setEditMode(const MgMotion* sender, bool editMode);
    virtual bool overturnPolygon(const MgMotion* sender);
    virtual Box2d getBoundingBox(const MgMotion* sender);
    virtual bool isSelectedByType(MgView* view, int type);
    virtual bool applyTransform(const MgMotion* sender, const Matrix2d& xf);
    
private:
    MgCmdSelect();
    virtual void release() { delete this; }
    virtual bool cancel(const MgMotion* sender);
    virtual int getSelectedIDs(MgView* view, int* ids, int count);
    virtual bool initializeWithSelection(const MgMotion* sender, MgStorage* s, const int* ids);
    virtual bool backStep(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool twoFingersMove(const MgMotion* sender);
    virtual const MgShape* getShape(const MgMotion* sender);

private:
    int getStep() { return 0; }
    const MgShape* hitTestAll(const MgMotion* sender, MgHitResult& res);
    const MgShape* getSelectedShape(const MgMotion* sender);
    bool canSelect(const MgShape* shape, const MgMotion* sender);
    int hitTestHandles(const MgShape* shape, const Point2d& pointM,
                         const MgMotion* sender, float tolmm = 10.f);
    bool isIntersectMode(const MgMotion* sender);
    int getLockSelShape(const MgMotion* sender, int defValue) const;
    int getLockSelHandle(const MgMotion* sender, int defValue) const;
    int getLockRotateHandle(const MgMotion* sender, int defValue) const;
    Point2d snapPoint(const MgMotion* sender, const MgShape* shape);
    
    typedef std::vector<int>::iterator sel_iterator;
    sel_iterator getSelectedPostion(const MgShape* shape);
    bool isSelected(const MgShape* shape);
    const MgShape* getShape(int id, const MgMotion* sender) const;
    Box2d _getBoundingBox(const MgMotion* sender);
    bool isDragRectCorner(const MgMotion* sender, Matrix2d& mat);
    bool isCloneDrag(const MgMotion* sender);
    void cloneShapes(MgView* view);
    bool applyCloneShapes(MgView* view, bool apply, bool addNewShapes = false);
    bool canTransform(const MgShape* shape, const MgMotion* sender);
    bool canRotate(const MgShape* shape, const MgMotion* sender);
    void selectionChanged(MgView* view);
    
private:
    std::vector<int>        m_selIds;           // 选中的图形的ID
    std::vector<MgShape*>   m_clones;           // 选中图形的复制对象
    int                     m_id;               // 选中图形的ID
    MgHitResult             m_hit;              // 点中结果
    Point2d                 m_ptSnap;           // 捕捉点
    Point2d                 m_ptStart;          // touchBegan捕捉点
    int                     m_handleIndex;      // 点中的是哪个控制点
    int                     m_rotateHandle;     // 旋转中心控制点
    float                   m_rotateAngle;      // 旋转角度
    int                     m_boxHandle;        // 选中框的活动控制点序号
    bool                    m_editMode;         // 控制点修改模式
    bool                    m_insertPt;         // 是否可插入新点
    bool                    m_showSel;          // 是否亮显选中的图形
    bool                    m_boxsel;           // 是否开始框选
    bool                    m_dragging;         // 是否正在拖动
    bool                    m_canRotateHandle;  // 是否允许绕控制点旋转
    bool                    m_shapeEdited;      // 图形可自定义编辑
};

#endif // TOUCHVG_CMD_SELECT_H_
