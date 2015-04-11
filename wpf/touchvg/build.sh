#!/bin/sh
# Type './build.sh' to generate C# classes.

rm -rf ../touchvglib/core/*.*
mkdir -p ../touchvglib/core

swig -c++ -csharp -namespace touchvg.core \
    -outdir ../touchvglib/core \
    -o touchvg_csharp_wrap.cpp \
    -I../../core/include \
    -I../../core/include/canvas \
    -I../../core/include/graph \
    -I../../core/include/cmd \
    -I../../core/include/storage \
    -I../../core/include/geom \
    -I../../core/include/gshape \
    -I../../core/include/shape \
    -I../../core/include/cmdobserver \
    -I../../core/include/cmdbase \
    -I../../core/include/test \
    -I../../core/include/view \
    -I../../core/include/cmdbasic \
    -I../../core/include/shapedoc \
    -I../../core/include/jsonstorage \
    -I../../core/include/cmdmgr \
    -I../../core/include/record \
    -I../../core/src/view \
      ../../core/src/view/touchvg.swig
