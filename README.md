# TouchVG

TouchVG is a lightweight 2D vector drawing framework mainly using C++ for iOS, Android and Windows.

Features described in [Online document](http://touchvg.github.io).

![arch](http://touchvg.github.io/images/arch.svg)

![iphone1](/doc/images/iphone1.png) | ![android1](/doc/images/android1.png) | ![iphone2](/doc/images/iphone2.png)

## License

This is an open source [LGPL 2.1](https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html) licensed project. It uses the following open source projects:

- [TouchVGCore](https://github.com/touchvg/vgcore) (LGPL): Cross-platform vector drawing libraries using C++.
- [svg-android](https://github.com/japgolly/svg-android) (Apache): Vector graphics support for Android.
- [SVGKit](https://github.com/SVGKit/SVGKit) (MIT): Display and interact with SVG Images with CoreAnimation on iOS.
- [simple-svg](http://code.google.com/p/simple-svg) (BSD): A C++ header file for creating SVG files.
- [rapidjson](https://github.com/Kanma/rapidjson) (MIT): A fast JSON parser/generator for C++ with both SAX/DOM style API.
- [x3py](https://github.com/rhcad/x3py) (Apache): Compile script files.
- [SWIG](https://github.com/swig/swig) (GPL): Use the tool to generate the glue code for Java and C#.
- [iOS-Universal-Library-Template](https://github.com/michaeltyson/iOS-Universal-Library-Template): Use it to create static library project.
- Algorithms: [NearestPoint.c](http://tog.acm.org/resources/GraphicsGems/gems/NearestPoint.c), 
[Bezier's bound box](http://processingjs.nihongoresources.com/bezierinfo/#bounds), 
[The intersection of two circles](http://blog.csdn.net/cyg0810/article/details/7765894), 
[Position judgment](http://orion.math.iastate.edu/burkardt/c_src/orourke/tri.c)
 and [Fitting digitized curves](https://github.com/erich666/GraphicsGems/blob/master/gems/FitCurves.c).

## How to Contribute

Contributors and sponsors are welcome. You may translate, commit issues or pull requests on this Github site.
To contribute, please follow the branching model outlined here: [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/).

Welcome to the Chinese QQ group `192093613` to discuss and share.

## Contributors

- [Zhang Yungui](https://github.com/rhcad)
- [Archer](https://github.com/a7ch3r)
- [ljlin](https://github.com/ljlin)
- [Pengjun](https://github.com/pengjun) / Line and triangle commands
- [Proteas](https://github.com/proteas)

# How to Compile

## Compile for Android

- Import all projects under `./android` directory of TouchVG in eclipse, then run `VGTest` or `vgdemo1` project to view the demonstration.

  - Android SDK version of the projects may need to modify according to your installation.
  - Recommend using the newer [ADT Bundle](http://developer.android.com/sdk/index.html) to avoid complex configuration.

-  You may replace full text and file names of `democmds` library project (for example, using UltraEdit) for your application. And you can add your shape classes or drawing commands. Thus, the TouchVG library does not require changes.

   - Do not want to write C++ code? Please reference to `android/test/src/vgtest/testview/shape` package to write your own shapes and commands.

-  To regenerate libtouchvg.so and libdemocmds.so, please enter `android` directory of TouchVG, then type `./build.sh`
(Need to add the [NDK](http://developer.android.com/tools/sdk/ndk/index.html) installation location to your PATH environment variable).

   - If the error `build/gmsl/__gmsl:512: *** non-numeric second argument to wordlist function` occurs, then open the `build/gmsl/__gmsl` file in the NDK installation directory, and change line 512 to:
     `int_encode = $(__gmsl_tr1)$(wordlist 1,$(words $1),$(__gmsl_input_int))`

   - MSYS and TDM-GCC(a MinGW distribution) are recommended on Windows.

   - To regenerate the kernel JNI classes, type `./build.sh-swig`
(Need to install [SWIG](http://sourceforge.net/projects/swig/files/), and add the location to PATH).

## Compile for iOS

-  Open `ios/TestVG.xcworkspace` in Xcode, then run the `TestView` demo app.

   - The `TestView` project has two targets:
   
     - `TestView` target using `libTouchVG.a` does not support SVG display.
     - `TestView-SVG` target using `libTouchVG-SVG.a` and `SVGKit` can display SVG shapes.

   - Static libraries required can be compiled in two ways:
   
        - Enter `ios` directory, then type `./build.sh` (Need to configure Xcode command line environment) to compile all static libraries to the `ios/output` directory.
          - Type `./build.sh -arch arm64` to make for iOS 64-bit.
          - Type `./build.sh clean` to remove object files.
        - Or select and build each library project in Xcode IDE.

   - To run on device, you may need to change the Bundle Identifier of the demo application, such as "com.yourcompany.TestView", and choose your own development certificate (Code Signing).

-  You may replace full text and file names of `democmds` library project (for example, using UltraEdit) for your application. And you can add your shape classes or drawing commands. Thus, the TouchVG library does not require changes.

## Compile for Windows

- Open `wpf/Test_cs10.sln` in Visual Studio 2010 (Need VC++ and C#), then run the`WpfDemo` application. Or open `wpf/Test_cs9.sln` in VS2008.

## Compile for other platform

- You can compile TouchVG for Python, Perl or Java applications on Linux, MinGW or Mac OS X.

  - Enter `core` directory which contains Makefile, then type the following make command:

     - `Make all install`: compile C + + static library .
     - `Make java`: Jar package and generate dynamic libraries for Java programs.
     - `Make python`, `make perl`: namely Python, Perl , etc. to generate class files and dynamic libraries.
     - `Make clean java.clean python.clean`: delete these temporary files compiled out .

   - MSYS and TDM-GCC(a MinGW distribution) are recommended on Windows.
