#ifndef _TEST_RANDOMSHAPE_H
#define _TEST_RANDOMSHAPE_H

#include "mgshapes.h"
#include "mgshape.h"

struct RandomParam
{
    int lineCount;
    int rectCount;
    int arcCount;
    int curveCount;
    bool randomLineStyle;
    bool  fill;
    
    static void init();

    RandomParam(int n = 1000) : lineCount(n), rectCount(n), arcCount(n)
    , curveCount(n), randomLineStyle(false), fill(false) {}

    int getShapeCount() const { return lineCount + rectCount + arcCount + curveCount; }
    int addShapes(MgShapes* shapes);
    void setShapeProp(GiContext& ctx);

    static float RandF(float fMin, float fMax);
    static int RandInt(int nMin, int nMax);
    static unsigned char RandUInt8(int nMin, int nMax);
};

#endif // _TEST_RANDOMSHAPE_H
