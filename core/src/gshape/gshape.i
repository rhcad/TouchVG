// Copyright (c) 2014, Zhang Yungui, https://github.com/rhcad/vgcore

#ifdef SWIGJAVA
%javaconst(1);
%insert("runtime") %{
#ifdef __ANDROID__
#define SWIG_JAVA_NO_DETACH_CURRENT_THREAD
#define SWIG_JAVA_ATTACH_CURRENT_THREAD_AS_DAEMON
#endif
    %}
#endif

%module(directors="1") gshape

%{
#include <mgbox.h>
#include <mgmat.h>
#include <mgbase.h>
#include <mgcurv.h>
#include <mglnrel.h>
#include <mgnear.h>
#include <mgpath.h>

#include <mgstorage.h>
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
%include <mgpath.h>

%feature("director") MgBaseShape;
%feature("director") MgBaseRect;
%feature("director") MgBaseLines;

%include <mgobject.h>
%include <mgbasesp.h>
%include <mgstorage.h>
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

#ifdef SWIGJAVA
%{
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* /*ajvm*/, void* /*reserved*/) {
    return JNI_VERSION_1_6;
}
%}
#endif
