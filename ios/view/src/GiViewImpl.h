//! \file GiViewImpl.h
//! \brief 定义iOS绘图视图类的内部实现接口
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

#import "GiPaintView.h"
#include "GiCanvasAdapter.h"
#include "gicoreview.h"
#include <vector>
#import <QuartzCore/QuartzCore.h>

class GiViewAdapter;
@class GiMagnifierView;

//! 动态图形的绘图视图类
@interface IosTempView : UIView {
    GiViewAdapter   *_adapter;
}

- (id)initView:(CGRect)frame :(GiViewAdapter *)adapter;

@end

//! 在层上显示图形的渲染类
@interface GiLayerRender : NSObject {
    CALayer         *_layer;
    GiViewAdapter   *_adapter;
    __block long    _drawing;
    __block long    _doc;
    __block long    _gs;
    dispatch_queue_t _queue;
}

- (id)initWithAdapter:(GiViewAdapter *)adapter;
- (void)stopRender;
- (void)clearCachedData;
- (void)startRender:(long)doc :(long)gs;
- (void)startRenderForPending;
- (CALayer *)getLayer;

@end

int getTickCount();

//! iOS绘图视图适配器
class GiViewAdapter : public GiView
{
private:
    GiPaintView *_view;                 //!< 静态图形视图, GiPaintView
    UIView      *_dynview;              //!< 动态图形视图, IosTempView
    GiCoreView  *_coreView;             //!< 内核视图分发器
    NSMutableArray *_buttons;           //!< 上下文按钮的数组
    NSMutableDictionary *_buttonImages; //!< 按钮图像缓存
    ImageCache  *_imageCache;           //!< 图像对象缓存
    bool        _actionEnabled;         //!< 是否允许上下文操作
    int         _appendIDs[10];         //!< 还未来得及重构显示的新增图形的ID
    int         _oldAppendCount;        //!< 后台渲染前的待渲染新增图形数
    int         _regenCount;            //!< 渲染次数
    GiLayerRender   *_render;           //!< 后台渲染对象
    dispatch_queue_t _recordQueue[2];   //!< 录制队列
    __block bool    _recordStopping[2]; //!< 录制队列待停止
    
public:
    std::vector<id> delegates;          //!< GiPaintViewDelegate 观察者数组
    struct {
        unsigned int didCommandChanged:1;
        unsigned int didSelectionChanged:1;
        unsigned int didContentChanged:1;
        unsigned int didDynamicChanged:1;
    } respondsTo;
    
    GiViewAdapter(GiPaintView *mainView, GiViewAdapter *refView);
    virtual ~GiViewAdapter();
    
    GiCoreView *coreView() { return _coreView; }
    ImageCache *imageCache() { return _imageCache; }
    UIView *mainView() { return _view; }
    UIView *getDynView(bool autoCreate);
    void clearCachedData();
    void stopRegen();
    void onFirstRegen();
    bool isMainThread() const;
    long acquireFrontDoc();
    
    int getAppendCount() const;
    void beginRender();
    bool renderInContext(CGContextRef ctx);
    int getAppendID(int index) const;
    
    void undo();
    void redo();
    enum RecordType { kUndo, kRecord, kPlay };
    bool startRecord(NSString *path, RecordType type);
    void stopRecord(bool forUndo);
    
    virtual void regenAll(bool changed);
    virtual void regenAppend(int sid);
    virtual void redraw(bool changed);
    virtual bool isContextActionsVisible();
    virtual bool showContextActions(const mgvector<int>& actions,
                                    const mgvector<float>& buttonXY,
                                    float x, float y, float w, float h);
    void hideContextActions();
    void setContextActionEnabled(bool enabled) { _actionEnabled = enabled; }
    
    virtual void commandChanged();
    virtual void selectionChanged();
    virtual void contentChanged();
    virtual void dynamicChanged();
    
    bool dispatchGesture(GiGestureType type, GiGestureState state, CGPoint pt);
    bool dispatchPan(GiGestureState state, CGPoint pt, bool switchGesture = false);
    bool twoFingersMove(UIGestureRecognizer *sender, int state = -1, bool switchGesture = false);
    
private:
    void setContextButton(UIButton *btn, NSString *caption, NSString *imageName);
    void regen_(bool changed, int sid, bool loading = false);
    void redraw_(bool changed);
    void recordShapes(bool forUndo, long doc, long shapes);
};

/*! \category GiPaintView()
    \brief GiPaintView 的内部数据定义
 */
@interface GiPaintView()<UIGestureRecognizerDelegate> {
    GiViewAdapter   *_adapter;              //!< 视图回调适配器
    
    UIGestureRecognizer     *_recognizers[7];
    UIPanGestureRecognizer  *_panRecognizer;            //!< 拖动手势识别器
    UITapGestureRecognizer  *_tapRecognizer;            //!< 单指点击手势识别器
    UITapGestureRecognizer  *_twoTapsRecognizer;        //!< 单指双击手势识别器
    UILongPressGestureRecognizer *_pressRecognizer;     //!< 单指长按手势识别器
    UIPinchGestureRecognizer    *_pinchRecognizer;      //!< 双指放缩手势识别器
    UIRotationGestureRecognizer *_rotationRecognizer;   //!< 双指旋转手势识别器
    BOOL                    _gestureEnabled;            //!< 手势可用性
    GiMagnifierView         *_magnifierView;            //!< 手指跟随放大镜
    
    std::vector<CGPoint>    _points;        //!< 手势生效前的轨迹
    CGPoint                 _startPt;       //!< 开始位置
    CGPoint                 _lastPt;        //!< 上次位置
    CGPoint                 _tapPoint;      //!< 点击位置
    int                     _tapCount;      //!< 点击次数
    int                     _touchCount;    //!< 触点个数
    BOOL            _gestureRecognized;     //!< 识别出手势
    BOOL                    _buttonHandled;
    CGPoint                 _ignorePt;
}

@end

/*! \category GiPaintView(GestureInternel)
    \brief GiPaintView 的手势响应实现部分
 */
@interface GiPaintView(GestureInternel)

- (void)ignoreTouch:(CGPoint)pt :(UIView *)handledButton;
- (void)redrawForDelay:(id)changed;
- (void)onContextActionsDisplay:(NSMutableArray *)buttons;

@end
