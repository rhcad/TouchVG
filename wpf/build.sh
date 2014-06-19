#!/bin/sh
# Type './build.sh' to generate C# classes.

if [ ! -f ../../TouchVGCore/ios/build.sh ] ; then
    git clone https://github.com/touchvg/TouchVGCore ../../TouchVGCore
fi
cd touchvg; sh build.sh; cd ..
