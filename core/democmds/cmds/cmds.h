//! \file cmds.h
//! \brief 定义示例应用内部入口类 DemoCmds_

#if !defined(DEMOCMDS_IMPL_H_) && !defined(SWIG)
#define DEMOCMDS_IMPL_H_

class MgView;

//! 示例应用内部入口类
/*! \ingroup CORE_COMMAND
 */
struct DemoCmdsImpl {
    //! 登记绘图命令和图形
    static int registerCmds(MgView* view);

    //! 得到当前图形的各种度量尺寸
    static int getDimensions(MgView* view, float* vars, char* types, int n);
};

#endif // DEMOCMDS_IMPL_H_
