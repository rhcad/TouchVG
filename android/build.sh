#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
cd ../thirdparty/TouchVGCore/android; ./build.sh; cd ../../../android

cd core/jni; ./build.sh $1; cd ../..
cd democmds/jni; ./build.sh $1; cd ../..

mkdir -p test/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so test/libs/armeabi
cp -v democmds/libs/armeabi/libdemocmds.so test/libs/armeabi

mkdir -p vgdemo1/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so vgdemo1/libs/armeabi
