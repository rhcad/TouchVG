//! \file GiViewHelper+Layer.mm
//! \brief 实现iOS绘图视图辅助类 GiViewHelper 的图层转换器部分
// Copyright (c) 2013-2015, https://github.com/rhcad/vgios, BSD License

#import "GiViewHelper+Layer.h"
#import "GiPaintView.h"
#include "GiShapeAdapter.h"
#include "gicoreview.h"
#include "mgshape.h"

@implementation GiViewHelper(Layer)

- (CALayer *)exportLayerTree:(BOOL)hidden {
    CALayer *rootLayer = [CALayer layer];
    rootLayer.frame = self.view.bounds;
    
    GiShapeLayerCallback shapeCallback(rootLayer, hidden);
    GiShapeAdapter adapter(&shapeCallback);
    GiCoreView *coreView = [self.view coreView];
    long doc, gs;
    
    @synchronized([self.view locker]) {
        doc = coreView->acquireFrontDoc();
        if (!doc) {
            coreView->submitBackDoc(NULL, false);
            doc = coreView->acquireFrontDoc();
        }
        gs = coreView->acquireGraphics([self.view viewAdapter]);
    }
    coreView->drawAll(doc, gs, &adapter);
    
    GiCoreView::releaseDoc(doc);
    coreView->releaseGraphics(gs);
    
    return rootLayer;
}

- (CALayer *)exportLayers {
    GiShapeLayerCallback shapeCallback(nil, false);
    GiShapeAdapter adapter(&shapeCallback);
    GiCoreView *coreView = [self.view coreView];
    long doc, gs;
    
    @synchronized([self.view locker]) {
        doc = coreView->acquireFrontDoc();
        if (!doc) {
            coreView->submitBackDoc(NULL, false);
            doc = coreView->acquireFrontDoc();
        }
        gs = coreView->acquireGraphics([self.view viewAdapter]);
    }
    coreView->drawAll(doc, gs, &adapter);
    
    GiCoreView::releaseDoc(doc);
    coreView->releaseGraphics(gs);
    
    return shapeCallback.layer();
}

- (CALayer *)exportLayersForShape:(long)handle {
    GiShapeLayerCallback shapeCallback(nil, false);
    GiShapeAdapter adapter(&shapeCallback);
    GiCoreView *coreView = [self.view coreView];
    long hgs;
    GiGraphics* gs;
    
    @synchronized([self.view locker]) {
        hgs = coreView->acquireGraphics([self.view viewAdapter]);
        gs = GiGraphics::fromHandle(hgs);
    }
    
    if (gs && gs->beginPaint(&adapter)) {
        const MgShape *sp = MgShape::fromHandle(handle);
        if (sp && sp->isKindOf(MgShape::Type())) {
            sp->draw(0, *gs, NULL, -1);
        }
        gs->endPaint();
    }
    
    coreView->releaseGraphics(hgs);
    
    return shapeCallback.layer();
}

- (NSArray *)exportPathsForShape:(long)handle {
    NSMutableArray *paths = [NSMutableArray array];
    CALayer *layer = [self exportLayersForShape:handle];
    
    for (CAShapeLayer *shapeLayer in layer.sublayers) {
        UIBezierPath *path = [UIBezierPath bezierPathWithCGPath:shapeLayer.path];
        path.lineWidth = shapeLayer.lineWidth;
        [paths addObject:path];
    }
    
    return paths;
}

@end
