#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
cd ../thirdparty/TouchVGCore/android; sh build.sh; cd ../../../android

cd core/jni; sh build.sh $1; cd ../..
cd ../thirdparty/DemoCmds/android; sh build.sh $1; cd ../../../android

mkdir -p test/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so test/libs/armeabi
cp -v ../thirdparty/DemoCmds/android/DemoCmds/libs/armeabi/libdemocmds.so test/libs/armeabi

mkdir -p vgdemo1/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so vgdemo1/libs/armeabi
