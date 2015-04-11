// Copyright (c) 2013-2014, Zhang Yungui, https://github.com/rhcad/vgcore

#ifdef SWIGJAVA
%javaconst(1);
%insert("runtime") %{
#ifdef __ANDROID__
#define SWIG_JAVA_NO_DETACH_CURRENT_THREAD
#define SWIG_JAVA_ATTACH_CURRENT_THREAD_AS_DAEMON
#endif
%}
#endif

%module(directors="1") touchvg

%{
#include <mgbox.h>
#include <mgmat.h>
#include <mgbase.h>
#include <mgcurv.h>
#include <mglnrel.h>
#include <mgnear.h>

#include <gigraph.h>
#include <gicanvas.h>
#include <mgpath.h>

#include <mgstorage.h>
#include <mgvector.h>
#include <mgjsonstorage.h>

#include <mgcshapes.h>
#include <mgshapetype.h>
#include <mgarc.h>
#include <mgdiamond.h>
#include <mgdot.h>
#include <mgellipse.h>
#include <mggrid.h>
#include <mgline.h>
#include <mglines.h>
#include <mgparallel.h>
#include <mgpathsp.h>
#include <mgrdrect.h>
#include <mgrect.h>
#include <mgsplines.h>

#include <mgshapes.h>
#include <mgcomposite.h>
#include <mgspfactory.h>
#include <mgimagesp.h>

#include <mgsnap.h>
#include <mgselect.h>
#include <mgaction.h>
#include <mgcmd.h>
#include <cmdsubject.h>

#include <mgdrawarc.h>
#include <mgdrawrect.h>

#include <mgcmdmgr.h>
#include <mgcmdmgrfactory.h>

#include <mglayer.h>
#include <mgshapedoc.h>

#include "gicoreview.h"
#include "gimousehelper.h"
#include "testcanvas.h"
#include "giplaying.h"
#include "gicoreviewdata.h"
%}

%include <mgdef.h>
%include <mgtol.h>
%include <mgvec.h>
%include <mgpnt.h>
%include <mgmat.h>
%include <mgbox.h>

%include <mgbase.h>
%include <mgcurv.h>
%include <mglnrel.h>
%include <mgnear.h>

%feature("director") GiCanvas;
%include <gicanvas.h>

%include <gicolor.h>
%include <gicontxt.h>
%include <gixform.h>
%include <mgpath.h>
%include <gigraph.h>

%include <mgvector.h>
%template(Ints) mgvector<int>;
%template(Longs) mgvector<long>;
%template(Floats) mgvector<float>;
%template(Chars) mgvector<char>;
%template(ConstShapes) mgvector<const MgShape*>;
%template(Shapes) mgvector<MgShape*>;

%include <mgstorage.h>
%include <mgjsonstorage.h>

%feature("director") MgObject;
%feature("director") MgBaseShape;
%feature("director") MgBaseRect;
%feature("director") MgBaseLines;
%feature("director") MgComposite;

%include <mgobject.h>
%include <mgbasesp.h>
%include <mgrect.h>
%include <mglines.h>
%include <mgcshapes.h>
%include <mgshapetype.h>
%include <mgarc.h>
%include <mgdiamond.h>
%include <mgdot.h>
%include <mgellipse.h>
%include <mggrid.h>
%include <mgline.h>
%include <mgparallel.h>
%include <mgpathsp.h>
%include <mgrdrect.h>
%include <mgsplines.h>

%include <mgshape.h>
%include <mgspfactory.h>
%include <mgshapes.h>
%include <mgcomposite.h>
%include <mgimagesp.h>

%include <mgaction.h>
%include <mgsnap.h>
%include <mgselect.h>
%include <mgview.h>

%feature("director") MgCommand;
%include <mgcmd.h>

%feature("director") CmdObserverDefault;
%include <cmdobserver.h>
%include <cmdsubject.h>

%feature("director") MgCommandDraw;
%feature("director") MgCmdDrawRect;
%include <mgcmddraw.h>
%include <mgdrawarc.h>
%include <mgdrawrect.h>

%include <mgcmdmgr.h>
%include <mgcmdmgrfactory.h>

%include <mglayer.h>
%include <mgshapedoc.h>

%feature("director") GiView;
%include "giview.h"

%feature("director") MgFindImageCallback;
%feature("director") MgStringCallback;
%feature("director") MgOptionCallback;
%include "mgstrcallback.h"
%include "mgcoreview.h"
%include "gigesture.h"
%include "gicoreview.h"
%include "testcanvas.h"
%include "giplaying.h"
%include "gicoreviewdata.h"
%include "recordshapes.h"

#ifndef __ANDROID__
%include "gimousehelper.h"
#endif

#ifdef SWIGJAVA
%{
#ifdef __ANDROID__
#include <android/log.h>
struct TmpLogFunc {
    const char* name;
    TmpLogFunc(const char* name) : name(name) {
        __android_log_print(ANDROID_LOG_VERBOSE,"vgstack","%s enter",name);
    }
    ~TmpLogFunc() {
        __android_log_print(ANDROID_LOG_VERBOSE,"vgstack","%s exit",name);
    }
};
#endif

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* /*ajvm*/, void* /*reserved*/) {
    return JNI_VERSION_1_6;
}
%}
#endif