#!/bin/sh
# Type './build.sh' to generate C# classes.

rm -rf ../democmdslib/core/*.*
swig -c++ -csharp -namespace democmds.core \
    -outdir ../democmdslib/core \
    -o democmds_csharp_wrap.cpp \
    -I../../../TouchVGCore/core/include \
    -I../../core/gate \
    ../../core/gate/democmds.i
