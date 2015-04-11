// mgshape.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgshape.h"
#include "mgstorage.h"
#include "mgcomposite.h"

bool MgShape::hasFillColor() const
{
    return context().hasFillColor() && shapec()->isClosed();
}

bool MgShape::draw(int mode, GiGraphics& gs, const GiContext *ctx, int segment) const
{
    GiContext tmpctx(context());

    if (shapec()->isKindOf(6)) { // MgComposite
        tmpctx = ctx ? *ctx : GiContext(0, GiColor(), GiContext::kNullLine);
    }
    else if (ctx) {
        float addw = ctx->getLineWidth();
        
        if (addw < -0.1f) {
            tmpctx.setExtraWidth(-addw);
        } else if (addw > 0.1f) {                               // 传入正数表示像素宽度
            tmpctx.setLineWidth(-addw, ctx->isAutoScale());     // 换成新的像素宽度
        }
        
        if (ctx->getLineColor().a > 0) {
            tmpctx.setLineColor(ctx->getLineColor());
        }
        if (!ctx->isNullLine()) {
            tmpctx.setLineStyle(ctx->getLineStyle());
        }
        if (ctx->hasFillColor()) {
            tmpctx.setFillColor(ctx->getFillColor());
        }
    }

    bool ret = false;
    Box2d rect(shapec()->getExtent() * gs.xf().modelToDisplay());

    rect.inflate(1 + gs.calcPenWidth(tmpctx.getLineWidth(), tmpctx.isAutoScale()) / 2);

    if (gs.beginShape(shapec()->getType(), getID(),
                      (int)shapec()->getChangeCount(),
                      rect.xmin, rect.ymin, rect.width(), rect.height())) {
        ret = drawShape(getParent(), *shapec(), mode, gs, tmpctx, segment);
        gs.endShape(shapec()->getType(), getID(), rect.xmin, rect.ymin);
    }
    return ret;
}

void MgShape::copy(const MgObject& src)
{
    if (src.isKindOf(Type())) {
        const MgShape& _src = (const MgShape&)src;
        shape()->copy(*_src.shapec());
        setContext(_src.context());
        setTag(_src.getTag());
        if (!getParent() && 0 == getID()) {
            setParent(_src.getParent(), _src.getID());
        }
    }
    else if (src.isKindOf(MgBaseShape::Type())) {
        shape()->copy(src);
    }
    shape()->update();
}

bool MgShape::isKindOf(int type) const
{
    return type == Type() || type == shapec()->getType();
}

bool MgShape::equals(const MgObject& src) const
{
    bool ret = false;

    if (src.isKindOf(Type())) {
        const MgShape& _src = (const MgShape&)src;
        ret = shapec()->equals(*_src.shapec())
            && context().equals(_src.context())
            && getTag() == _src.getTag();
    }

    return ret;
}

bool MgShape::save(MgStorage* s) const
{
    GiColor c;

    s->writeInt("tag", getTag());
    s->writeInt("lineStyle", (unsigned char)context().getLineStyle());
    s->writeFloat("lineWidth", context().getLineWidth());

    c = context().getLineColor();
    s->writeUInt("lineColor", c.b | (c.g << 8) | (c.r << 16) | (c.a << 24));
    c = context().getFillColor();
    s->writeUInt("fillColor", c.b | (c.g << 8) | (c.r << 16) | (c.a << 24));
    
    if (context().getStartArrayHead()) {
        s->writeInt("startArrayHead", context().getStartArrayHead());
    }
    if (context().getEndArrayHead()) {
        s->writeInt("endArrayHead", context().getEndArrayHead());
    }

    return shapec()->save(s);
}

bool MgShape::load(MgShapeFactory* factory, MgStorage* s)
{
    setTag(s->readInt("tag", getTag()));

    GiContext ctx;
    ctx.setLineStyle(s->readInt("lineStyle", 0));
    ctx.setLineWidth(s->readFloat("lineWidth", 0), true);
    ctx.setLineColor(GiColor(s->readInt("lineColor", 0xFF000000), true));
    ctx.setFillColor(GiColor(s->readInt("fillColor", 0), true));
    ctx.setStartArrayHead(s->readInt("startArrayHead", 0));
    ctx.setEndArrayHead(s->readInt("endArrayHead", 0));
    setContext(ctx);

    bool ret = shape()->load(factory, s);
    if (ret) {
        shape()->update();
    }

    return ret;
}

void MgShape::setContext(const GiContext& ctx, int mask)
{
    if ((mask & GiContext::kCopyAll) != GiContext::kCopyAll
        && shapec()->isKindOf(MgComposite::Type())) {
        MgShapeIterator it( ((MgComposite*)shape())->shapes());
        while (const MgShape* sp = it.getNext()) {
            ((MgShape*)sp)->setContext(ctx, mask);
        }
    }
}
