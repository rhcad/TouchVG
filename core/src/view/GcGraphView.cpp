//! \file GcGraphView.cpp
//! \brief 实现主绘图视图类 GcGraphView
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "GcGraphView.h"
#include "mglog.h"

// GcBaseView
//

GcBaseView::~GcBaseView()
{
    LOGD("View %p destroyed", this);
}

void GcBaseView::onSize(int dpi, int w, int h)
{
    xform()->setResolution((float)dpi);
    xform()->setWndSize(w, h);
}

bool GcBaseView::onGesture(const MgMotion& )
{
    return false;
    /*
    if (motion.gestureType != kGiGesturePan || !_zoomEnabled){
        return false;
    }
    if (motion.gestureState <= kMgGestureBegan) {
        _lastScale = xform()->getZoomValue(_lastCenter);
    }
    else if (motion.gestureState == kMgGestureMoved) {
        Vector2d vec(motion.point - motion.startPt);
        xform()->zoom(_lastCenter, _lastScale);     // 先恢复
        xform()->zoomPan(vec.x, vec.y);             // 平移到当前点
        _zooming = true;
        cmdView()->regenAll(false);
    }
    else if (_zooming) {
        _zooming = false;
        cmdView()->regenAll(false);
    }
    
    return true;*/
}

bool GcBaseView::twoFingersMove(const MgMotion& motion)
{
    if (!_zoomEnabled) {
        return false;
    }
    if (motion.gestureState <= kMgGestureBegan) {
        _lastScale = xform()->getZoomValue(_lastCenter);
    }
    else if (motion.gestureState == kMgGestureMoved
        && motion.startPt != motion.startPt2
        && motion.point != motion.point2) {         // 双指变单指则忽略移动
        Point2d at((motion.startPt + motion.startPt2) / 2);
        Point2d pt((motion.point + motion.point2) / 2);
        float d1 = motion.point.distanceTo(motion.point2);
        float d0 = motion.startPt.distanceTo(motion.startPt2);
        float scale = d1 / d0;
        
        xform()->zoom(_lastCenter, _lastScale);     // 先恢复
        xform()->zoomByFactor(scale - 1, &at);      // 以起始点为中心放缩显示
        xform()->zoomPan(pt.x - at.x, pt.y - at.y); // 平移到当前点
        
        _zooming = true;
        cmdView()->regenAll(false);
    }
    else if (_zooming) {
        _zooming = false;
        cmdView()->regenAll(false);
    }
    
    return true;
}

void GcBaseView::draw(GiGraphics&)
{
}

void GcBaseView::dyndraw(GiGraphics&)
{
}

// GcGraphView
//

GcGraphView::GcGraphView(MgView* mgview, GiView *view) : GcBaseView(mgview, view)
{
}

GcGraphView::~GcGraphView()
{
}

void GcGraphView::draw(GiGraphics& gs)
{
    int gridType = cmdView()->getOptionInt("showGrid", 0);
    if (gridType < 1 || gridType > 2 || gs.xf().getViewScale() < 0.05f)
        return;
    
    Box2d rect(gs.xf().getWndRectW());
    GiContext ctx(0, GiColor(127, 127, 127, gridType == 2 ? 48 : 20));
    
    if (gridType == 1) {
        GiContext ctx5(0, GiColor(127, 127, 127, 48));
        float x = mgbase::roundReal(rect.xmin, -1) - 10;
        float y = mgbase::roundReal(rect.ymin, -1) - 10;
        int i = mgRound(x) / 10;
        
        for (; x < rect.xmax + 10; x += 10) {
            gs.drawLine(i++ % 5 ? &ctx : &ctx5, Point2d(x, rect.ymin), Point2d(x, rect.ymax), false);
        }
        i = mgRound(y) / 10;
        for (; y < rect.ymax + 10; y += 10) {
            gs.drawLine(i++ % 5 ? &ctx : &ctx5, Point2d(rect.xmin, y), Point2d(rect.xmax, y), false);
        }
    }
    else if (gridType == 2) {
        for (float x = rect.xmin - 10; x < rect.xmax + 10; x += 10) {
            for (float y = rect.ymin - 10; y < rect.ymax + 10; y += 10) {
                gs.drawLine(&ctx, Point2d(x, y - 0.5f), Point2d(x, y + 0.5f), false);
                gs.drawLine(&ctx, Point2d(x - 0.5f, y), Point2d(x + 0.5f, y), false);
            }
        }
    }
    
    GcBaseView::draw(gs);
}
