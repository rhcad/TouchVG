#!/bin/sh
#
if [ ! -f touchvg_java_wrap.cpp ]; then # Make JNI classes
    mkdir -p ../src/touchvg/core
    rm -rf ../src/touchvg/core/*.*
    
    swig -c++ -java -package touchvg.core -D__ANDROID__ \
        -outdir ../src/touchvg/core \
        -o touchvg_java_wrap.cpp \
        -I../../../core/pub_inc \
        -I../../../core/pub_inc/canvas \
        -I../../../core/pub_inc/graph \
        -I../../../core/pub_inc/cmd \
        -I../../../core/mgr_inc/test \
        -I../../../core/mgr_inc/view \
        -I../../../core/pub_inc/storage \
        -I../../../core/pub_inc/geom \
        -I../../../core/pub_inc/shape \
        -I../../../core/pub_inc/cmdobserver \
        -I../../../core/pub_inc/cmdbase \
        -I../../../core/mgr_inc/cmdbasic \
        -I../../../core/mgr_inc/shapedoc \
        -I../../../core/mgr_inc/json \
        -I../../../core/mgr_inc/cmdmgr \
        ../../../core/mgr_src/view/touchvg.i
fi
ndk-build # Make libtouchvg.so
