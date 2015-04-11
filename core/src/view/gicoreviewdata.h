//! \file gicoreviewdata.h
//! \brief 定义GiCoreView内部数据类 GiCoreViewData
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_CORE_VIEWDATA_H
#define TOUCHVG_CORE_VIEWDATA_H

#include "recordshapes.h"
#include "giplaying.h"
#include "mgview.h"
#include "mgspfactory.h"
#include <algorithm>

//! 可并行播放的图形数据类
struct GiPlayShapes
{
    GiPlaying*      playing;
    MgRecordShapes* player;
    GiPlayShapes() : playing(NULL), player(NULL) {}
};

//! GiCoreView内部数据类
class GiCoreViewData : public MgView
{
public:
#ifndef SWIG
    volatile long   startPauseTick;
#endif
    GiPlaying*      drawing;
    MgShapeDoc*     backDoc;
    GiPlayShapes    play;
private:
    MgRecordShapes* _recorder[2];
    std::vector<GiPlaying*> playings;
    
public:
    virtual void submitBackXform() = 0;         //!< 应用后端坐标系对象到前端
    
    static GiCoreViewData* fromHandle(long h) { GiCoreViewData* p; *(long*)&p = h; return p; } //!< 转为对象
    GiCoreViewData() : startPauseTick(0) {
        _recorder[0] = _recorder[1] = NULL;
    }
    ~GiCoreViewData() {
        for (unsigned j = 0; j < playings.size(); j++) {
            playings[j]->release(NULL);
        }
    }
    MgRecordShapes* recorder(bool forUndo) {
        return _recorder[forUndo ? 0 : 1];
    }
    void setRecorder(bool forUndo, MgRecordShapes* p) {
        delete _recorder[forUndo ? 0 : 1];
        _recorder[forUndo ? 0 : 1] = p;
    }
    int getPlayingCount() {
        return (int)playings.size();
    }
    long acquireFrontDoc(int index) {
        return playings[index]->acquireFrontDoc();
    }
    long acquireFrontShapes(int index) {
        return playings[index]->acquireFrontShapes();
    }
    void addPlaying(GiPlaying* p) {
        playings.push_back(p);
    }
    void removePlaying(GiPlaying* p) {
        playings.erase(std::find(playings.begin(), playings.end(), p));
    }
};

#endif // TOUCHVG_CORE_VIEWDATA_H
