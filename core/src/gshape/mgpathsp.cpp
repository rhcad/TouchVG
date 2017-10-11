// mgpathsp.cpp: 实现路径图形 MgPathShape
// Copyright (c) 2004-2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore

#include "mgpathsp.h"
#include "mgshape_.h"
#include "vector"
#include <sstream>
#include <string.h>

MG_IMPLEMENT_CREATE(MgPathShape)

MgPathShape::MgPathShape()
{
}

MgPathShape::~MgPathShape()
{
}

int MgPathShape::_getPointCount() const
{
    return _path.getCount();
}

Point2d MgPathShape::_getPoint(int index) const
{
    return _path.getPoint(index);
}

void MgPathShape::_setPoint(int index, const Point2d& pt)
{
    _path.setPoint(index, pt);
}

void MgPathShape::_copy(const MgPathShape& src)
{
    _path.copy(src._path);
    __super::_copy(src);
}

bool MgPathShape::_equals(const MgPathShape& src) const
{
    if (_path.getCount() != src._path.getCount() || !__super::_equals(src)) {
        return false;
    }
    for (int i = 0; i < _path.getCount(); i++) {
        if (_path.getPoint(i) != src._path.getPoint(i) ||
            _path.getNodeType(i) != src._path.getNodeType(i)) {
            return false;
        }
    }
    return true;
}

void MgPathShape::_update()
{
    _extent.set(_path.getCount(), _path.getPoints());
    __super::_update();
}

void MgPathShape::_transform(const Matrix2d& mat)
{
    for (int i = 0; i < _path.getCount(); i++) {
        _path.setPoint(i, _path.getPoint(i) * mat);
    }
    __super::_transform(mat);
}

void MgPathShape::_clear()
{
    _path.clear();
    __super::_clear();
}

bool MgPathShape::_isClosed() const
{
    return !!(_path.getNodeType(_path.getCount() - 1) & kMgCloseFigure);
}

bool MgPathShape::isCurve() const
{
    for (int i = 0; i < _path.getCount(); i++) {
        if (_path.getNodeType(i) & (kMgBezierTo | kMgQuadTo))
            return true;
    }
    return false;
}

float MgPathShape::_hitTest(const Point2d& pt, float tol, MgHitResult& res) const
{
    int n = _path.getCount();
    const Point2d* pts = _path.getPoints();
    const char* types = _path.getTypes();
    Point2d pos, ends, bz[7], nearpt;
    bool err = false;
    const Box2d rect (pt, 2 * tol, 2 * tol);
    std::vector<Point2d> edges;
    
    res.dist = _FLT_MAX - tol;
    for (int i = 0; i < n && !err; i++) {
        float dist = _FLT_MAX;
        pos = ends;
        
        switch (types[i] & ~kMgCloseFigure) {
            case kMgMoveTo:
                ends = pts[i];
                break;
                
            case kMgLineTo:
                ends = pts[i];
                if (rect.isIntersect(Box2d(pos, ends))) {
                    dist = mglnrel::ptToLine(pos, ends, pt, nearpt);
                }
                break;
                
            case kMgBezierTo:
                if (i + 2 >= n) {
                    err = true;
                    break;
                }
                bz[0] = pos;
                bz[1] = pts[i];
                bz[2] = pts[i+1];
                bz[3] = pts[i+2];
                ends = bz[3];
                
                if (rect.isIntersect(mgnear::bezierBox1(bz))) {
                    dist = mgnear::nearestOnBezier(pt, bz, nearpt);
                }
                i += 2;
                break;
                
            case kMgQuadTo:
                if (i + 1 >= n) {
                    err = true;
                    break;
                }
                bz[0] = pos;
                bz[1] = pts[i];
                bz[2] = pts[i+1];
                ends = bz[2];
                
                mgcurv::quadBezierToCubic(bz, bz + 3);
                if (rect.isIntersect(mgnear::bezierBox1(bz + 3))) {
                    dist = mgnear::nearestOnBezier(pt, bz, nearpt);
                }
                i++;
                break;
                
            default:
                err = true;
                break;
        }
        edges.push_back(ends);
        if (res.dist > dist) {
            res.dist = dist;
            res.segment = i;
            res.nearpt = nearpt;
        }
    }
    if (isClosed() && edges.size() > 2) {
        MgHitResult tmpres;
        tmpres = res;
        linesHit((int)edges.size(), &edges.front(), true, pt, tol, tmpres);
        res.inside = tmpres.inside;
    }
    
    return res.dist;
}

bool MgPathShape::_hitTestBox(const Box2d& rect) const
{
    if (!__super::_hitTestBox(rect))
        return false;
    
    int n = _path.getCount();
    const Point2d* pts = _path.getPoints();
    const char* types = _path.getTypes();
    Point2d pos, ends, bz[7];
    int ret = 0;
    
    for (int i = 0; i < n && !ret; i++) {
        pos = ends;
        
        switch (types[i] & ~kMgCloseFigure) {
            case kMgMoveTo:
                ends = pts[i];
                break;
                
            case kMgLineTo:
                ends = pts[i];
                ret = rect.isIntersect(Box2d(pos, ends));
                break;
                
            case kMgBezierTo:
                if (i + 2 >= n) {
                    ret |= 2;
                    break;
                }
                bz[0] = pos;
                bz[1] = pts[i];
                bz[2] = pts[i+1];
                bz[3] = pts[i+2];
                ends = bz[3];
                ret |= rect.isIntersect(mgnear::bezierBox1(bz)) ? 1 : 0;
                i += 2;
                break;
                
            case kMgQuadTo:
                if (i + 1 >= n) {
                    ret |= 2;
                    break;
                }
                bz[0] = pos;
                bz[1] = pts[i];
                bz[2] = pts[i+1];
                ends = bz[2];
                mgcurv::quadBezierToCubic(bz, bz + 3);
                ret |= rect.isIntersect(mgnear::bezierBox1(bz + 3)) ? 1 : 0;
                i++;
                break;
                
            default:
                ret |= 2;
                break;
        }
    }
    
    return ret == 1;
}

static void exportPath(std::stringstream& ss, const MgPath& path)
{
    int n = path.getCount();
    const Point2d* pts = path.getPoints();
    const char* types = path.getTypes();
    char cmd = 0;
    Point2d curpt;
    
    for (int i = 0; i < n; i++) {
        switch (types[i] & ~kMgCloseFigure) {
            case kMgMoveTo:
                ss << "M" << pts[i].x << "," << pts[i].y << " ";
                cmd = 'L';
                break;
                
            case kMgLineTo:
                if (mgEquals(curpt.x, pts[i].x)) {
                    if (cmd != 'V') {
                        cmd = 'V';
                        ss << cmd;
                    }
                    ss << pts[i].y << " ";
                }
                else if (mgEquals(curpt.y, pts[i].y)) {
                    if (cmd != 'H') {
                        cmd = 'H';
                        ss << cmd;
                    }
                    ss << pts[i].x << " ";
                }
                else {
                    if (cmd != 'L') {
                        cmd = 'L';
                        ss << cmd;
                    }
                    ss << pts[i].x << "," << pts[i].y << " ";
                }
                break;
                
            case kMgBezierTo:
                if (i + 2 >= n) {
                    break;
                }
                if (curpt + path.getEndTangent() == pts[i]) {
                    if (cmd != 'S') {
                        cmd = 'S';
                        ss << cmd;
                    }
                    ss << pts[i+1].x << "," << pts[i+1].y;
                    ss << " " << pts[i+2].x << "," << pts[i+2].y << " ";
                } else {
                    if (cmd != 'C') {
                        cmd = 'C';
                        ss << cmd;
                    }
                    ss << pts[i].x << "," << pts[i].y;
                    ss << " " << pts[i+1].x << "," << pts[i+1].y;
                    ss << " " << pts[i+2].x << "," << pts[i+2].y << " ";
                }
                i += 2;
                break;
                
            case kMgQuadTo:
                if (i + 1 >= n) {
                    break;
                }
                if (curpt + path.getEndTangent() == pts[i]) {
                    if (cmd != 'T') {
                        cmd = 'T';
                        ss << cmd;
                    }
                    ss << pts[i+1].x << "," << pts[i+1].y << " ";
                } else {
                    if (cmd != 'Q') {
                        cmd = 'Q';
                        ss << cmd;
                    }
                    ss << pts[i].x << "," << pts[i].y;
                    ss << " " << pts[i+1].x << "," << pts[i+1].y << " ";
                }
                i++;
                break;
                
            default:
                break;
        }
        if (types[i] & kMgCloseFigure) {
            cmd = 'Z';
            ss << "Z ";
        }
        curpt = path.getPoint(i);
    }
}

bool MgPathShape::_save(MgStorage* s) const
{
    bool ret = __super::_save(s);
    std::stringstream ss;
    
    exportPath(ss, _path);
    s->writeString("d", ss.str().c_str());
    
    return ret;
}

int MgPathShape::exportSVGPath(char* buf, int size) const
{
    return exportSVGPath(_path, buf, size);
}

int MgPathShape::exportSVGPath(const MgPath& path, char* buf, int size)
{
    std::stringstream ss;
    
    exportPath(ss, path);
    if (!buf || size < 1)
        return (int)ss.str().size();
    
    std::string str(ss.str());
    size = mgMin(size, (int)str.size());
#if defined(_MSC_VER) && _MSC_VER >= 1400 // VC8
    strncpy_s(buf, size, str.c_str(), size);
#else
    strncpy(buf, str.c_str(), size);
#endif
    
    return size;
}

bool MgPathShape::_load(MgShapeFactory* factory, MgStorage* s)
{
    bool ret = __super::_load(factory, s);
    int len = s->readString("d", (char*)0, 0);
    
    if (!ret || len < 1)
        return false;
    
    char *buf = new char[1 + len];
    
    len = s->readString("d", buf, len);
    buf[len] = 0;
    ret = importSVGPath(buf);
    
    delete[] buf;
    
    return ret;
}

bool MgPathShape::importSVGPath(const char* d)
{
    _path.clear();
    return _path.addSVGPath(d).getCount() > 0;
}
