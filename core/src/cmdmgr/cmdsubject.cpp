// cmdsubject.cpp
// Copyright (c) 2004-2015, https://github.com/rhcad/vgcore, BSD License

#include "mgcmdmgr_.h"
#include "cmdsubject.h"
#include <vector>
#include <string>

//! CmdSubject 的实现类
class CmdSubjectImpl : public CmdSubject
{
public:
    CmdSubjectImpl() {}

private:
    virtual void registerObserver(CmdObserver* observer) {
        if (observer) {
            unregisterObserver(observer);
            _arr.push_back(Observer(observer, ""));
        }
    }
    virtual void unregisterObserver(CmdObserver* observer) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (it->first == observer) {
                _arr.erase(it);
                break;
            }
        }
    }
    virtual bool registerNamedObserver(const char* name, CmdObserver* observer) {
        bool ret = observer && name && *name && !findNamedObserver(name);
        if (ret) {
            unregisterObserver(observer);
            _arr.push_back(Observer(observer, name));
        }
        return ret;
    }
    virtual CmdObserver* findNamedObserver(const char* name) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (it->second == name) {
                return it->first;
            }
        }
        return (CmdObserver*)0;
    }
    
    virtual void onDocLoaded(const MgMotion* sender, bool forUndo) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onDocLoaded(sender, forUndo);
        }
    }
    virtual void onEnterSelectCommand(const MgMotion* sender) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onEnterSelectCommand(sender);
        }
    }
    virtual void onUnloadCommands(MgCmdManager* sender) {
        Observers arr(_arr);
        _arr.clear();
        for (Iterator it = arr.begin(); it != arr.end(); ++it) {
            it->first->onUnloadCommands(sender);
        }
    }
    virtual bool selectActionsNeedHided(const MgMotion* sender) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (it->first->selectActionsNeedHided(sender)) {
                return true;
            }
        }
        return false;
    }
    virtual int addShapeActions(const MgMotion* sender,
        mgvector<int>& actions, int n, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            n = it->first->addShapeActions(sender, actions, n, shape);
        }
        return n;
    }
    virtual bool doAction(const MgMotion* sender, int action) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (it->first->doAction(sender, action))
                return true;
        }
        return false;
    }
    virtual bool doEndAction(const MgMotion* sender, int action) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (it->first->doEndAction(sender, action))
                return true;
        }
        return false;
    }
    virtual void drawInShapeCommand(const MgMotion* sender, 
        MgCommand* cmd, GiGraphics* gs) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->drawInShapeCommand(sender, cmd, gs);
        }
    }
    virtual void drawInSelectCommand(const MgMotion* sender, 
        const MgShape* shape, int handleIndex, GiGraphics* gs) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->drawInSelectCommand(sender, shape, handleIndex, gs);
        }
    }
    virtual void onSelectTouchEnded(const MgMotion* sender, int shapeid,
        int handleIndex, int snapid, int snapHandle,
        int count, const int* ids)
    {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onSelectTouchEnded(sender, shapeid, 
                handleIndex, snapid, snapHandle, count, ids);
        }
    }
    virtual void onGatherSnapIgnoredID(const MgMotion* sender, const MgShape* sp,
                                       int* ids, int& i, int n) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onGatherSnapIgnoredID(sender, sp, ids, i, n);
        }
    }

    virtual void onSelectionChanged(const MgMotion* sender) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onSelectionChanged(sender);
        }
    }
    virtual bool onShapeWillAdded(const MgMotion* sender, MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeWillAdded(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual void onShapeAdded(const MgMotion* sender, MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onShapeAdded(sender, shape);
        }
    }
    virtual bool onShapeWillDeleted(const MgMotion* sender, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeWillDeleted(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual int onShapeDeleted(const MgMotion* sender, const MgShape* shape) {
        int n = 0;
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            n += it->first->onShapeDeleted(sender, shape);
        }
        return n;
    }
    virtual bool onShapeCanRotated(const MgMotion* sender, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeCanRotated(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual bool onShapeCanTransform(const MgMotion* sender, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeCanTransform(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual bool onShapeCanUnlock(const MgMotion* sender, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeCanUnlock(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual bool onShapeCanUngroup(const MgMotion* sender, const MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeCanUngroup(sender, shape)) {
                return false;
            }
        }
        return true;
    }
    virtual bool onShapeCanMovedHandle(const MgMotion* sender, const MgShape* sp, int index) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeCanMovedHandle(sender, sp, index)) {
                return false;
            }
        }
        return true;
    }
    virtual void onShapeMoved(const MgMotion* sender, MgShape* shape, int segment) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onShapeMoved(sender, shape, segment);
        }
    }
    virtual bool onShapeWillChanged(const MgMotion* sender, MgShape* sp, const MgShape* oldsp) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onShapeWillChanged(sender, sp, oldsp)) {
                return false;
            }
        }
        return true;
    }
    virtual void onShapeChanged(const MgMotion* sender, MgShape* shape) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onShapeChanged(sender, shape);
        }
    }

    virtual MgBaseShape* createShape(const MgMotion* sender, int type) {
        MgBaseShape* sp = (MgBaseShape*)0;
        for (Iterator it = _arr.begin(); !sp && it != _arr.end(); ++it) {
            sp = it->first->createShape(sender, type);
        }
        return sp;
    }

    virtual MgCommand* createCommand(const MgMotion* sender, const char* name) {
        MgCommand* cmd = (MgCommand*)0;
        for (Iterator it = _arr.begin(); !cmd && it != _arr.end(); ++it) {
            cmd = it->first->createCommand(sender, name);
        }
        return cmd;
    }
    
    virtual bool onPreGesture(MgMotion* sender) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            if (!it->first->onPreGesture(sender)) {
                return false;
            }
        }
        return true;
    }
    
    virtual void onPostGesture(const MgMotion* sender) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onPostGesture(sender);
        }
    }
    
    virtual void onPointSnapped(const MgMotion* sender, const MgShape* sp) {
        for (Iterator it = _arr.begin(); it != _arr.end(); ++it) {
            it->first->onPointSnapped(sender, sp);
        }
    }

private:
    typedef std::pair<CmdObserver*, std::string> Observer;
    typedef std::vector<Observer> Observers;
    typedef Observers::iterator Iterator;
    Observers _arr;
};

CmdSubject* MgCmdManagerImpl::getCmdSubject()
{
    if (!_subject) {
        _subject = new CmdSubjectImpl();
    }
    return _subject;
}

void MgCmdManagerImpl::freeSubject()
{
    if (_subject) {
        delete _subject;
        _subject = (CmdSubjectImpl*)0;
    }
}
