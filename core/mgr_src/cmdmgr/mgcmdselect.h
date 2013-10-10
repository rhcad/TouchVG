//! \file mgcmdselect.h
//! \brief 定义选择命令类 MgCmdSelect
// Copyright (c) 2004-2013, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CMD_SELECT_H_
#define TOUCHVG_CMD_SELECT_H_

#include <mgcmd.h>
#include <mgselect.h>
#include <vector>

//! 选择命令类
/*! \ingroup CORE_COMMAND
*/
class MgCmdSelect : public MgCommand, public MgSelection
{
protected:
    MgCmdSelect();
    virtual ~MgCmdSelect();

public:
    static const char* Name() { return "select"; }
    static MgCommand* Create() { return new MgCmdSelect; }
    bool dynamicChangeEnded(MgView* view, bool apply);
    
public:
    virtual int getSelection(MgView* view, int count, MgShape** shapes, bool forChange = false);
    virtual MgSelState getSelectState(MgView* view);
    virtual int getSelectType(MgView* view);
    virtual bool selectAll(const MgMotion* sender);
    virtual bool deleteSelection(const MgMotion* sender);
    virtual bool cloneSelection(const MgMotion* sender);
    virtual bool groupSelection(const MgMotion* sender);
    virtual bool ungroupSelection(const MgMotion* sender);
    virtual void resetSelection(const MgMotion* sender);
    virtual bool addSelection(const MgMotion* sender, int shapeID);
    virtual bool deleteVertext(const MgMotion* sender);
    virtual bool insertVertext(const MgMotion* sender);
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
    
private:
    virtual const char* getName() const { return Name(); }
    virtual void release() { delete this; }

    virtual bool cancel(const MgMotion* sender);
    virtual bool initialize(const MgMotion* sender, MgStorage* s);
    virtual bool backStep(const MgMotion* sender);
    virtual bool draw(const MgMotion* sender, GiGraphics* gs);
    virtual int gatherShapes(const MgMotion* sender, MgShapes* shapes);
    virtual bool click(const MgMotion* sender);
    virtual bool doubleClick(const MgMotion* sender);
    virtual bool longPress(const MgMotion* sender);
    virtual bool touchBegan(const MgMotion* sender);
    virtual bool touchMoved(const MgMotion* sender);
    virtual bool touchEnded(const MgMotion* sender);
    virtual bool twoFingersMove(const MgMotion* sender);

private:
    int getStep() { return 0; }
    MgShape* hitTestAll(const MgMotion* sender, MgHitResult& res);
    MgShape* getSelectedShape(const MgMotion* sender);
    bool canSelect(MgShape* shape, const MgMotion* sender);
    int hitTestHandles(MgShape* shape, const Point2d& pointM, 
                         const MgMotion* sender, float tolmm = 10.f);
    bool isIntersectMode(const MgMotion* sender);
    Point2d snapPoint(const MgMotion* sender, const MgShape* shape);
    
    typedef std::vector<int>::iterator sel_iterator;
    sel_iterator getSelectedPostion(MgShape* shape);
    bool isSelected(MgShape* shape);
    MgShape* getShape(int id, const MgMotion* sender) const;
    bool isDragRectCorner(const MgMotion* sender, Matrix2d& mat);
    bool isCloneDrag(const MgMotion* sender);
    void cloneShapes(MgView* view);
    bool applyCloneShapes(MgView* view, bool apply, bool addNewShapes = false);
    bool canTransform(MgShape* shape, const MgMotion* sender);
    bool canRotate(MgShape* shape, const MgMotion* sender);
    
private:
    std::vector<int>        m_selIds;           // 选中的图形的ID
    std::vector<MgShape*>   m_clones;           // 选中图形的复制对象
    int                     m_id;               // 选中图形的ID
    MgHitResult             m_hit;              // 点中结果
    Point2d                 m_ptSnap;           // 捕捉点
    Point2d                 m_ptStart;          // touchBegan捕捉点
    int                     m_handleIndex;      // 点中的是哪个控制点
    int                     m_rotateHandle;     // 旋转中心控制点
    int                     m_boxHandle;        // 选中框的活动控制点序号
    bool                    m_editMode;         // 控制点修改模式
    bool                    m_insertPt;         // 是否可插入新点
    bool                    m_showSel;          // 是否亮显选中的图形
    bool                    m_boxsel;           // 是否开始框选
};

#endif // TOUCHVG_CMD_SELECT_H_
