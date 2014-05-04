#!/bin/sh
# Type './build.sh' to make Android native libraries.
# Type `./build.sh -swig` to re-generate JNI classes too.
#
cd ../../TouchVGCore/android; sh build.sh; cd ../../DemoCmds/android
cd DemoCmds/jni; sh build.sh $1; cd ../..
