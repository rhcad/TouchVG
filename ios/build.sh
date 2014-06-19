#!/bin/sh
# Type './build.sh' to make iOS libraries.
# Type './build.sh -arch arm64' to make iOS libraries for iOS 64-bit.
# Type './build.sh clean' to remove object files.

if [ ! -f ../../TouchVGCore/ios/build.sh ] ; then
    git clone https://github.com/touchvg/TouchVGCore ../../TouchVGCore
fi
if [ ! -f ../../SVGKit/SVGKit.podspec ] ; then
    git clone https://github.com/SVGKit/SVGKit ../../SVGKit
fi

iphoneos71=`xcodebuild -showsdks | grep -i iphoneos7.1`
iphoneos70=`xcodebuild -showsdks | grep -i iphoneos7.0`
iphoneos61=`xcodebuild -showsdks | grep -i iphoneos6.1`
iphoneos51=`xcodebuild -showsdks | grep -i iphoneos5.1`
iphoneos43=`xcodebuild -showsdks | grep -i iphoneos4.3`

if [ -n "$iphoneos71" ]; then
    xcodebuild -project TouchVG.xcodeproj $1 $2 -sdk iphoneos7.1 -configuration Release -alltargets
else
if [ -n "$iphoneos70" ]; then
    xcodebuild -project TouchVG.xcodeproj $1 $2 -sdk iphoneos7.0 -configuration Release -alltargets
else
if [ -n "$iphoneos61" ]; then
    xcodebuild -project TouchVG.xcodeproj $1 $2 -sdk iphoneos6.1 -configuration Release -alltargets
else
if [ -n "$iphoneos51" ]; then
    xcodebuild -project TouchVG.xcodeproj $1 $2 -sdk iphoneos5.1 -configuration Release
else
if [ -n "$iphoneos43" ]; then
    xcodebuild -project TouchVG.xcodeproj $1 $2 -sdk iphoneos4.3 -configuration Release
fi
fi
fi
fi
fi

mkdir -p output/TouchVG
cp -R build/Release-universal/*.a output
cp -R build/Release-universal/include/TouchVG/*.h output/TouchVG
