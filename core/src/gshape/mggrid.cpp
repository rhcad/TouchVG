// mggrid.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mggrid.h"
#include "mgshape_.h"

MG_IMPLEMENT_CREATE(MgGrid)

MgGrid::MgGrid()
{
    setFlag(kMgRotateDisnable, true);
}

MgGrid::~MgGrid()
{
}

void MgGrid::_copy(const MgGrid& src)
{
    m_cell = src.m_cell;
    __super::_copy(src);
}

bool MgGrid::_equals(const MgGrid& src) const
{
    return m_cell == src.m_cell && __super::_equals(src);
}

void MgGrid::_clear()
{
    m_cell = Vector2d::kIdentity();
    __super::_clear();
}

void MgGrid::setFlag(MgShapeBit bit, bool on)
{
    __super::setFlag(bit, bit == kMgRotateDisnable || on);
}

int MgGrid::_getHandleCount() const
{
    return (getFlag(kMgFixedLength) || getFlag(kMgLocked)) ? 8 : 9;
}

Point2d MgGrid::_getHandlePoint(int index) const
{
    return (index < 8 ? __super::_getHandlePoint(index) : getPoint(3) +
            (m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell));
}

bool MgGrid::_setHandlePoint(int index, const Point2d& pt, float tol)
{
    if (index < 8) {
        return __super::_setHandlePoint(index, pt, tol);
    }
    
    float cx = fabsf(pt.x - getPoint(3).x);
    float cy = fabsf(pt.y - getPoint(3).y);
    cx = mgIsZero(cx) ? cx : mgMax(cx, tol / 3.f);
    cy = mgIsZero(cy) ? cy : mgMax(cy, tol / 3.f);
    cx = mgbase::roundReal(mgMax(cx, cy), 1);
    m_cell.set(cx, cx);
    
    return true;
}

bool MgGrid::isValid(float tol) const
{
    return (m_cell.x > tol && m_cell.y > tol
            && m_cell.x < getWidth() && m_cell.y < getHeight());
}

bool MgGrid::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    s->writeFloat("cellw", m_cell.x);
    s->writeFloat("celly", m_cell.y);
    return ret;
}

bool MgGrid::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    m_cell.set(s->readFloat("cellw", m_cell.x), s->readFloat("celly", m_cell.y));
    return ret;
}

int MgGrid::snap(Point2d& pnt, Point2d& dist) const
{
    int ret = 0;
    Point2d newpt(pnt);
    Point2d org(getPoint(3));
    Vector2d cell(m_cell == Vector2d() ? Vector2d(getWidth()/4, getHeight()/4) : m_cell / 2);
    
    if (cell.x < _MGZERO || cell.y < _MGZERO)
        return ret;
    
    dist.x *= 3;
    dist.y *= 3;
    
    for (float x = cell.x; x < getWidth() - _MGZERO; x += cell.x) {        
        if (dist.x > fabsf(pnt.x - (org.x + x))) {
            newpt.x = org.x + x;
            dist.x = fabsf(pnt.x - newpt.x);
            ret |= 1;
        }
    }
    for (float y = cell.y; y < getHeight() - _MGZERO; y += cell.y) {
        if (dist.y > fabsf(pnt.y - (org.y + y))) {
            newpt.y = org.y + y;
            dist.y = fabsf(pnt.y - newpt.y);
            ret |= 2;
        }
    }
    
    pnt = newpt;
    if ((ret & 1) == 0)
        dist.x /= 3;
    if ((ret & 2) == 0)
        dist.y /= 3;
    
    return ret;
}
