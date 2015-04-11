//! \file mgselect.h
//! \brief 定义选择集接口 MgSelection
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_MGSELECTION_H_
#define TOUCHVG_MGSELECTION_H_

struct MgView;
class MgMotion;
class MgShape;
class Point2d;
class Matrix2d;

//! 选择状态
typedef enum {
    kMgSelNone,         //!< 没有选择图形
    kMgSelOneShape,     //!< 选择了一个图形
    kMgSelMultiShapes,  //!< 选择了多个图形
    kMgSelVertexes,     //!< 显示图形的多个节点
    kMgSelVertex,       //!< 选择了一个图形的节点
    kMgSelDraw,         //!< 在绘图命令中
} MgSelState;

//! 显示开关，用于"selectDrawFlags"配置项
typedef enum {
    kMgSelDrawDragLine      = 1<<1,     //!< 拖动提示的参考线
    kMgSelDrawDragDot       = 1<<2,     //!< 拖动的终点
    kMgSelDrawGroupBorder   = 1<<3,     //!< 成组图形的外框
    kMgSelDrawActionBorder  = 1<<4,     //!< 上下文按钮包络框
    kMgSelDrawSelBorder     = 1<<5,     //!< 选择集包络框
    kMgSelDrawBlueShape     = 1<<6,     //!< 蓝色显示选中的图形
    kMgSelDrawHandle        = 1<<7,     //!< 控制点
    
    kMgSelDrawXformBox      = 1<<16,    //!< 矩形变形框
    kMgSelDrawNearPt        = 1<<17,    //!< 最近点
} MgSelDrawFlags;

//! 选择集接口
/*! \ingroup CORE_COMMAND
    \interface MgSelection
    \see MgSelState
*/
struct MgSelection {
    virtual ~MgSelection() {}
#ifndef SWIG
    //! 得到当前选择的图形
    virtual int getSelection(MgView* view, int count, const MgShape** shapes) = 0;
    //! 得到当前选择的图形，用于修改
    virtual int getSelectionForChange(MgView* view, int count, MgShape** shapes) = 0;
#endif
#ifdef SWIG_MGVECTOR_H
    //! 得到当前选择的图形
    int getSelection(MgView* view, mgvector<const MgShape*>& shapes) {
        shapes.setSize(getSelectionCount(view));
        return getSelection(view, shapes.count(), shapes.address());
    }
    //! 得到当前选择的图形，用于修改
    int getSelectionForChange(MgView* view, mgvector<MgShape*>& shapes) {
        shapes.setSize(getSelectionCount(view));
        return getSelectionForChange(view, shapes.count(), shapes.address());
    }
#endif
    //! 得到选中的图形的个数
    int getSelectionCount(MgView* view) { return getSelection(view, 0, (const MgShape**)0); }
    
    //! 返回选择状态
    virtual MgSelState getSelectState(MgView* view) = 0;

    //! 返回选中的图形的类型, MgShapeType
    virtual int getSelectType(MgView* view) = 0;
    
    //! 返回当前控制点序号
    virtual int getSelectedHandle(const MgMotion* sender) = 0;
    
    //! 返回当前图形的句柄
    virtual long getSelectedShapeHandle(const MgMotion* sender) = 0;
    
    //! 选中所有图形
    virtual bool selectAll(const MgMotion* sender) = 0;
    
    //! 删除选中的图形
    virtual bool deleteSelection(const MgMotion* sender) = 0;
    
    //! 对选中的图形复制出新图形，并选中新图形
    virtual bool cloneSelection(const MgMotion* sender) = 0;

    //! 对选中的图形成组，并选中新成组图形
    virtual bool groupSelection(const MgMotion* sender) = 0;

    //! 对选中的成组图形解组
    virtual bool ungroupSelection(const MgMotion* sender) = 0;
    
    //! 清除选择集
    virtual void resetSelection(const MgMotion* sender) = 0;

    //! 添加选择图形
    virtual bool addSelection(const MgMotion* sender, int shapeID) = 0;
    
    //! 删除当前图形的选中的顶点
    virtual bool deleteVertex(const MgMotion* sender) = 0;
    
    //! 在当前位置插入一个顶点
    virtual bool insertVertex(const MgMotion* sender) = 0;
    
    //! 切换当前图形是否闭合
    virtual bool switchClosed(const MgMotion* sender) = 0;
    
    //! 返回当前选中图形是否固定边长
    virtual bool isFixedLength(MgView* view) = 0;
    
    //! 设置当前选中图形是否固定边长
    virtual bool setFixedLength(const MgMotion* sender, bool fixed) = 0;
    
    //! 返回当前选中图形是否锁定形状
    virtual bool isLocked(MgView* view) = 0;
    
    //! 设置当前选中图形是否锁定形状
    virtual bool setLocked(const MgMotion* sender, bool locked) = 0;
    
    //! 返回当前是否处于顶点编辑状态
    virtual bool isEditMode(MgView* view) = 0;
    
    //! 设置当前是否处于顶点编辑状态
    virtual bool setEditMode(const MgMotion* sender, bool editMode) = 0;
    
    //! 对当前选中多边形沿一条边翻转
    virtual bool overturnPolygon(const MgMotion* sender) = 0;
    
    //! 返回选择包络框，模型坐标
    virtual Box2d getBoundingBox(const MgMotion* sender) = 0;

    //! 判断当前选择的图形是否从指定类型派生
    virtual bool isSelectedByType(MgView* view, int type) = 0;
    
    //! 对当前选中的图形进行几何变形
    virtual bool applyTransform(const MgMotion* sender, const Matrix2d& xf) = 0;
};

#endif // TOUCHVG_MGSELECTION_H_
