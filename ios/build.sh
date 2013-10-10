#!/bin/sh
# Type 'sh build.sh' to make iOS libraries.

iphoneos61=`xcodebuild -showsdks | grep -i iphoneos6.1`
iphoneos51=`xcodebuild -showsdks | grep -i iphoneos5.1`
iphoneos43=`xcodebuild -showsdks | grep -i iphoneos4.3`

if [ -n "$iphoneos61" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos6.1 -configuration Release -arch armv7
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos6.1 -configuration Release -arch armv7
else
if [ -n "$iphoneos51" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos5.1 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos5.1 -configuration Release
else
if [ -n "$iphoneos43" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos4.3 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos4.3 -configuration Release
fi
fi
fi

mkdir -p output/TouchVG
cp -R TouchVG/build/Release-universal/libTouchVG.a output
cp -R TouchVG/build/Release-universal/usr/local/include/*.h output/TouchVG

mkdir -p output/DemoCmds
cp -R DemoCmds/build/Release-universal/libDemoCmds.a output
cp -R DemoCmds/build/Release-universal/usr/local/include/*.h output/DemoCmds