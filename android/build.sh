#!/bin/sh
# Type './build.sh' or './build.sh NDK_DEBUG=1' to make Android native libraries.
#
cd core/jni; ./build.sh $1 $2; cd ../..
cd democmds/jni; ./build.sh $1 $2; cd ../..

mkdir -p test/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so test/libs/armeabi
cp -v democmds/libs/armeabi/libdemocmds.so test/libs/armeabi
