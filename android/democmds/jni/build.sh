#!/bin/sh
#
if [ ! -f democmds_java_wrap.cpp ]; then # Make JNI classes
    mkdir -p ../src/democmds/core
    rm -rf ../src/democmds/core/*.*
    
    swig -c++ -java -package democmds.core -D__ANDROID__ \
        -outdir ../src/democmds/core \
        -o democmds_java_wrap.cpp \
        -I../../../core/pub_inc \
        -I../../../core/pub_inc/cmd \
        -I../../../core/democmds/gate \
        ../../../core/democmds/gate/democmds.i
fi
ndk-build # Make libdemocmds.so
