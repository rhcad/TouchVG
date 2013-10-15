//! \file gicoreview.cpp
//! \brief 实现内核视图类 GiCoreView
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

#include "gicoreview.h"
#include "GcShapeDoc.h"
#include "GcMagnifierView.h"
#include <mgcmdmgr.h>
#include <mgcmdmgrfactory.h>
#include <mgbasicspreg.h>
#include <mglockdata.h>
#include <RandomShape.h>
#include <mgjsonstorage.h>
#include <cmdsubject.h>
#include <mgselect.h>
#include <mglog.h>
#include <cmdbasic.h>
#include <map>

#define CALL_VIEW(func) if (curview) curview->func
#define CALL_VIEW2(func, v) curview ? curview->func : v

//! 供Java等语言用的 MgShape 实现类
class MgShapeExt : public MgShape
{
public:
    MgBaseShape* _shape;
    GiContext   _context;
    int         _id;
    MgShapes*   _parent;
    int         _tag;
    
    MgShapeExt(MgBaseShape* shape)
        : _shape(shape), _id(0), _parent(NULL), _tag(0) {
    }
    virtual ~MgShapeExt() { }

    GiContext* context() { return &_context; }
    const GiContext* contextc() const { return &_context; }
    MgBaseShape* shape() { return _shape; }
    const MgBaseShape* shapec() const { return _shape; }
    int getType() const { return 0x20000 | _shape->getType(); }
    void release() { _shape->release(); delete this; }
    int getTag() const { return _tag; }
    void setTag(int tag) { _tag = tag; }
    int getID() const { return _id; }
    MgShapes* getParent() const { return _parent; }
    
    MgObject* clone() const {
        MgShapeExt *p = new MgShapeExt(_shape->cloneShape());
        p->copy(*this);
        return p;
    }
    void setParent(MgShapes* p, int sid) {
        _parent = p;
        _id = sid;
        shape()->setOwner(this);
    }
};

class GiCoreViewImpl
    : public MgView
    , private MgLockData
    , private MgShapeFactory
{
public:
    GcShapeDoc*     _doc;
    MgCmdManager*   _cmds;
    GcBaseView*     curview;
    long            refcount;
    MgMotion        motion;
    std::vector<int>    newids;
    int             gestureHandler;
    MgJsonStorage   defaultStorage;
    long            regenPending;
    long            appendPending;
    long            redrawPending;
    std::map<int, MgShape* (*)()>   _shapeCreators;

public:
    GiCoreViewImpl() : curview(NULL), refcount(1), gestureHandler(0)
        , regenPending(-1), appendPending(-1), redrawPending(-1)
    {
        motion.view = this;
        motion.gestureType = 0;
        motion.gestureState = kMgGesturePossible;
        _doc = new GcShapeDoc();
        _cmds = MgCmdManagerFactory::create();

        MgBasicShapes::registerShapes(this);
        MgBasicCommands::registerCmds(this);
    }

    ~GiCoreViewImpl() {
        _cmds->release();
        delete _doc;
    }

    MgCmdManager* cmds() const { return _cmds; }
    GcShapeDoc* document() const { return _doc; }
    MgShapeDoc* doc() const { return _doc->doc(); }
    MgShapes* shapes() const { return doc()->getCurrentShapes(); }
    GiContext* context() const { return doc()->context(); }
    GiTransform* xform() const { return CALL_VIEW2(xform(), NULL); }
    GiGraphics* graph() const { return CALL_VIEW2(graph(), NULL); }
    Matrix2d& modelTransform() const { return doc()->modelTransform(); }

    int getNewShapeID() { return _cmds->getNewShapeID(); }
    void setNewShapeID(int sid) { _cmds->setNewShapeID(sid); }
    MgLockData* getLockData() { return this; }
    CmdSubject* getCmdSubject() { return cmds()->getCmdSubject(); }
    MgSelection* getSelection() { return cmds()->getSelection(); }
    MgShapeFactory* getShapeFactory() { return this; }
    MgSnap* getSnap() { return _cmds->getSnap(); }
    MgActionDispatcher* getAction() {
        return _cmds->getActionDispatcher(); }

    bool registerCommand(const char* name, MgCommand* (*creator)()) {
        return _cmds->registerCommand(name, creator); }
    bool toSelectCommand(const MgMotion* sender) {
        return _cmds->setCommand(sender, "select", NULL); }
    const char* getCommandName() { return _cmds->getCommandName(); }
    MgCommand* getCommand() { return _cmds->getCommand(); }
    MgCommand* findCommand(const char* name) {
        return _cmds->findCommand(name); }
    bool setCommand(const MgMotion* sender, const char* name) {
        return _cmds->setCommand(sender, name, NULL); }
    bool setCurrentShapes(MgShapes* shapes) {
        return doc()->setCurrentShapes(shapes); }

    bool shapeWillAdded(MgShape* shape) {
        return getCmdSubject()->onShapeWillAdded(&motion, shape); }
    bool shapeWillDeleted(MgShape* shape) {
        return getCmdSubject()->onShapeWillDeleted(&motion, shape); }
    bool shapeCanRotated(const MgShape* shape) {
        return getCmdSubject()->onShapeCanRotated(&motion, shape); }
    bool shapeCanTransform(const MgShape* shape) {
        return getCmdSubject()->onShapeCanTransform(&motion, shape); }
    bool shapeCanUnlock(const MgShape* shape) {
        return getCmdSubject()->onShapeCanUnlock(&motion, shape); }
    bool shapeCanUngroup(const MgShape* shape) {
        return getCmdSubject()->onShapeCanUngroup(&motion, shape); }
    void shapeMoved(MgShape* shape, int segment) {
        getCmdSubject()->onShapeMoved(&motion, shape, segment); }

    void commandChanged() {
        CALL_VIEW(deviceView()->commandChanged());
    }
    void selectionChanged() {
        CALL_VIEW(deviceView()->selectionChanged());
    }

    bool removeShape(MgShape* shape) {
        showContextActions(0, NULL, Box2d::kIdentity(), NULL);
        bool ret = (shape && shape->getParent()
            && shape->getParent()->removeShape(shape->getID()));
        if (ret) {
            getCmdSubject()->onShapeDeleted(&motion, shape);
        }
        return ret;
    }

    bool useFinger() {
        return CALL_VIEW2(deviceView()->useFinger(), true);
    }

    bool isContextActionsVisible() {
        return CALL_VIEW2(deviceView()->isContextActionsVisible(), false);
    }

    bool showContextActions(int /*selState*/, const int* actions, 
        const Box2d& selbox, const MgShape*)
    {
        int n = 0;
        for (; actions && actions[n] > 0; n++) {}

        if (n > 0 && motion.pressDrag && isContextActionsVisible()) {
            return false;
        }
        mgvector<int> arr(actions, n);
        mgvector<float> pos(2 * n);
        calcContextButtonPosition(pos, n, selbox);
        return CALL_VIEW2(deviceView()->showContextActions(arr, pos,
            selbox.xmin, selbox.ymin, selbox.width(), selbox.height()), false);
    }

    void shapeAdded(MgShape* sp) {
        if (newids.empty()) {
            newids.push_back(sp->getID());      // 记下新图形的ID
            regenAppend();                      // 通知视图获取快照并增量重绘
        }
        else if (newids.back() != 0) {          // 已经regenAppend，但视图还未重绘
            newids.insert(newids.begin(), sp->getID()); // 记下更多的ID
        }
        else {                                  // 已经regenAppend并增量重绘
            regenAll();
        }
        getCmdSubject()->onShapeAdded(&motion, sp);
    }

    void redraw() {
        if (redrawPending >= 0) {
            redrawPending++;
        }
        else {
            CALL_VIEW(deviceView()->redraw());  // 将调用dynDraw
        }
    }

    void regenAll() {
        newids.clear();
        if (regenPending >= 0) {
            regenPending++;
        }
        else {  // 将调用drawAll
            for (int i = 0; i < _doc->getViewCount(); i++) {
                _doc->getView(i)->deviceView()->regenAll();
            }
        }
    }

    void regenAppend() {
        if (appendPending >= 0) {
            appendPending++;
        }
        else {  // 将调用drawAppend
            for (int i = 0; i < _doc->getViewCount(); i++) {
                _doc->getView(i)->deviceView()->regenAppend();
            }
        }
    }

    void setView(GcBaseView* view) {
        if (curview != view) {
            curview = view;
        }
    }

    static void onShapesLocked(MgShapeDoc* doc, void* obj, bool locked) {
        GiCoreViewImpl* p = (GiCoreViewImpl*)obj;
        if (!locked && p->curview && p->doc() == doc) {
            p->curview->deviceView()->contentChanged();
        }
    }

    void checkDrawAppendEnded();
    bool drawCommand(GcBaseView* view, const MgMotion& motion, GiGraphics& gs);
    bool gestureToCommand(const MgMotion& motion);

private:
    void registerDocObserver(DocLocked func, void* obj) {
        doc()->registerObserver(func, obj); }
    void unregisterDocObserver(DocLocked func, void* obj) {
        doc()->unregisterObserver(func, obj); }
    bool lockData(int flags, int timeout) {
        return doc()->lockData(flags, timeout); }
    long unlockData(bool forWrite) {
        return doc()->unlockData(forWrite); }
    void afterChanged() { doc()->afterChanged(); }
    bool lockedForRead() const {
        return doc()->getLockData()->lockedForRead(); }
    bool lockedForWrite() const {
        return doc()->getLockData()->lockedForWrite(); }
    int getEditFlags() const {
        return doc()->getLockData()->getEditFlags(); }
    void resetEditFlags() {
        doc()->getLockData()->setEditFlags(0); }
    bool lockDynData(bool forWrite, int timeout) {
        return doc()->getDynLockData()->lockData(forWrite, timeout); }
    long unlockDynData(bool forWrite) {
        return doc()->getDynLockData()->unlockData(forWrite); }
    bool lockedForReadDyn() const {
        return doc()->getDynLockData()->lockedForRead(); }
    bool lockedForWriteDyn() const {
        return doc()->getDynLockData()->lockedForWrite(); }

    void registerShape(int type, MgShape* (*creator)()) {
        type = type & 0xFFFF;
        if (creator) {
            _shapeCreators[type] = creator;
        }
        else {
            _shapeCreators.erase(type);
        }
    }
    MgShape* createShape(int type) {
        std::map<int, MgShape* (*)()>::const_iterator it = _shapeCreators.find(type & 0xFFFF);
        if (it != _shapeCreators.end()) {
            return (it->second)();
        }

        MgBaseShape* sp = getCmdSubject()->createShape(&motion, type);
        if (sp) {
            return new MgShapeExt(sp);
        }
        
        return NULL;
    }

private:
    void calcContextButtonPosition(mgvector<float>& pos, int n, const Box2d& box)
    {
        Box2d selbox(box);

        selbox.inflate(12, 18);
        if (box.height() < (n < 7 ? 40 : 80)) {
            selbox.deflate(0, (box.height() - (n < 7 ? 40 : 80)) / 2);
        }
        if (box.width() < (n == 3 || n > 4 ? 120 : 40)) {
            selbox.deflate((box.width() - (n==3||n>4 ? 120 : 40)) / 2, 0);
        }

        Box2d rect(calcButtonPosition(pos, n, selbox));
        Vector2d off(moveActionsInView(rect));

        for (int i = 0; i < n; i++) {
            pos.set(2 * i, pos.get(2 * i) + off.x, pos.get(2 * i + 1) + off.y);
        }
    }

    Box2d calcButtonPosition(mgvector<float>& pos, int n, const Box2d& selbox)
    {
        Box2d rect;

        for (int i = 0; i < n; i++) {
            switch (i)
            {
            case 0:
                if (n == 1) {
                    pos.set(2 * i, selbox.center().x, selbox.ymin); // MT
                } else {
                    pos.set(2 * i, selbox.xmin, selbox.ymin);       // LT
                }
                break;
            case 1:
                if (n == 3) {
                    pos.set(2 * i, selbox.center().x, selbox.ymin); // MT
                } else {
                    pos.set(2 * i, selbox.xmax, selbox.ymin);       // RT
                }
                break;
            case 2:
                if (n == 3) {
                    pos.set(2 * i, selbox.xmax, selbox.ymin);       // RT
                } else {
                    pos.set(2 * i, selbox.xmax, selbox.ymax);       // RB
                }
                break;
            case 3:
                pos.set(2 * i, selbox.xmin, selbox.ymax);           // LB
                break;
            case 4:
                pos.set(2 * i, selbox.center().x, selbox.ymin);     // MT
                break;
            case 5:
                pos.set(2 * i, selbox.center().x, selbox.ymax);     // MB
                break;
            case 6:
                pos.set(2 * i, selbox.xmax, selbox.center().y);     // RM
                break;
            case 7:
                pos.set(2 * i, selbox.xmin, selbox.center().y);     // LM
                break;
            default:
                return rect;
            }
            rect.unionWith(Box2d(Point2d(pos.get(2 * i), pos.get(2 * i + 1)), 32, 32));
        }

        return rect;
    }

    Vector2d moveActionsInView(Box2d& rect)
    {
        Vector2d off;
        Box2d viewrect(0.f, 0.f, (float)xform()->getWidth(), (float)xform()->getHeight());

        if (!rect.isEmpty() && !viewrect.contains(rect)) {
            if (rect.xmin < 0) {
                off.x = -rect.xmin;
            }
            else if (rect.xmax > viewrect.xmax) {
                off.x = viewrect.xmax - rect.xmax;
            }

            if (rect.ymin < 0) {
                off.y = -rect.ymin;
            }
            else if (rect.ymax > viewrect.ymax) {
                off.y = viewrect.ymax - rect.ymax;
            }
        }

        return off;
    }
};

static int _dpi = 96;

class DrawLocker
{
    GiCoreViewImpl* _impl;
public:
    DrawLocker(GiCoreViewImpl* impl) : _impl(impl) {
        if (_impl->regenPending >= 0
            || _impl->appendPending >= 0
            || _impl->redrawPending >= 0)
        {
            _impl = NULL;
        }
        else {
            _impl->regenPending = 0;
            _impl->appendPending = 0;
            _impl->redrawPending = 0;
        }
    }

    ~DrawLocker() {
        if (!_impl) {
            return;
        }
        long regenPending = _impl->regenPending;
        long appendPending = _impl->appendPending;
        long redrawPending = _impl->redrawPending;

        _impl->regenPending = -1;
        _impl->appendPending = -1;
        _impl->redrawPending = -1;

        if (regenPending > 0)
            _impl->regenAll();
        else if (appendPending > 0)
            _impl->regenAppend();
        else if (redrawPending > 0)
            _impl->redraw();
    }
};

GcBaseView::GcBaseView(MgView* mgview, GiView *view)
: _mgview(mgview), _view(view), _gs(&_xf)
{
    mgview->document()->addView(this);
}

MgShapeDoc* GcBaseView::doc()
{
    return cmdView()->document()->doc();
}

MgShapes* GcBaseView::shapes()
{
    return doc()->getCurrentShapes();
}

GiCoreView::GiCoreView(GiCoreView* mainView)
{
    if (mainView) {
        impl = mainView->impl;
        impl->refcount++;
    }
    else {
        impl = new GiCoreViewImpl;
    }
}

GiCoreView::~GiCoreView()
{
    if (--impl->refcount == 0) {
        delete impl;
    }
}

MgView* GiCoreView::viewAdapter()
{
    return impl;
}

long GiCoreView::viewAdapterHandle()
{
    long ret;
    *(MgView **)&ret = viewAdapter();
    return ret;
}

void GiCoreView::createView(GiView* view, int type)
{
    if (view && !impl->_doc->findView(view)) {
        impl->curview = new GcGraphView(impl, view);
        if (type == 0) {
            setCommand(view, "splines");
        }
    }
}

void GiCoreView::createMagnifierView(GiView* newview, GiView* mainView)
{
    GcGraphView* refview = dynamic_cast<GcGraphView *>(impl->_doc->findView(mainView));

    if (refview && newview && !impl->_doc->findView(newview)) {
        new GcMagnifierView(impl, newview, refview);
    }
}

void GiCoreView::destoryView(GiView* view)
{
    GcBaseView* aview = this ? impl->_doc->findView(view) : NULL;

    if (aview) {
        impl->_doc->removeView(aview);

        if (impl->curview == aview) {
            impl->curview = impl->_doc->firstView();
        }
        delete aview;
    }
}

int GiCoreView::setBkColor(GiView* view, int argb)
{
    GcBaseView* aview = impl->_doc->findView(view);
    return aview ? aview->graph()->setBkColor(GiColor(argb)).getARGB() : 0;
}

void GiCoreView::setScreenDpi(int dpi)
{
    if (_dpi != dpi && dpi > 0) {
        _dpi = dpi;
    }
}

void GiCoreViewImpl::checkDrawAppendEnded()
{
    int n = 0;

    for (size_t i = 0; i < newids.size(); i++) {
        if (newids[i] == 0) {
            n++;
        }
    }
    if (n >= _doc->getViewCount()) {
        newids.clear();
    }
}

int GiCoreView::drawAll(GiView* view, GiCanvas* canvas)
{
    GcBaseView* aview = impl->_doc->findView(view);
    GiGraphics* gs = aview->graph();
    int n = 0;

    if (aview && gs->beginPaint(canvas)) {
        n = aview->drawAll(*gs);
        gs->endPaint();
    }
    if (!impl->newids.empty()) {
        impl->newids.push_back(0);
    }
    impl->checkDrawAppendEnded();

    return n;
}

bool GiCoreView::drawAppend(GiView* view, GiCanvas* canvas)
{
    GcBaseView* aview = impl->_doc->findView(view);
    GiGraphics* gs = aview->graph();
    int n = 0;

    if (aview && !impl->newids.empty()
        && gs->beginPaint(canvas)) {
            n = aview->drawAppend(&impl->newids.front(), *gs);
            gs->endPaint();
    }
    if (!impl->newids.empty()) {
        impl->newids.push_back(0);
    }
    impl->checkDrawAppendEnded();

    return n > 0;
}

void GiCoreView::dynDraw(GiView* view, GiCanvas* canvas)
{
    GcBaseView* aview = impl->_doc->findView(view);
    GiGraphics* gs = aview->graph();

    impl->motion.d2mgs = impl->cmds()->displayMmToModel(1, gs);

    if (aview && gs->beginPaint(canvas)) {
        impl->drawCommand(aview, impl->motion, *gs);
        aview->dynDraw(impl->motion, *gs);
        gs->endPaint();
    }
}

void GiCoreView::onSize(GiView* view, int w, int h)
{
    GcBaseView* aview = impl->_doc->findView(view);
    if (aview) {
        aview->onSize(_dpi, w, h);
    }
}

bool GiCoreView::onGesture(GiView* view, GiGestureType type,
                           GiGestureState state, float x, float y, bool switchGesture)
{
    DrawLocker locker(impl);
    GcBaseView* aview = impl->_doc->findView(view);
    bool ret = false;

    if (aview) {
        impl->setView(aview);
        impl->motion.gestureType = type;
        impl->motion.gestureState = (MgGestureState)state;
        impl->motion.pressDrag = (type == kGiGesturePress && state < kGiGestureEnded);
        impl->motion.switchGesture = switchGesture;
        impl->motion.point.set(x, y);
        impl->motion.pointM = impl->motion.point * aview->xform()->displayToModel();
        impl->motion.point2 = impl->motion.point;
        impl->motion.point2M = impl->motion.pointM;
        impl->motion.d2m = impl->cmds()->displayMmToModel(1, &impl->motion);

        if (state <= kGiGestureBegan) {
            impl->motion.startPt = impl->motion.point;
            impl->motion.startPtM = impl->motion.pointM;
            impl->motion.lastPt = impl->motion.point;
            impl->motion.lastPtM = impl->motion.pointM;
            impl->motion.startPt2 = impl->motion.point;
            impl->motion.startPt2M = impl->motion.pointM;
            impl->gestureHandler = (impl->gestureToCommand(impl->motion) ? 1
                : (aview->onGesture(impl->motion) ? 2 : 0));
        }
        else if (impl->gestureHandler == 1) {
            impl->gestureToCommand(impl->motion);
        }
        else if (impl->gestureHandler == 2) {
            aview->onGesture(impl->motion);
        }
        ret = (impl->gestureHandler > 0);

        impl->motion.lastPt = impl->motion.point;
        impl->motion.lastPtM = impl->motion.pointM;
    }

    return ret;
}

bool GiCoreView::twoFingersMove(GiView* view, GiGestureState state,
                                float x1, float y1, float x2, float y2, bool switchGesture)
{
    DrawLocker locker(impl);
    GcBaseView* aview = impl->_doc->findView(view);
    bool ret = false;

    if (aview) {
        impl->setView(aview);
        impl->motion.gestureType = kGiTwoFingersMove;
        impl->motion.gestureState = (MgGestureState)state;
        impl->motion.pressDrag = false;
        impl->motion.switchGesture = switchGesture;
        impl->motion.point.set(x1, y1);
        impl->motion.pointM = impl->motion.point * aview->xform()->displayToModel();
        impl->motion.point2.set(x2, y2);
        impl->motion.point2M = impl->motion.point2 * aview->xform()->displayToModel();
        impl->motion.d2m = impl->cmds()->displayMmToModel(1, &impl->motion);

        if (state <= kGiGestureBegan) {
            impl->motion.startPt = impl->motion.point;
            impl->motion.startPtM = impl->motion.pointM;
            impl->motion.lastPt = impl->motion.point;
            impl->motion.lastPtM = impl->motion.pointM;
            impl->motion.startPt2 = impl->motion.point2;
            impl->motion.startPt2M = impl->motion.point2M;
            impl->gestureHandler = (impl->gestureToCommand(impl->motion) ? 1
                : (aview->twoFingersMove(impl->motion) ? 2 : 0));
        }
        else if (impl->gestureHandler == 1) {
            impl->gestureToCommand(impl->motion);
        }
        else if (impl->gestureHandler == 2) {
            aview->twoFingersMove(impl->motion);
        }
        ret = (impl->gestureHandler > 0);

        impl->motion.lastPt = impl->motion.point;
        impl->motion.lastPtM = impl->motion.pointM;
    }

    return ret;
}

bool GiCoreView::isPressDragging()
{
    return impl->motion.pressDrag;
}

GiGestureType GiCoreView::getGestureType()
{
    return (GiGestureType)impl->motion.gestureType;
}

GiGestureState GiCoreView::getGestureState()
{
    return (GiGestureState)impl->motion.gestureState;
}

const char* GiCoreView::getCommand() const
{
    return impl->_cmds->getCommandName();
}

bool GiCoreView::setCommand(GiView* view, const char* name, const char* params)
{
    DrawLocker locker(impl);
    GcBaseView* aview = impl->_doc->findView(view);
    bool ret = false;

    if (aview) {
        impl->setView(aview);
        MgJsonStorage s;
        ret = impl->_cmds->setCommand(&impl->motion, name, 
                                      s.storageForRead(params));
    }

    return ret;
}

bool GiCoreView::doContextAction(int action)
{
    DrawLocker locker(impl);
    return impl->_cmds->doContextAction(&impl->motion, action);
}

void GiCoreView::clearCachedData()
{
    impl->doc()->clearCachedData();
}

int GiCoreView::addShapesForTest()
{
    int n = RandomParam().addShapes(impl->shapes());
    impl->regenAll();
    return n;
}

int GiCoreView::getShapeCount()
{
    MgShapesLock locker(MgShapesLock::ReadOnly, impl);
    return impl->doc()->getShapeCount();
}

int GiCoreView::getChangeCount()
{
    return impl->doc()->getChangeCount();
}

int GiCoreView::getSelectedShapeCount()
{
    return impl->cmds()->getSelection(impl, 0, NULL);
}

int GiCoreView::getSelectedShapeType()
{
    MgSelection* sel = impl->cmds()->getSelection();
    return sel ? sel->getSelectType(impl) : 0;
}

bool GiCoreView::loadShapes(MgStorage* s)
{
    bool ret = true;

    impl->setCommand(&impl->motion, impl->getCommandName());
    MgCommand* cmd = impl->getCommand();
    if (cmd) cmd->cancel(&impl->motion);

    if (s) {
        MgShapesLock locker(MgShapesLock::Load, impl);
        ret = impl->doc()->load(impl->getShapeFactory(), s);
        LOGD("Load %d shapes", impl->doc()->getShapeCount());
    }
    else {
        MgShapesLock locker(MgShapesLock::Remove, impl);
        impl->doc()->clear();
    }
    impl->regenAll();
    impl->getCmdSubject()->onDocLoaded(&impl->motion);

    return ret;
}

bool GiCoreView::saveShapes(MgStorage* s)
{
    MgShapesLock locker(MgShapesLock::ReadOnly, impl);
    return s && impl->doc()->save(s);
}

void GiCoreView::clear()
{
    DrawLocker locker(impl);
    loadShapes((MgStorage*)0);
}

const char* GiCoreView::getContent()
{
    const char* content = "";
    if (saveShapes(impl->defaultStorage.storageForWrite())) {
        content = impl->defaultStorage.stringify();
    }
    return content; // has't free defaultStorage's string buffer
}

void GiCoreView::freeContent()
{
    impl->defaultStorage.clear();
}

bool GiCoreView::setContent(const char* content)
{
    DrawLocker locker(impl);
    bool ret = loadShapes(impl->defaultStorage.storageForRead(content));
    impl->defaultStorage.clear();
    return ret;
}

bool GiCoreView::loadFromFile(const char* vgfile)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    FILE *fp = NULL;
    fopen_s(&fp, vgfile, "rt");
#else
    FILE *fp = fopen(vgfile, "rt");
#endif
    DrawLocker locker(impl);
    MgJsonStorage s;
    bool ret = loadShapes(s.storageForRead(fp));

    if (fp) {
        fclose(fp);
    } else {
        LOGE("Fail to open file: %s", vgfile);
    }

    return ret;
}

bool GiCoreView::saveToFile(const char* vgfile, bool pretty)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    FILE *fp = NULL;
    fopen_s(&fp, vgfile, "wt");
#else
    FILE *fp = fopen(vgfile, "wt");
#endif
    MgJsonStorage s;
    bool ret = (fp != NULL
        && saveShapes(s.storageForWrite())
        && s.save(fp, pretty));

    if (fp) {
        fclose(fp);
    } else {
        LOGE("Fail to open file: %s", vgfile);
    }

    return ret;
}

bool GiCoreView::zoomToExtent()
{
    Box2d rect(impl->doc()->getExtent() * impl->xform()->modelToWorld());
    bool ret = impl->xform()->zoomTo(rect);
    if (ret) {
        impl->regenAll();
    }
    return ret;
}

bool GiCoreView::zoomToModel(float x, float y, float w, float h)
{
    Box2d rect(Box2d(x, y, x + w, y + h) * impl->xform()->modelToWorld());
    bool ret = impl->xform()->zoomTo(rect);
    if (ret) {
        impl->regenAll();
    }
    return ret;
}

float GiCoreView::calcPenWidth(float lineWidth)
{
    return impl->graph()->calcPenWidth(lineWidth, false);
}

static GiContext _tmpContext;
static bool _contextEditing = false;

GiContext& GiCoreView::getContext(bool forChange)
{
    if (!forChange) {
        _contextEditing = false;
    }

    MgShape* shape = NULL;
    impl->_cmds->getSelection(impl, 1, &shape, forChange);
    _tmpContext = shape ? *shape->context() : *impl->context();

    return _tmpContext;
}

void GiCoreView::setContext(int mask)
{
    setContext(_tmpContext, mask, _contextEditing ? 0 : 1);
}

void GiCoreView::setContextEditing(bool editing)
{
    if (_contextEditing != editing) {
        _contextEditing = editing;
        if (!editing) {
            DrawLocker locker(impl);
            impl->_cmds->dynamicChangeEnded(impl, true);
        }
    }
}

void GiCoreView::setContext(const GiContext& ctx, int mask, int apply)
{
    DrawLocker locker(impl);

    if (mask != 0) {
        int n = impl->_cmds->getSelection(impl, 0, NULL, true);
        std::vector<MgShape*> shapes(n, (MgShape*)0);

        if (n > 0 && impl->_cmds->getSelection(impl, n, 
            (MgShape**)&shapes.front(), true) > 0)
        {
            for (int i = 0; i < n; i++) {
                if (shapes[i]) {
                    shapes[i]->context()->copy(ctx, mask);
                }
            }
            impl->redraw();
        }
        else {
            impl->context()->copy(ctx, mask);
        }
    }

    if (apply != 0) {
        impl->_cmds->dynamicChangeEnded(impl, apply > 0);
    }
}

int GiCoreView::addImageShape(const char* name, float width, float height)
{
    MgShape* shape = impl->_cmds->addImageShape(&impl->motion, name, width, height);
    return shape ? shape->getID() : 0;
}

int GiCoreView::addImageShape(const char* name, float xc, float yc, float w, float h)
{
    MgShape* shape = impl->_cmds->addImageShape(&impl->motion, name, xc, yc, w, h);
    return shape ? shape->getID() : 0;
}

bool GiCoreView::getBoundingBox(mgvector<float>& box)
{
    bool ret = box.count() == 4 && impl->curview;
    if (ret) {
        Box2d rect;
        impl->_cmds->getBoundingBox(rect, &impl->motion);
        box.set(0, rect.xmin, rect.ymin);
        box.set(2, rect.xmax, rect.ymax);
    }
    return ret;
}

bool GiCoreView::getBoundingBox(mgvector<float>& box, int shapeId)
{
    const MgShape* shape = impl->shapes()->findShape(shapeId);
    bool ret = box.count() == 4 && shape;
    
    if (ret) {
        Box2d rect(shape->shapec()->getExtent());
        impl->_cmds->getBoundingBox(rect, &impl->motion);
        box.set(0, rect.xmin, rect.ymin);
        box.set(2, rect.xmax, rect.ymax);
    }
    return ret;
}

bool GiCoreViewImpl::drawCommand(GcBaseView* view, const MgMotion& motion, GiGraphics& gs)
{
    bool ret = false;

    if (view == curview) {
        MgDynShapeLock locker(false, this);
        MgCommand* cmd = _cmds->getCommand();

        ret = cmd && cmd->draw(&motion, &gs);
        if (ret && cmd->isDrawingCommand()) {
            getCmdSubject()->drawInShapeCommand(&motion, cmd, &gs);
        }
    }

    return ret;
}

bool GiCoreViewImpl::gestureToCommand(const MgMotion& motion)
{
    MgDynShapeLock locker(true, motion.view);
    MgCommand* cmd = _cmds->getCommand();
    bool ret = false;

    if (motion.gestureState == kMgGestureCancel || !cmd) {
        return cmd && cmd->cancel(&motion);
    }
    if (motion.gestureState == kMgGesturePossible
        && motion.gestureType != kGiTwoFingersMove) {
        return true;
    }

    switch (motion.gestureType)
    {
    case kGiTwoFingersMove:
        ret = cmd->twoFingersMove(&motion);
        break;
    case kGiGesturePan:
        switch (motion.gestureState)
        {
        case kMgGestureBegan:
            ret = cmd->touchBegan(&motion);
            break;
        case kMgGestureMoved:
            ret = cmd->touchMoved(&motion);
            break;
        case kMgGestureEnded:
        default:
            ret = cmd->touchEnded(&motion);
            break;
        }
        break;
    case kGiGestureTap:
        ret = cmd->click(&motion);
        break;
    case kGiGestureDblTap:
        ret = cmd->doubleClick(&motion);
        break;
    case kGiGesturePress:
        ret = cmd->longPress(&motion);
        break;
    }

    if (!ret) {
        LOGD("The current command (%s) don't support #%d gesture (state=%d)",
            cmd->getName(), motion.gestureType, motion.gestureState);
    }
    return ret;
}
