// recordshapes.h
// Copyright (c) 2013-2014, Zhang Yungui
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#ifndef TOUCHVG_RECORD_SHAPES_H_
#define TOUCHVG_RECORD_SHAPES_H_

#include <string>
#include <vector>

class MgShapeDoc;
class MgShapes;
class MgShape;
struct MgShapeFactory;

//! Helper class for recording shapes.
class MgRecordShapes
{
public:
    enum { ADD = 1, EDIT = 2, DEL = 4, DYN = 8,
        DOC_CHANGED = 1, SHAPE_APPEND = 2, DYN_CHANGED = 4 };
    MgRecordShapes(const char* path, MgShapeDoc* doc, bool forUndo, long curTick);
    ~MgRecordShapes();
    
    long getCurrentTick(long curTick) const;
#ifndef SWIG
    bool recordStep(long tick, long changeCountOld, long changeCountNew, MgShapeDoc* doc,
                    MgShapes* dynShapes, const std::vector<MgShapes*>& extShapes);
    std::string getFileName(bool back, int index) const;
    std::string getPath() const;
#endif
    bool isLoading() const;
    void setLoading(bool loading);
    bool onResume(long ticks);
    void restore(int index, int count, int tick, long curTick);
    void stopRecordIndex();
    
#ifndef SWIG
    bool canUndo() const;
    bool canRedo() const;
    bool undo(MgShapeFactory *factory, MgShapeDoc* doc, long* changeCount);
    bool redo(MgShapeFactory *factory, MgShapeDoc* doc, long* changeCount);
    void resetDoc(MgShapeDoc* doc);
    int getMaxFileCount() const;
#endif
    
    bool isPlaying() const;
    int getFileTick() const;
    int getFileFlags() const;
    int getFileCount() const;
    bool applyFirstFile(MgShapeFactory *factory, MgShapeDoc* doc);
    bool applyFirstFile(MgShapeFactory *factory, MgShapeDoc* doc, const char* filename);
    int applyRedoFile(MgShapeFactory *f, MgShapeDoc* doc, MgShapes* dyns, int index);
    int applyUndoFile(MgShapeFactory *f, MgShapeDoc* doc, MgShapes* dyns, int index, long curTick);
#ifndef SWIG
    static bool loadFrameIndex(std::string path, std::vector<int>& arr);
#endif

private:
    static int applyFile(int& tick, MgShapeFactory *f,
                         MgShapeDoc* doc, MgShapes* dyns, const char* fn,
                         long* changeCount = NULL, MgShape* lastShape = NULL);
    
private:
    struct Impl;
    Impl* _im;
};

#endif // TOUCHVG_RECORD_SHAPES_H_
