#!/bin/sh
# Type 'sh build.sh' to make native libraries for Android applications.
#
cd core/jni; sh build.sh; cd ../..
cd democmds/jni; sh build.sh; cd ../..

mkdir -p test/libs/armeabi
cp -v core/libs/armeabi/libtouchvg.so test/libs/armeabi
cp -v democmds/libs/armeabi/libdemocmds.so test/libs/armeabi
cp -v core/bin/touchvg.jar test/libs
cp -v democmds/bin/democmds.jar test/libs