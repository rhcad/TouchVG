#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type './build.sh -B' to rebuild the native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
# Type `./build.sh APP_ABI=x86` to build for the x86 Emulator.
#
if [ "$1"x = "-swig"x ] || [ ! -f touchvg_java_wrap.cpp ] ; then # Make JNI classes
    mkdir -p ../src/rhcad/touchvg/core
    rm -rf ../src/rhcad/touchvg/core/*.*
    
    swig -c++ -java -package rhcad.touchvg.core -D__ANDROID__ \
        -features autodoc=1 \
        -outdir ../src/rhcad/touchvg/core \
        -o touchvg_java_wrap.cpp \
        -I../../../core/include \
        -I../../../core/include/canvas \
        -I../../../core/include/graph \
        -I../../../core/include/cmd \
        -I../../../core/include/storage \
        -I../../../core/include/geom \
        -I../../../core/include/gshape \
        -I../../../core/include/shape \
        -I../../../core/include/cmdobserver \
        -I../../../core/include/cmdbase \
        -I../../../core/include/test \
        -I../../../core/include/view \
        -I../../../core/include/cmdbasic \
        -I../../../core/include/shapedoc \
        -I../../../core/include/jsonstorage \
        -I../../../core/include/cmdmgr \
        -I../../../core/include/record \
        -I../../../core/src/view \
          ../../../core/src/view/touchvg.swig
    python replacejstr.py
    #python addlog.py
fi
if [ "$1"x = "-swig"x ] ; then
    ndk-build $2
else
    ndk-build $1 $2
fi
