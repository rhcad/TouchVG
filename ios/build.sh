#!/bin/sh
# Type './build.sh' to make iOS libraries.

iphoneos70=`xcodebuild -showsdks | grep -i iphoneos7.0`
iphoneos61=`xcodebuild -showsdks | grep -i iphoneos6.1`
iphoneos51=`xcodebuild -showsdks | grep -i iphoneos5.1`
iphoneos43=`xcodebuild -showsdks | grep -i iphoneos4.3`

if [ -n "$iphoneos70" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos7.0 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos7.0 -configuration Release
    xcodebuild -project TouchGL/TouchGL.xcodeproj -sdk iphoneos7.0 -configuration Release
else
if [ -n "$iphoneos61" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos6.1 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos6.1 -configuration Release
    xcodebuild -project TouchGL/TouchGL.xcodeproj -sdk iphoneos6.1 -configuration Release
else
if [ -n "$iphoneos51" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos5.1 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos5.1 -configuration Release
    xcodebuild -project TouchGL/TouchGL.xcodeproj -sdk iphoneos5.1 -configuration Release
else
if [ -n "$iphoneos43" ]; then
    xcodebuild -project TouchVG/TouchVG.xcodeproj -sdk iphoneos4.3 -configuration Release
    xcodebuild -project DemoCmds/DemoCmds.xcodeproj -sdk iphoneos4.3 -configuration Release
    xcodebuild -project TouchGL/TouchGL.xcodeproj -sdk iphoneos4.3 -configuration Release
fi
fi
fi
fi

mkdir -p output/TouchVG
cp -R TouchVG/build/Release-universal/libTouchVG.a output
cp -R TouchVG/build/Release-universal/TouchVG/include/*.h output/TouchVG

mkdir -p output/DemoCmds
cp -R DemoCmds/build/Release-universal/libDemoCmds.a output
cp -R DemoCmds/build/Release-universal/DemoCmds/include/*.h output/DemoCmds

mkdir -p output/TouchGL
cp -R TouchGL/build/Release-universal/libTouchGL.a output
cp -R TouchGL/build/Release-universal/TouchGL/include/*.h output/TouchGL
