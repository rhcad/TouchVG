#!/bin/sh
# Type './build.sh' to generate C# classes.

rm -rf ../touchvglib/core/*.*

swig -c++ -csharp -namespace touchvg.core \
    -outdir ../touchvglib/core \
    -o touchvg_csharp_wrap.cpp \
    -I../../../TouchVGCore/core/include \
    -I../../../TouchVGCore/core/include/canvas \
    -I../../../TouchVGCore/core/include/graph \
    -I../../../TouchVGCore/core/include/cmd \
    -I../../../TouchVGCore/core/include/storage \
    -I../../../TouchVGCore/core/include/geom \
    -I../../../TouchVGCore/core/include/shape \
    -I../../../TouchVGCore/core/include/cmdobserver \
    -I../../../TouchVGCore/core/include/cmdbase \
    -I../../../TouchVGCore/core/include/test \
    -I../../../TouchVGCore/core/include/view \
    -I../../../TouchVGCore/core/include/cmdbasic \
    -I../../../TouchVGCore/core/include/shapedoc \
    -I../../../TouchVGCore/core/include/jsonstorage \
    -I../../../TouchVGCore/core/include/cmdmgr \
    -I../../../TouchVGCore/core/include/record \
    -I../../../TouchVGCore/core/src/view \
    ../../../TouchVGCore/core/src/view/touchvg.swig
