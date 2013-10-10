关于 TouchVG
------------
> TouchVG 是一个主要由C++开发的轻量级2D矢量绘图框架，可在 iOS、Android、Windows 等多个平台构建矢量绘图应用。
> 
> 作者：张云贵 <rhcad@hotmail.com> 开源协议：LGPL 2.1，见 LICENSE 文件描述。

TouchVG 的功能
--------------
> 支持多点触摸或鼠标的交互绘图方式，可以无级放缩或平移显示矢量图形、图像。

> 除了基本的15余种矢量图形类型外，可使用C++或Java等来扩充更多图形类型。

> 除了基本的20余种选择和绘图命令外，还可使用C++、Java等扩充更多的交互命令。

> 可以交互式修改图形的线宽、颜色等属性，可以进行放缩变形、克隆等图形操作。

> 支持JSON等格式的图形序列化操作，允许扩充XML、数据库等其他序列化适配器。

> 支持多模块的扩展机制，可构建数字教育、动态几何、图文笔记等多种行业绘图模块。

> 更多说明见本项目的维基文档。


意见和建议
----------
> 在您使用 TouchVG 的过程中有任何意见和建议，请到此项目 [提交Issue](http://git.oschina.net/rhcad/touchvg/issues)。

Android编译说明
--------------

> 在 eclipse 中导入此项目下的工程，运行 VGTest 工程可看演示效果。

>> 可能需要根据实际安装情况修改工程的SDK版本号(target)。
    
> 可以将 democmds 改为你的应用库工程，该工程包含 democmds.jar 和 libdemocmds.so(本地库)。

> 如需重新生成 libdemocmds.so，则进入此项目的 android 目录，运行 `sh build.sh` 。

>> 使用NDK-r8c以后的版本如果出现 `build/gmsl/__gmsl:512: *** non-numeric second argument to wordlist function` 错误，则打开 NDK 安装目录下的 `build/gmsl/__gmsl` 文件，将512行改为：
    `int_encode = $(__gmsl_tr1)$(wordlist 1,$(words $1),$(__gmsl_input_int))`

>> 在Windows上推荐使用 MSYS（Unix模拟环境工具）、TDM-GCC（一种MinGW发行版）。

> 如需重新生成内核JNI类（democmds.jar中的core包），则先删除 `democmds_java_wrap.cpp`，再运行`sh build.sh`（需要安装[SWIG](http://sourceforge.net/projects/swig/files/)）。

iOS编译说明
----------

> 在 Xcode 中打开 ios/TestVG.xcworkspace 文件，运行 TestView 演示程序。

> 编译条件：Mac OS X 10.5 以上，Xcode 3.2 以上。

>> 在真机上调试时需要修改主程序的Bundle Identifier，例如“com.yourcompany.TestCanvas”，并选择自己的开发证书（Code Signing）。

> 可以将 DemoCmds 改为你的应用库工程。

> 如需重新生成 ios/output下的 libDemoCmds.a，则进入此项目的 ios 目录，运行`sh build.sh`（需要配置Xcode命令行环境）。

WPF编译说明
----------

> 在 Visual Studio 2010 中打开 `wpf/Test_cs10.sln`，运行 WpfDemo 工程。VS2008则打开 `wpf/Test_cs9.sln` 。

> 如需重新生成内核类，需要安装[SwigWin](http://sourceforge.net/projects/swig/files/swigwin/)。
  
其他编译说明
--------
> 还可在Linux、MinGW、Mac OS X下编译静态库，或为 Python、Perl、Java 等应用编译动态库。

> 进入`core`目录，该目录下有 Makefile 文件，运行下列 make 指令：

>> `make all install`：编译C++静态库。

>> `make java`：为Java程序生成Jar包和动态库。

>> `make python`、`make perl`：分别为Python、Perl等生成类文件和动态库。

>> `make clean java.clean python.clean`：删除上述编译出的临时文件。

> 在Windows上推荐使用 MSYS（Unix模拟环境工具）、TDM-GCC（一种MinGW发行版）。
