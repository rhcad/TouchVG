#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
if [ "$1"x = "-swig"x ] || [ ! -f democmds_java_wrap.cpp ] ; then # Make JNI classes
    mkdir -p ../src/democmds/core
    rm -rf ../src/democmds/core/*.*
    
    swig -c++ -java -package democmds.core -D__ANDROID__ \
        -outdir ../src/democmds/core \
        -o democmds_java_wrap.cpp \
        -I../../../../TouchVGCore/core/include \
        -I../../../core/gate \
          ../../../core/gate/democmds.i
    python replacejstr.py
fi
ndk-build
