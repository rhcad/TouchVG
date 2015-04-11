# TouchVG for WPF

This is a lightweight 2D vector drawing framework using [vgcore](https://github.com/rhcad/vgcore) for Windows (WPF).

Features described in [Online document](http://touchvg.github.io). Please visit [vgwpf-demo](https://github.com/rhcad/vgwpf-demo) to see more examples.

![arch](http://touchvg.github.io/images/arch.svg)

## License

This is an open source [BSD licensed](../LICENSE) licensed project. It uses the following open source projects:

- [vgcore](https://github.com/rhcad/vgcore) (LGPL): Cross-platform vector drawing libraries using C++.
- [x3py](https://github.com/rhcad/x3py) (Apache): Compile script files.
- [SWIG](https://github.com/swig/swig) (GPL): Use the tool to generate the glue code for Java and C#.

## How to Contribute

Contributors and sponsors are welcome. You may translate, commit issues or pull requests on this Github site.
To contribute, please follow the branching model outlined here: [A successful Git branching model](http://nvie.com/posts/a-successful-git-branching-model/).

## How to Compile

- Open `Test_cs10.sln` in Visual Studio 2010 (Need VC++ and C#). Or open `Test_cs9.sln` in VS2008.

  - Need to install the lastest version of [SWIG](http://sourceforge.net/projects/swig/files/), and add the location to PATH.
  
- Type `./build.sh` can regenerate `touchvglib/core/*.cs`.

# Add more shapes and commands

- You can use [newproj.py](https://github.com/rhcad/DemoCmds/blob/master/newproj.py) to create library project containing your own shapes and commands. So the TouchVG and TouchVGCore libraries does not require changes.

  - Checkout and enter [DemoCmds](https://github.com/rhcad/DemoCmds) directory, then type `python newproj.py YourCmds`:

     ```shell
     git clone https://github.com/rhcad/DemoCmds.git
     cd DemoCmds
     python newproj.py MyCmds
     ```
 