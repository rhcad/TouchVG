关于 TouchVG
------------
> TouchVG 是一个主要由C++开发的轻量级2D矢量绘图框架，可在 iOS、Android、Windows 等多个平台构建矢量绘图应用。
> 
> 作者：张云贵 <rhcad@hotmail.com> 开源协议：LGPL 2.1，见 LICENSE 文件描述。

![iphone1](http://git.oschina.net/rhcad/TouchVG/raw/master/doc/images/iphone1.png)  ![iphone2](http://git.oschina.net/rhcad/TouchVG/raw/master/doc/images/iphone2.png)  ![android1](http://git.oschina.net/rhcad/TouchVG/raw/master/doc/images/android1.png)

TouchVG 的功能
--------------
> 支持多点触摸或鼠标的交互绘图方式，可以无级放缩或平移显示矢量图形、图像、[SVG](http://zh.wikipedia.org/wiki/SVG)。

> 高性能绘图、快速手绘光滑曲线形状，在 iOS 上支持 CALayer 后台渲染和动态并行渲染，在 Android 上支持 SurfaceView 异步渲染。

> 除了基本的15余种矢量图形类型外，可使用C++或Java等来扩充更多的图形类型。

> 除了基本的20余种选择和绘图命令外，还可使用C++、Java等扩充更多的交互命令。

> 可以交互式修改图形的线宽、颜色等属性，可以进行放缩变形、克隆等图形操作。

> 支持JSON等格式的图形序列化操作，允许扩充XML、数据库等其他序列化适配器。

> 导出 SVG 矢量格式图片、PNG 透明背景图片。

> 无限步 Undo/Redo 记忆，轻数据量的矢量录屏和回放。

> 支持多模块的扩展机制，可构建数字教育、动态几何、图文笔记、会议白板等多种行业绘图模块。

> 更多说明将在TouchVG的 [在线文档](https://github.com/rhcad/touchvg-doc) 中补充，可看[最新功能的屏幕录像](http://www.soku.com/search_video/q_touchvg)。

引用的开源框架
--------------
> Larva Labs 的 Android SVG 解析框架 [svg-android](https://github.com/japgolly/svg-android) (Apache License 2.0)。

> iOS SVG 解析框架 [SVGKit](https://github.com/SVGKit/SVGKit) (MIT)。

> 用于输出 SVG 的简易框架 [simple-svg](http://code.google.com/p/simple-svg) (BSD-3)。

> 用于跨语言转换的 [SWIG](https://github.com/swig/swig)，用此工具生成粘合代码。

> iOS静态库模板 [iOS-Universal-Library-Template](https://github.com/michaeltyson/iOS-Universal-Library-Template)。

> JSON 解析和输出框架 [rapidjson](https://github.com/Kanma/rapidjson) (MIT)。

> GCC编译脚本 [x3py](https://github.com/rhcad/x3py) (Apache License 2.0)。

> 算法：[圆弧和Bezier曲线最近点计算](http://tog.acm.org/resources/GraphicsGems/gems/NearestPoint.c)、
[三次Bezier范围计算](http://processingjs.nihongoresources.com/bezierinfo/#bounds)、
[两圆求交点](http://blog.csdn.net/cyg0810/article/details/7765894)、
[点线位置判断](http://orion.math.iastate.edu/burkardt/c_src/orourke/tri.c)。

意见和建议
----------
> 在您使用 TouchVG 的过程中有任何意见和建议，请到此项目 [提交Issue](https://github.com/rhcad/touchvg/issues)。

捐款
-------
> 如果您在使用后感到满意，或者希望增加特别的功能、获得更多技术支持，请[为TouchVG项目捐款](https://me.alipay.com/rhcad)，也可扫下面的二维码捐款，以支持作者不断完善此项目。

> ![Yu_E_Bao](http://git.oschina.net/rhcad/TouchVG/raw/master/doc/images/yuebao.png)


Android编译说明
---------------

> 在 eclipse 中导入此项目下的所有工程，运行 VGTest 或 vgdemo1 工程可看演示效果。

>> 可能需要根据实际安装情况修改工程的SDK版本号。

>> 建议使用较新的 [ADT Bundle](http://developer.android.com/sdk/index.html)，省掉了eclipse和ADT的安装配置麻烦事。
    
> 可以将 democmds 改为你的应用库工程（例如使用UltraEdit保持大小写全文替换，并替换文件名），
该工程包含 democmds.jar 和 libdemocmds.so(本地库)。在此增加自己的图形类和命令类，这样就不需要改动 TouchVG 内核了。

>> 不写C++代码、不要democmds也行，参考 `android/test/src/vgtest/testview/shape`包写自己的图形和命令吧。

> 如需重新生成 libtouchvg.so 和 libdemocmds.so，则进入此项目的 android 目录，运行 `./build.sh`
（需要将[NDK](http://developer.android.com/tools/sdk/ndk/index.html)安装位置加到PATH环境变量，以便使用 ndk-build）。

>> 使用NDK-r8c以后的版本如果出现 `build/gmsl/__gmsl:512: *** non-numeric second argument to wordlist function` 错误，
则打开 NDK 安装目录下的 `build/gmsl/__gmsl` 文件，将512行改为：
    `int_encode = $(__gmsl_tr1)$(wordlist 1,$(words $1),$(__gmsl_input_int))`

>> 如果是在Windows上编译，推荐使用 MSYS（Unix模拟环境工具）、TDM-GCC（一种MinGW发行版）。

>> 如需重新生成内核JNI类，则运行 `./build.sh -swig`
（需要安装[SWIG](http://sourceforge.net/projects/swig/files/)，并将SWIG位置加到 PATH 环境变量）。

iOS编译说明
----------

> 在 Xcode 中打开 ios/TestVG.xcworkspace 文件，运行 TestView 演示程序。

>> 该程序有两个编译目标项，TestView 不支持SVG的显示，使用 libTouchVG.a；TestView-SVG 使用 SVGKit 库显示SVG图形，使用 libTouchVG-SVG.a。

>> 所需的多个静态库有两种编译方式：
>>> 进入此项目的 ios 目录，运行`./build.sh`（需要配置Xcode命令行环境，以便使用 xcodebuild）自动编译所有静态库到 `ios/output` 目录下。
>>> 在 Xcode 集成开发环境中分别选择相应的库工程编译。

>> 在真机上调试时需要修改主程序的Bundle Identifier，例如“com.yourcompany.TestView”，并选择自己的开发证书（Code Signing）。

> 可以将 DemoCmds 改为你的应用库工程（例如使用UltraEdit保持大小写全文替换，并替换文件名），
增加自己的图形类和命令类，这样就不需要改动 TouchVG 内核了。

WPF编译说明
-----------

> 在 Visual Studio 2010 中打开 `wpf/Test_cs10.sln`，运行 WpfDemo 工程。VS2008则打开 `wpf/Test_cs9.sln` 。

> 如需重新生成内核类（编译 Swig Files 下的 touchvg.swig 或 democmds.i），需要安装
[SwigWin](http://sourceforge.net/projects/swig/files/swigwin/)，并将SWIG解压位置加到 PATH 环境变量。
  
其他编译说明
------------
> 还可在 Linux、MinGW、Mac OS X 下编译静态库，或为 Python、Perl、Java 等应用编译动态库。

> 进入`core`目录，该目录下有 Makefile 文件，运行下列 make 指令：

>> `make all install`：编译C++静态库。

>> `make java`：为Java程序生成Jar包和动态库。

>> `make python`、`make perl`：分别为Python、Perl等生成类文件和动态库。

>> `make clean java.clean python.clean`：删除上述编译出的临时文件。

> 如果是在Windows上编译，推荐使用 MSYS（Unix模拟环境工具）、TDM-GCC（一种MinGW发行版）。
