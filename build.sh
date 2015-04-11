#!/bin/sh
# Type `./build.sh -swig` to re-generate JNI and C# classes.
# Type './build.sh' to make Android native libraries.
# Type `./build.sh APP_ABI=x86` to build for the x86 Emulator.
# Type './build.sh -B' to rebuild the native libraries.
#

cd android; sh build.sh $1 $2; cd ..

if [ "$1"x = "-swig"x ] ; then
    cd wpf; sh build.sh; cd ..
fi
