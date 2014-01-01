// Copyright (c) 2013, Zhang Yungui, https://github.com/rhcad/touchvg

#ifdef SWIGJAVA
%insert("runtime") %{
#ifdef __ANDROID__
#define SWIG_JAVA_NO_DETACH_CURRENT_THREAD
#define SWIG_JAVA_ATTACH_CURRENT_THREAD_AS_DAEMON
#endif
%}
#endif

%module(directors="1") touchgl

%{
#include <gicontxt.h>
#include <gicanvas.h>
#include <mgstorage.h>
#include "gicoreview.h"
#include "gimousehelper.h"
#include "touchglview.h"
%}

%include <gicanvas.h>

%include <gicolor.h>
%include <gicontxt.h>

%include <mgvector.h>
%template(Ints) mgvector<int>;
%template(Floats) mgvector<float>;

%include <mgstorage.h>

%feature("director") GiView;
%include "giview.h"

%include "mgcoreview.h"
%include "gigesture.h"
%include "gicoreview.h"

#ifndef __ANDROID__
%include "gimousehelper.h"
#endif
%include "touchglview.h"

#ifdef SWIGJAVA
%{
struct TmpJOBJ {
    JNIEnv* env;
    jobject* jo;
    template <class T>
    TmpJOBJ(JNIEnv* e, T* v) : env(e), jo((jobject*)v) { }
    TmpJOBJ(JNIEnv* e, jobject& v) : env(e), jo(&v) { }
    ~TmpJOBJ() { if (*jo) { env->DeleteLocalRef(*jo); *jo = 0; } }
};
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* /*ajvm*/, void* /*reserved*/) {
    return JNI_VERSION_1_6;
}
%}
#endif