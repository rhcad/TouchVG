// nanosvg.cpp: Implement MgPath::addSVGPath(s).
// Copyright (c) 2014, Zhang Yungui
// License: LGPL, https://github.com/rhcad/vgcore
//
// addSVGPath is based on nanosvg.h (https://github.com/memononen/nanosvg)
// NanoSVG is a simple stupid single-header-file SVG parse.
// Copyright (c) 2013-14 Mikko Mononen memon@inside.org

#include "mgpath.h"
#include <stdlib.h>
#include <string.h>

static int nsvg__isspace(char c)
{
	return strchr(" \t\n\v\f\r", c) != 0;
}

static int nsvg__isdigit(char c)
{
	return strchr("0123456789", c) != 0;
}

static int nsvg__isnum(char c)
{
	return strchr("0123456789+-.eE", c) != 0;
}

static const char* nsvg__getNextPathItem(const char* s, char* it)
{
	int i = 0;
	it[0] = '\0';
	// Skip white spaces and commas
	while (*s && (nsvg__isspace(*s) || *s == ',')) s++;
	if (!*s) return s;
	if (*s == '-' || *s == '+' || nsvg__isdigit(*s)) {
		// sign
		if (*s == '-' || *s == '+') {
			if (i < 63) it[i++] = *s;
			s++;
		}
		// integer part
		while (*s && nsvg__isdigit(*s)) {
			if (i < 63) it[i++] = *s;
			s++;
		}
		if (*s == '.') {
			// decimal point
			if (i < 63) it[i++] = *s;
			s++;
			// fraction part
			while (*s && nsvg__isdigit(*s)) {
				if (i < 63) it[i++] = *s;
				s++;
			}
		}
		// exponent
		if (*s == 'e' || *s == 'E') {
			if (i < 63) it[i++] = *s;
			s++;
			if (*s == '-' || *s == '+') {
				if (i < 63) it[i++] = *s;
				s++;
			}
			while (*s && nsvg__isdigit(*s)) {
				if (i < 63) it[i++] = *s;
				s++;
			}
		}
		it[i] = '\0';
	} else {
		// Parse command
		it[0] = *s++;
		it[1] = '\0';
		return s;
	}
    
	return s;
}

static int nsvg__getArgsPerElement(char cmd)
{
	switch (cmd) {
		case 'v':
		case 'V':
		case 'h':
		case 'H':
			return 1;
		case 'm':
		case 'M':
		case 'l':
		case 'L':
		case 't':
		case 'T':
			return 2;
		case 'q':
		case 'Q':
		case 's':
		case 'S':
			return 4;
		case 'c':
		case 'C':
			return 6;
		case 'a':
		case 'A':
			return 7;
	}
	return 0;
}

static float nsvg__sqr(float x) { return x*x; }
static float nsvg__vmag(float x, float y) { return sqrtf(x*x + y*y); }

static float nsvg__vecrat(float ux, float uy, float vx, float vy)
{
	return (ux*vx + uy*vy) / (nsvg__vmag(ux,uy) * nsvg__vmag(vx,vy));
}

static float nsvg__vecang(float ux, float uy, float vx, float vy)
{
	float r = nsvg__vecrat(ux,uy, vx,vy);
	if (r < -1.0f) r = -1.0f;
	if (r > 1.0f) r = 1.0f;
	return ((ux*vy < uy*vx) ? -1.0f : 1.0f) * acosf(r);
}

static void nsvg__xformPoint(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2] + t[4];
	*dy = x*t[1] + y*t[3] + t[5];
}

static void nsvg__xformVec(float* dx, float* dy, float x, float y, float* t)
{
	*dx = x*t[0] + y*t[2];
	*dy = x*t[1] + y*t[3];
}

static void nsvg__pathArcTo(MgPath& path, const float* args, bool rel)
{
	// Ported from canvg (https://code.google.com/p/canvg/)
	float rx, ry, rotx;
	float x1, y1, x2, y2, cx, cy, dx, dy, d;
	float x1p, y1p, cxp, cyp, s, sa, sb;
	float ux, uy, vx, vy, a1, da;
	float x, y, tanx, tany, a, px, py, ptanx, ptany, t[6];
	float sinrx, cosrx;
	int fa, fs;
	int i, ndivs;
	float hda, kappa;
    Point2d curpt(path.getEndPoint());
    
	rx = fabsf(args[0]);				// y radius
	ry = fabsf(args[1]);				// x radius
	rotx = args[2] * _M_D2R;            // x rotation engle
	fa = fabsf(args[3]) > 1e-6 ? 1 : 0;	// Large arc
	fs = fabsf(args[4]) > 1e-6 ? 1 : 0;	// Sweep direction
	x1 = curpt.x;						// start point
	y1 = curpt.y;
	if (rel) {							// end point
		x2 = curpt.x + args[5];
		y2 = curpt.y + args[6];
	} else {
		x2 = args[5];
		y2 = args[6];
	}
    
	dx = x1 - x2;
	dy = y1 - y2;
	d = sqrtf(dx*dx + dy*dy);
	if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f) {
		// The arc degenerates to a line
        path.lineTo(Point2d(x2, y2), rel);
		curpt.x = x2;
		curpt.y = y2;
		return;
	}
    
	sinrx = sinf(rotx);
	cosrx = cosf(rotx);
    
	// Convert to center point parameterization.
	// http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes
	// 1) Compute x1', y1'
	x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
	y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;
	d = nsvg__sqr(x1p)/nsvg__sqr(rx) + nsvg__sqr(y1p)/nsvg__sqr(ry);
	if (d > 1) {
		d = sqrtf(d);
		rx *= d;
		ry *= d;
	}
	// 2) Compute cx', cy'
	s = 0.0f;
	sa = nsvg__sqr(rx)*nsvg__sqr(ry) - nsvg__sqr(rx)*nsvg__sqr(y1p) - nsvg__sqr(ry)*nsvg__sqr(x1p);
	sb = nsvg__sqr(rx)*nsvg__sqr(y1p) + nsvg__sqr(ry)*nsvg__sqr(x1p);
	if (sa < 0.0f) sa = 0.0f;
	if (sb > 0.0f)
		s = sqrtf(sa / sb);
	if (fa == fs)
		s = -s;
	cxp = s * rx * y1p / ry;
	cyp = s * -ry * x1p / rx;
    
	// 3) Compute cx,cy from cx',cy'
	cx = (x1 + x2)/2.0f + cosrx*cxp - sinrx*cyp;
	cy = (y1 + y2)/2.0f + sinrx*cxp + cosrx*cyp;
    
	// 4) Calculate theta1, and delta theta.
	ux = (x1p - cxp) / rx;
	uy = (y1p - cyp) / ry;
	vx = (-x1p - cxp) / rx;
	vy = (-y1p - cyp) / ry;
	a1 = nsvg__vecang(1.0f,0.0f, ux,uy);	// Initial angle
	da = nsvg__vecang(ux,uy, vx,vy);		// Delta angle
    
    //	if (vecrat(ux,uy,vx,vy) <= -1.0f) da = NSVG_PI;
    //	if (vecrat(ux,uy,vx,vy) >= 1.0f) da = 0;
    
	if (fa) {
		// Choose large arc
		if (da > 0.0f)
			da = da - _M_2PI;
		else
			da = _M_2PI + da;
	}
    
	// Approximate the arc using cubic spline segments.
	t[0] = cosrx; t[1] = sinrx;
	t[2] = -sinrx; t[3] = cosrx;
	t[4] = cx; t[5] = cy;
    
	// Split arc into max 90 degree segments.
	ndivs = (int)(fabsf(da) / _M_PI_2 + 0.5f);
	hda = (da / (float)ndivs) / 2.0f;
	kappa = fabsf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));
	if (da < 0.0f)
		kappa = -kappa;
    
	for (i = 0; i <= ndivs; i++) {
		a = a1 + da * (i/(float)ndivs);
		dx = cosf(a);
		dy = sinf(a);
		nsvg__xformPoint(&x, &y, dx*rx, dy*ry, t); // position
		nsvg__xformVec(&tanx, &tany, -dy*rx * kappa, dx*ry * kappa, t); // tangent
		if (i > 0)
            path.bezierTo(Point2d(px+ptanx, py+ptany), Point2d(x-tanx, y-tany), Point2d(x, y), rel);
		px = x;
		py = y;
		ptanx = tanx;
		ptany = tany;
	}
    
	curpt.x = x2;
	curpt.y = y2;
}

MgPath& MgPath::addSVGPath(const char* s)
{
    char item[64];
    char cmd = 0;
	float args[10] = { 0, 0, 0, 0, 0, 0 };
	int nargs = 0;
	int rargs = 0;
    
    while (*s) {
        s = nsvg__getNextPathItem(s, item);
        if (!*item) break;
        if (!nsvg__isnum(item[0])) {
            cmd = item[0];
            rargs = nsvg__getArgsPerElement(cmd);
            nargs = 0;
            if (cmd == 'Z' || cmd == 'z') {
                closeFigure();
            }
        } else {
            if (nargs < 10)
                args[nargs++] = (float)atof(item);
            if (nargs >= rargs) {
                switch (cmd) {
                    case 'm':
                    case 'M':
                        moveTo(Point2d(args[0], args[1]), cmd == 'm');
                        // Moveto can be followed by multiple coordinate pairs,
                        // which should be treated as linetos.
                        cmd = (cmd == 'm') ? 'l' : 'L';
                        rargs = nsvg__getArgsPerElement(cmd);
                        break;
                    case 'l':
                    case 'L':
                        lineTo(Point2d(args[0], args[1]), cmd == 'l');
                        break;
                    case 'H':
                    case 'h':
                        horzTo(args[0], cmd == 'h');
                        break;
                    case 'V':
                    case 'v':
                        vertTo(args[0], cmd == 'v');
                        break;
                    case 'C':
                    case 'c':
                        bezierTo(Point2d(args[0], args[1]),
                                 Point2d(args[2], args[3]),
                                 Point2d(args[4], args[5]), cmd == 'c');
                        break;
                    case 'S':
                    case 's':
                        smoothBezierTo(Point2d(args[0], args[1]),
                                       Point2d(args[2], args[3]), cmd == 's');
                        break;
                    case 'Q':
                    case 'q':
                        quadTo(Point2d(args[0], args[1]),
                               Point2d(args[2], args[3]), cmd == 'q');
                        break;
                    case 'T':
                    case 't':
                        smoothQuadTo(Point2d(args[0], args[1]), cmd == 't');
                        break;
                    case 'A':
                    case 'a':
                        nsvg__pathArcTo(*this, args, cmd == 'a' ? 1 : 0);
                        break;
                    default:
                        break;
                }
                nargs = 0;
            }
        }
    }
    
    return *this;
}
