//! \file GiShapeAdapter.h
//! \brief 定义输出 UIBezierPath 的画布适配器类 GiShapeAdapter
// Copyright (c) 2013-2015, https://github.com/rhcad/vgios, BSD License

#import <UIKit/UIKit.h>
#import "GiCanvasAdapter.h"

//! GiShapeAdapter输出路径的回调接口
struct GiShapeAdapterCallback
{
    virtual void addPath(UIBezierPath *path, UIColor *strokeColor, UIColor *fillColor) = 0;
    virtual bool beginShape(int type, int sid, CGRect frame) = 0;
    virtual void endShape(int type, int sid) = 0;
};

//! GiShapeAdapterCallback 的默认实现，转换为层
/*! \ingroup GROUP_IOS
    本类可将矢量图形转换为三级层，第二级为每个图形的层，其下有每个路径的CAShapeLayer。
    如果构造时没有传入顶级层，则转换为二级层，第二级为每个路径的CAShapeLayer，不按图形分组。
 */
class GiShapeLayerCallback : public GiShapeAdapterCallback
{
public:
    GiShapeLayerCallback(CALayer *rootLayer, bool hidden);
    CALayer *layer() { return _rootLayer ? _rootLayer : _shapeLayer; }
    
private:
    virtual void addPath(UIBezierPath *path, UIColor *strokeColor, UIColor *fillColor);
    virtual bool beginShape(int type, int sid, CGRect frame);
    virtual void endShape(int type, int sid);
    
    CALayer *_rootLayer;
    CALayer *_shapeLayer;
    bool    _hidden;
};

//! 输出 UIBezierPath 的画布适配器类
/*! \ingroup GROUP_IOS
 */
class GiShapeAdapter : public GiCanvas
{
public:
    GiShapeAdapter(GiShapeAdapterCallback* shapeCallback);
    void endOutput();
    
public:
    virtual void setPen(int argb, float width, int style, float phase, float orgw);
    virtual void setBrush(int argb, int style);
    virtual void clearRect(float x, float y, float w, float h);
    virtual void drawRect(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void drawLine(float x1, float y1, float x2, float y2);
    virtual void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill);
    virtual void beginPath();
    virtual void moveTo(float x, float y);
    virtual void lineTo(float x, float y);
    virtual void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y);
    virtual void quadTo(float cpx, float cpy, float x, float y);
    virtual void closePath();
    virtual void drawPath(bool stroke, bool fill);
    virtual void saveClip();
    virtual void restoreClip();
    virtual bool clipRect(float x, float y, float w, float h);
    virtual bool clipPath();
    virtual bool drawHandle(float x, float y, int type, float angle);
    virtual bool drawBitmap(const char* name, float xc, float yc, float w, float h, float angle);
    virtual float drawTextAt(const char* text, float x, float y, float h, int align, float angle);
    virtual bool beginShape(int type, int sid, int version, float x, float y, float w, float h);
    virtual void endShape(int type, int sid, float x, float y);
    
private:
    bool hasLineColor() const;
    bool hasFillColor() const;
    void fireLastPath();
    void checkNeedFire(bool stroke, bool fill);
    
private:
    GiShapeAdapterCallback* _callback;
    UIBezierPath    *_container;
    UIBezierPath    *_path;
    UIColor         *_lineColor;
    UIColor         *_fillColor;
};
