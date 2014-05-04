#!/bin/sh
# Type './build.sh' to generate C# classes.

cd touchvg; sh build.sh; cd ..
cd ../thirdparty/DemoCmds/win; sh build.sh $1; cd ../../../wpf
