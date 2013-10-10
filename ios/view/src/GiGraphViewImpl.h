//! \file GiGraphViewImpl.h
//! \brief 定义iOS绘图视图类的内部实现接口
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#import "GiGraphView.h"
#include "GiCanvasAdapter.h"
#include "gicoreview.h"
#include <vector>

class GiViewAdapter;

//! 动态图形的绘图视图类
/*! \class IosTempView
 */
@interface IosTempView : UIView {
    GiViewAdapter   *_adapter;
}

- (id)initView:(CGRect)frame :(GiViewAdapter *)adapter;

@end

//! iOS绘图视图适配器
class GiViewAdapter : public GiView
{
private:
    UIView      *_view;         //!< 静态图形视图, GiGraphView
    UIView      *_dynview;      //!< 动态图形视图, IosTempView
    GiCoreView  *_coreView;     //!< 内核视图分发器
    UIImage     *_tmpshot;      //!< 用于增量绘图的临时快照
    long        _drawCount;     //!< 用于增量绘图的计数
    NSMutableArray *_buttons;           //!< 上下文按钮的数组
    NSMutableDictionary *_buttonImages; //!< 按钮图像缓存
    
public:
    std::vector<id> delegates;  //!< GiGraphViewDelegate 观察者数组
    struct {
        unsigned int didCommandChanged:1;
        unsigned int didSelectionChanged:1;
        unsigned int didContentChanged:1;
    } respondsTo;
    
    GiViewAdapter(UIView *mainView, GiCoreView *coreView);
    virtual ~GiViewAdapter();
    
    GiCoreView *coreView() { return _coreView; }
    UIImage *snapshot(bool autoDraw);
    bool drawAppend(GiCanvas* canvas);
    void clearCachedData();
    
    virtual void regenAll();
    virtual void regenAppend();
    virtual void redraw();
    virtual bool isContextActionsVisible();
    virtual bool showContextActions(const mgvector<int>& actions,
                                    const mgvector<float>& buttonXY,
                                    float x, float y, float w, float h);
    void hideContextActions();
    
    virtual void commandChanged();
    virtual void selectionChanged();
    virtual void contentChanged();
    
    bool dispatchGesture(GiGestureType gestureType, GiGestureState gestureState, CGPoint pt);
    bool dispatchPan(GiGestureState gestureState, CGPoint pt, bool switchGesture = false);
    bool twoFingersMove(UIGestureRecognizer *sender, int state = -1, bool switchGesture = false);
    
private:
    void setContextButton(UIButton *btn, NSString *caption, NSString *imageName);
};

/*! \category GiGraphView()
    \brief GiGraphView 的内部数据定义
 */
@interface GiGraphView()<UIGestureRecognizerDelegate> {
    GiViewAdapter   *_adapter;              //!< 视图回调适配器
    
    UIPanGestureRecognizer *_panRecognizer;             //!< 拖动手势识别器
    UITapGestureRecognizer *_tapRecognizer;             //!< 单指点击手势识别器
    UITapGestureRecognizer *_twoTapsRecognizer;         //!< 单指双击手势识别器
    UILongPressGestureRecognizer *_pressRecognizer;     //!< 单指长按手势识别器
    UIPinchGestureRecognizer *_pinchRecognizer;         //!< 双指放缩手势识别器
    UIRotationGestureRecognizer *_rotationRecognizer;   //!< 双指旋转手势识别器
    
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

/*! \category GiGraphView(GestureRecognizer)
    \brief GiGraphView 的手势响应实现部分
 */
@interface GiGraphView(GestureRecognizer)

- (void)setupGestureRecognizers;
- (BOOL)panHandler:(UIGestureRecognizer *)sender;
- (BOOL)tapHandler:(UITapGestureRecognizer *)sender;
- (BOOL)twoTapsHandler:(UITapGestureRecognizer *)sender;
- (BOOL)pressHandler:(UILongPressGestureRecognizer *)sender;
- (void)delayTap;
- (void)dispatchTapPending;

@end
