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
        -outdir ../src/rhcad/touchvg/core \
        -o touchvg_java_wrap.cpp \
        -I../../../../TouchVGCore/core/include \
        -I../../../../TouchVGCore/core/include/canvas \
        -I../../../../TouchVGCore/core/include/graph \
        -I../../../../TouchVGCore/core/include/cmd \
        -I../../../../TouchVGCore/core/include/storage \
        -I../../../../TouchVGCore/core/include/geom \
        -I../../../../TouchVGCore/core/include/shape \
        -I../../../../TouchVGCore/core/include/cmdobserver \
        -I../../../../TouchVGCore/core/include/cmdbase \
        -I../../../../TouchVGCore/core/include/test \
        -I../../../../TouchVGCore/core/include/view \
        -I../../../../TouchVGCore/core/include/cmdbasic \
        -I../../../../TouchVGCore/core/include/shapedoc \
        -I../../../../TouchVGCore/core/include/jsonstorage \
        -I../../../../TouchVGCore/core/include/cmdmgr \
        -I../../../../TouchVGCore/core/include/record \
        -I../../../../TouchVGCore/core/src/view \
          ../../../../TouchVGCore/core/src/view/touchvg.swig
    python replacejstr.py
    #python addlog.py
fi
if [ "$1"x = "-swig"x ] ; then
    ndk-build $2
else
    ndk-build $1 $2
fi
