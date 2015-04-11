#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type './build.sh -B' to rebuild the native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
# Type `./build.sh APP_ABI=x86` to build for the x86 Emulator.
#

cd TouchVGCore/jni; sh build.sh $1 $2; cd ../..
cd TouchVG/jni; sh build.sh $1 $2; cd ../..

mkdir -p output/armeabi
cp -vR TouchVG/bin/touchvg.jar             output
cp -vR TouchVG/libs/armeabi                output
cp -vR TouchVG/libs/x86                    output
