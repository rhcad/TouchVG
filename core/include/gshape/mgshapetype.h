//! \file mgshapetype.h
//! \brief 定义图形类型号 MgShapeType
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_SHAPETYPE_H_
#define TOUCHVG_SHAPETYPE_H_

//! 基本图形的类型号
/*! \ingroup CORE_SHAPE
    1-40为内置图形类型，41以上为其他应用可用的类型号
*/
typedef enum {
    kMgShapeMultiType = 9999,   //!< 混合图形类型
    kMgShapeNone = 0,           //!< 没有图形
    kMgShapeList = 1,           //!< 图形列表, MgShapes
    kMgShapeWrapper = 2,        //!< 封装图形, MgShape
    kMgShapeBase = 3,           //!< 图形基类, MgBaseShape
    kMgShapeBaseRect = 4,       //!< 矩形基类, MgBaseRect
    kMgShapeBaseLines = 5,      //!< 折线基类, MgBaseLines
    kMgShapeComposite = 6,      //!< 复合图形, MgComposite
    kMgShapeLayer = 7,          //!< 图层, MgLayer
    kMgShapeDoc = 8,            //!< 图形文档, MgShapeDoc

    kMgShapeGroup = 9,          //!< 成组图形, MgGroup
    kMgShapeLine = 10,          //!< 线段, MgLine
    kMgShapeRect = 11,          //!< 矩形、正方形, MgRect
    kMgShapeEllipse = 12,       //!< 椭圆、圆, MgEllipse
    kMgShapeRoundRect = 13,     //!< 圆角矩形, MgRoundRect
    kMgShapeDiamond = 14,       //!< 菱形, MgDiamond
    kMgShapeLines = 15,         //!< 折线、多边形, MgLines
    kMgShapeSplines = 16,       //!< 二次样条曲线类, MgSplines
    kMgShapeParallel = 17,      //!< 平行四边形, MgParallel
    kMgShapeImage = 18,         //!< 图像矩形, MgImageShape
    kMgShapeArc = 19,           //!< 圆弧, MgArc
    kMgShapeGrid = 20,          //!< 网格, MgGrid
    
    kMgShapeRecord = 30,        //!< 录制, MgRecordShape
    kMgShapeDot = 31,           //!< 点, MgDot
    kMgShapePath = 32,          //!< 路径, MgPathShape
} MgShapeType;

#endif // TOUCHVG_SHAPETYPE_H_
