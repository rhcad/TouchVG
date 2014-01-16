#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
if [ "$1"x = "-swig"x ] || [ ! -f touchvg_java_wrap.cpp ] ; then # Make JNI classes
    mkdir -p ../src/rhcad/touchvg/core
    rm -rf ../src/rhcad/touchvg/core/*.*
    
    swig -c++ -java -package rhcad.touchvg.core -D__ANDROID__ \
        -outdir ../src/rhcad/touchvg/core \
        -o touchvg_java_wrap.cpp \
        -I../../../thirdparty/TouchVGCore/core/include \
        -I../../../thirdparty/TouchVGCore/core/include/canvas \
        -I../../../thirdparty/TouchVGCore/core/include/graph \
        -I../../../thirdparty/TouchVGCore/core/include/cmd \
        -I../../../thirdparty/TouchVGCore/core/include/storage \
        -I../../../thirdparty/TouchVGCore/core/include/geom \
        -I../../../thirdparty/TouchVGCore/core/include/shape \
        -I../../../thirdparty/TouchVGCore/core/include/cmdobserver \
        -I../../../thirdparty/TouchVGCore/core/include/cmdbase \
        -I../../../thirdparty/TouchVGCore/core/include/test \
        -I../../../thirdparty/TouchVGCore/core/include/view \
        -I../../../thirdparty/TouchVGCore/core/include/cmdbasic \
        -I../../../thirdparty/TouchVGCore/core/include/shapedoc \
        -I../../../thirdparty/TouchVGCore/core/include/jsonstorage \
        -I../../../thirdparty/TouchVGCore/core/include/cmdmgr \
          ../../../thirdparty/TouchVGCore/core/src/view/touchvg.swig
    python replacejstr.py
fi
ndk-build
