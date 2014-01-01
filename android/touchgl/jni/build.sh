#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
if [ "$1"x = "-swig"x ] || [ ! -f touchgl_java_wrap.cpp ] ; then # Make JNI classes
    mkdir -p ../src/touchgl/core
    rm -rf ../src/touchgl/core/*.*
    
    swig -c++ -java -package touchgl.core -D__ANDROID__ \
        -outdir ../src/touchgl/core \
        -o touchgl_java_wrap.cpp \
        -I../../../thirdparty/TouchVGCore/core/include \
        -I../../../thirdparty/TouchVGCore/core/include/canvas \
        -I../../../thirdparty/TouchVGCore/core/include/graph \
        -I../../../thirdparty/TouchVGCore/core/include/cmd \
        -I../../../thirdparty/TouchVGCore/core/include/storage \
        -I../../../thirdparty/TouchVGCore/core/include/view \
        -I../../../core/include/touchgl \
          ../../../core/src/touchgl/touchgl.swig
    python replacejstr.py
fi
ndk-build
