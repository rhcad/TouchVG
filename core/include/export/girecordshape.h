//! \file girecordshape.h
//! \brief Define the shape class to record drawing: MgRecordShape.
// Copyright (c) 2013, https://github.com/rhcad/touchvg

#ifndef TOUCHVG_CORE_GIRECORDSHAPE_H
#define TOUCHVG_CORE_GIRECORDSHAPE_H

#include "mgshape.h"
#include <vector>

//! The shape class to record drawing.
/*! \ingroup CORE_SHAPE
 */
class MgRecordShape : public MgBaseShape
{
public:
    MgRecordShape() : _sid(0) {}
    virtual ~MgRecordShape() { _clear(); }
    
    //! Interface of recording items.
    struct ICmd {
        virtual ~ICmd() {}
        virtual int type() const = 0;
        virtual void copy(const ICmd& src) = 0;
        virtual bool save(MgStorage* s) const = 0;
        virtual bool load(MgStorage* s) = 0;
        virtual bool draw(GiGraphics& gs, const Matrix2d& w2d) const = 0;
        virtual Box2d getExtentW() const { return Box2d(0, 0, 1, 1); }
    };
    
    int getCount() const { return (int)_items.size(); }
    void addItem(const Matrix2d& w2m, ICmd* p);
    void setRefID(int sid) { _sid = sid; }
    
    static MgRecordShape* create() { return new MgRecordShape(); }
    static int Type() { return 30; }
    
    virtual MgObject* clone() const;
    virtual void copy(const MgObject& src);
    virtual void release() { delete this; }
    virtual bool equals(const MgObject& src) const;
    virtual int getType() const { return Type(); }
    virtual bool isKindOf(int type) const { return type == Type() || type == MgBaseShape::Type(); }
    
    virtual void clear() { _clear(); MgBaseShape::_clear(); }
    virtual bool draw(int mode, GiGraphics& gs, const GiContext& ctx, int segment) const;
    virtual void output(MgPath&) const {}
    virtual bool save(MgStorage* s) const;
    virtual bool load(MgShapeFactory* factory, MgStorage* s);
    
    virtual bool isCurve() const { return true; }
    virtual int getPointCount() const { return 0; }
    virtual Point2d getPoint(int) const { return Point2d(); }
    virtual void setPoint(int, const Point2d&) {}
    virtual float hitTest(const Point2d&, float, MgHitResult&) const { return _FLT_MAX; }
    
private:
    void _clear();
    ICmd* createItem(int type) const;
    
    typedef std::vector<ICmd*>  ITEMS;
    ITEMS   _items;
    int     _sid;
};

#endif // TOUCHVG_CORE_GIRECORDSHAPE_H
