// HitTestCmd.cpp: 实现点中测试命令类
// License: LGPL, https://github.com/rhcad/touchvg

#include "HitTestCmd.h"

bool HitTestCmd::draw(const MgMotion* sender, GiGraphics* gs)
{
    GiContext ctx(0, GiColor(172, 0, 0, 64));

    for (std::list<Item>::const_iterator it = _items.begin();
         it != _items.end(); ++it) {
        gs->drawLine(&ctx, it->nearpt, it->pt);
    }
    const MgShape* sp = sender->view->shapes()->findShape(_curid);
    if (sp) {
        GiContext ctxSel(0, GiColor(0, 0, 255, 128));
        sp->draw(2, *gs, &ctxSel, _hit.segment);
    }
    if (_cur.dist < 1e8f) {
        ctx.setLineAlpha(128);
        ctx.setLineWidth(-1, false);
        gs->drawLine(&ctx, _cur.nearpt, _cur.pt);
    }

    return true;
}

bool HitTestCmd::touchBegan(const MgMotion*)
{
    return true;
}

bool HitTestCmd::touchMoved(const MgMotion* sender)
{
    if (mgIsZero(_tol)) {
        _tol = sender->displayMmToModel(20);
    }
    
    Box2d box(sender->pointM, 2 * _tol, 0);
    const MgShape* sp = sender->view->shapes()->hitTest(box, _hit);
    float mindist = sender->displayMmToModel(0.5f);
    
    _curid = sp ? sp->getID() : 0;
    _cur.nearpt = _hit.nearpt;
    _cur.pt = box.center();
    sender->view->redraw();
    
    if (sp && sender->dragging()) {
        //for (float x = -0.25f * _tol; x < 0.25f * _tol; x += _tol * 0.25f) {
        //for (float y = -0.25f * _tol; y < 0.25f * _tol; y += _tol * 0.25f) {
        //box.set(sender->pointM + Vector2d(x, y), 2 * _tol, 0);
        Item item;
        item.dist = _hit.dist;
        item.nearpt = _hit.nearpt;
        item.pt = box.center();
        if (item.dist < _tol) {
            std::list<Item>::const_iterator it = _items.begin();
            for (; it != _items.end() && !it->pt.isEqualTo(item.pt, Tol(mindist)); ++it) ;
            if (it == _items.end()) {
                _items.push_back(item);
            }
        }
    }

    return true;
}
