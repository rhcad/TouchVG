//! \file ViewHelper.java
//! \brief Android绘图视图辅助类
// Copyright (c) 2012-2013, https://github.com/rhcad/touchvg

package touchvg.view;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import touchvg.core.GiContextBits;
import touchvg.core.MgView;
import android.content.Context;
import android.graphics.Bitmap;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;

//! Android绘图视图辅助类
/*! \ingroup GROUP_ANDROID
 */
public class ViewHelper {
    private GraphView mView;                //!< 要操作的视图
    
    //! 指定视图的构造函数
    public ViewHelper(GraphView view) {
        mView = view;
    }
    
    //! 获取当前活动视图的默认构造函数
    public ViewHelper() {
        mView = GraphView.activeView();
    }
    
    //! 返回当前激活视图
    public static GraphView activeView() {
        return GraphView.activeView();
    }
    
    //! 得到要操作的视图
    public GraphView getView() {
        return mView;
    }
    
    //! 返回内核视图的句柄, MgView 指针
    public int cmdViewHandle() {
        return mView.coreView().viewAdapterHandle();
    }
    
    //! 返回内核命令视图
    public MgView cmdView() {
        return mView.coreView().viewAdapter();
    }
    
    //! 自动创建FrameLayout布局，在其中创建普通绘图视图，并记下此视图
    public FrameLayout createGraphView(Context context) {
        mView = new GraphView(context);
        final FrameLayout layout = new FrameLayout(context);
        layout.addView(mView, new LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        return layout;
    }
    
    /**
     * @brief 自动创建FrameLayout布局，在其中创建放大镜视图，并记下此视图
     * @param context 视图上下文对象
     * @param mainView 参照的主视图对象，如果为null则取本对象的视图或当前激活视图
     * @return 容纳新绘图视图的FrameLayout布局对象
     */
    public FrameLayout createMagnifierView(Context context, GraphView mainView) {
        mView = new GraphView(context, mainView != null ? mainView : mView);
        final FrameLayout layout = new FrameLayout(context);
        layout.addView(mView, new LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
        return layout;
    }
    
    /**
     * @brief 设置上下文按钮的图像ID数组
     * @param imageIDs 基本操作按钮的图像ID数组, 32x32
     * @param captionsID 基本操作按钮的文字数组ID，例如 R.array.vg_action_captions
     * @param extraImageIDs 行业特定操作按钮的图像ID数组, 32x32
     * @param handleImageIDs 控制点的图像ID数组
     * 使用示例:
     * @code
     * private static final int[] IMAGEIDS = { 0, R.drawable.vg_selall, 0, R.drawable.vg_draw,
     *     R.drawable.vg_back, R.drawable.vg_delete, R.drawable.vg_clone, R.drawable.vg_fixlen,
     *     R.drawable.vg_freelen, R.drawable.vg_lock, R.drawable.vg_unlock, R.drawable.vg_edit,
     *     R.drawable.vg_endedit, 0, 0, R.drawable.vg_addvertex, R.drawable.vg_delvertex,
     *     R.drawable.vg_group, R.drawable.vg_ungroup, R.drawable.vg_overturn };
     * private static final int[] HANDLEIDS = { R.drawable.vgdot1, R.drawable.vgdot2,
     *     R.drawable.vgdot3, R.drawable.vg_lock, 
     *     R.drawable.vg_unlock, R.drawable.vg_back, R.drawable.vg_endedit };
     * ViewHelper.setContextButtonImages(IMAGEIDS, R.array.vg_action_captions, null, HANDLEIDS);
     * @endcode
     */
    public static void setContextButtonImages(int[] imageIDs, int captionsID, 
            int[] extraImageIDs, int[] handleImageIDs) {
        GraphView.setContextButtonImages(imageIDs, captionsID, extraImageIDs, handleImageIDs);
    }
    
    //! 得到当前命令名称
    public String getCommand() {
        return mView.coreView().getCommand();
    }
    
    //! 启动指定名称的命令
    public boolean setCommand(String name) {
        return mView.coreView().setCommand(mView.viewAdapter(), name);
    }
    
    //! 返回线宽，正数表示单位为0.01毫米，零表示1像素宽，负数表示单位为像素
    public int getLineWidth() {
        return Math.round(mView.coreView().getContext(false).getLineWidth());
    }
    
    //! 设置线宽，正数表示单位为0.01毫米，零表示1像素宽，负数表示单位为像素
    public void setLineWidth(int w) {
        mView.coreView().getContext(true).setLineWidth(w, true);
        mView.coreView().setContext(GiContextBits.kContextLineWidth.swigValue());
    }
    
    //! 返回像素单位的线宽，总是为正数
    public int getStrokeWidth() {
        float w = mView.coreView().getContext(false).getLineWidth();
        return Math.round(mView.coreView().calcPenWidth(w));
    }
    
    //! 设置像素单位的线宽，总是为正数
    public void setStrokeWidth(int w) {
        mView.coreView().getContext(true).setLineWidth(-Math.abs(w), true);
        mView.coreView().setContext(GiContextBits.kContextLineWidth.swigValue());
    }
    
    //! 返回线型, 0-5:实线,虚线,点线,点划线,双点划线,空线
    public int getLineStyle() {
        float w = mView.coreView().getContext(false).getLineStyle();
        return Math.round(mView.coreView().calcPenWidth(w));
    }
    
    //! 设置线型, 0-5:实线,虚线,点线,点划线,双点划线,空线
    public void setLineStyle(int style) {
        mView.coreView().getContext(true).setLineStyle(style);
        mView.coreView().setContext(GiContextBits.kContextLineStyle.swigValue());
    }
    
    //! 返回线条颜色，忽略透明度分量，0 表示不画线条
    public int getLineColor() {
        return mView.coreView().getContext(false).getLineColor().getARGB();
    }
    
    //! 设置线条颜色，忽略透明度分量，0 表示不画线条
    public void setLineColor(int argb) {
        mView.coreView().getContext(true).setLineARGB(argb);
        mView.coreView().setContext(argb == 0 ? GiContextBits.kContextLineARGB.swigValue()
                                    : GiContextBits.kContextLineRGB.swigValue());
    }
    
    //! 返回线条透明度, 0-255
    public int getLineAlpha() {
        return mView.coreView().getContext(false).getLineColor().getA();
    }
    
    //! 设置线条透明度, 0-255
    public void setLineAlpha(int alpha) {
        mView.coreView().getContext(true).setLineAlpha(alpha);
        mView.coreView().setContext(GiContextBits.kContextLineAlpha.swigValue());
    }
    
    //! 返回填充颜色，忽略透明度分量，0 表示不填充
    public int getFillColor() {
        return mView.coreView().getContext(false).getFillColor().getARGB();
    }
    
    //! 设置填充颜色，忽略透明度分量，0 表示不填充
    public void setFillColor(int argb) {
        mView.coreView().getContext(true).setFillARGB(argb);
        mView.coreView().setContext(argb == 0 ? GiContextBits.kContextFillARGB.swigValue()
                                    : GiContextBits.kContextFillRGB.swigValue());
    }
    
    //! 返回填充透明度, 0-255
    public int getFillAlpha() {
        return mView.coreView().getContext(false).getFillColor().getA();
    }
    
    //! 设置填充透明度, 0-255
    public void setFillAlpha(int alpha) {
        mView.coreView().getContext(true).setFillAlpha(alpha);
        mView.coreView().setContext(GiContextBits.kContextFillAlpha.swigValue());
    }
    
    //! 绘图属性是否正在动态修改. 拖动时先设为true，然后改变绘图属性，完成后设为false.
    public void setContextEditing(boolean editing) {
        mView.coreView().setContextEditing(editing);
    }
    
    //! 添加测试图形
    public int addShapesForTest() {
        return mView.coreView().addShapesForTest();
    }
    
    //! 放缩显示全部内容
    public boolean zoomToExtent() {
        return mView.coreView().zoomToExtent();
    }
    
    //! 放缩显示指定范围到视图区域
    public boolean zoomToModel(float x, float y, float w, float h) {
        return mView.coreView().zoomToModel(x, y, w, h);
    }
    
    //! 得到静态图形的快照
    public Bitmap snapshot() {
        return mView.snapshot();
    }
    
    //! 保存静态图形的快照到PNG文件
    public boolean savePng(String filename) {
        boolean ret = false;
        synchronized(mView.snapshot()) {
            try {
                final FileOutputStream os = new FileOutputStream(filename);
                ret = mView.snapshot().compress(Bitmap.CompressFormat.PNG, 100, os);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return ret;
    }
    
    //! 返回图形总数
    public int getShapeCount() {
        return mView.coreView().getShapeCount();
    }
    
    //! 返回选中的图形个数
    public int getSelectedCount() {
        return mView.coreView().getSelectedShapeCount();
    }
    
    //! 返回选中的图形的类型, MgShapeType
    public int getSelectedType() {
        return mView.coreView().getSelectedShapeType();
    }
    
    //! 得到图形的JSON内容
    public String getContent() {
        final String str = mView.coreView().getContent();
        mView.coreView().freeContent();
        return str;
    }
    
    //! 从JSON内容中加载图形
    public boolean setContent(String content) {
        return mView.coreView().setContent(content);
    }
    
    //! 从JSON文件中加载图形
    public boolean loadFromFile(String vgfile) {
        return mView.coreView().loadFromFile(vgfile);
    }
    
    //! 保存图形到JSON文件
    public boolean saveToFile(String vgfile) {
        boolean ret;
        if (getShapeCount() == 0) {
            ret = new File(vgfile).delete();
        } else {
            ret = createFile(vgfile)
            && mView.coreView().saveToFile(vgfile);
        }
        return ret;
    }
    
    //! 创建指定的文件和上一级文件夹
    public static boolean createFile(String filename) {
        final File file = new File(filename);
        final File pf = file.getParentFile();
        if (!pf.exists()) {
            pf.mkdirs();
        }
        if (!file.exists()) {
            try {
                file.createNewFile();
            } catch (IOException e) {
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }
}
