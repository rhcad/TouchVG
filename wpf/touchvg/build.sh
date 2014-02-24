#!/bin/sh
# Type './build.sh' to generate C# classes.

rm -rf ../touchvglib/core/*.*

swig -c++ -csharp -namespace touchvg.core \
    -outdir ../touchvglib/core \
    -o touchvg_csharp_wrap.cpp \
    -I../../thirdparty/TouchVGCore/core/include \
    -I../../thirdparty/TouchVGCore/core/include/canvas \
    -I../../thirdparty/TouchVGCore/core/include/graph \
    -I../../thirdparty/TouchVGCore/core/include/cmd \
    -I../../thirdparty/TouchVGCore/core/include/storage \
    -I../../thirdparty/TouchVGCore/core/include/geom \
    -I../../thirdparty/TouchVGCore/core/include/shape \
    -I../../thirdparty/TouchVGCore/core/include/cmdobserver \
    -I../../thirdparty/TouchVGCore/core/include/cmdbase \
    -I../../thirdparty/TouchVGCore/core/include/test \
    -I../../thirdparty/TouchVGCore/core/include/view \
    -I../../thirdparty/TouchVGCore/core/include/cmdbasic \
    -I../../thirdparty/TouchVGCore/core/include/shapedoc \
    -I../../thirdparty/TouchVGCore/core/include/jsonstorage \
    -I../../thirdparty/TouchVGCore/core/include/cmdmgr \
    ../../thirdparty/TouchVGCore/core/src/view/touchvg.swig
