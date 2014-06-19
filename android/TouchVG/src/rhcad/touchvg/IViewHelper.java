//! \file IViewHelper.java
//! \brief Android绘图视图辅助类
// Copyright (c) 2012-2014, https://github.com/rhcad/touchvg

package rhcad.touchvg;

import java.util.ArrayList;

import rhcad.touchvg.core.GiCoreView;
import rhcad.touchvg.core.MgView;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PointF;
import android.graphics.Rect;
import android.graphics.RectF;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;

//! 绘图视图辅助API接口
public interface IViewHelper {

    //! [状态无关]返回绘图包的版本号，1.1.jarver.sover
    public String getVersion();

    //! [状态无关]返回当前激活视图
    public IGraphView activeView();

    //! 得到要操作的视图
    public IGraphView getGraphView();

    //! 设置要操作的视图
    public void setGraphView(IGraphView view);

    //! 得到要操作的视图
    public View getView();

    //! 得到上一级视图
    public ViewGroup getParent();

    //! 返回视图上下文
    public Context getContext();

    //! 返回内核视图分发器对象
    public GiCoreView coreView();

    //! 返回内核视图的句柄, MgView 指针
    public int cmdViewHandle();

    //! 返回内核命令视图
    public MgView cmdView();

    //! 在指定的布局中创建SurfaceView绘图视图，并记下此视图
    public ViewGroup createSurfaceView(Context context, ViewGroup layout);

    //! 在指定的布局中创建SurfaceView绘图视图，在onCreate(Bundle)或onRestoreInstanceState(Bundle)中调用
    public ViewGroup createSurfaceView(Context context, ViewGroup layout, Bundle savedState);

    //! 在指定的布局中创建FrameLayout及其下的SurfaceView绘图视图，自动创建结束绘图用的ImageView(没有的话)
    public ViewGroup createSurfaceAndImageView(Context context, ViewGroup layout, Bundle savedState);

    //! 返回SurfaceView绘图视图对应的结束绘图用的ImageView
    public ImageView getImageViewForSurface();

    //! 在指定的布局（建议为FrameLayout）中创建普通绘图视图，并记下此视图
    public ViewGroup createGraphView(Context context, ViewGroup layout);

    //! 在指定的布局（建议为FrameLayout）中创建普通绘图视图，在onCreate(Bundle)或onRestoreInstanceState(Bundle)中调用
    public ViewGroup createGraphView(Context context, ViewGroup layout, Bundle savedState);

    //! 在指定的布局中创建放大镜视图，并记下此视图
    public ViewGroup createMagnifierView(Context context, ViewGroup layout, IGraphView mainView);

    //! [状态无关]设置额外的上下文操作按钮的图像ID数组，其动作序号从40起
    public void setExtraContextImages(Context context, int[] ids);

    //! 得到当前命令名称
    public String getCommand();

    //! 当前是否为指定名称的命令
    public boolean isCommand(String name);

    //! 启动指定名称的命令(可用的命令名在LogCat中会打印出来，例如“registerCommand 11:lines”中的“lines”)
    public boolean setCommand(String name);

    //! 启动指定名称的命令，并指定JSON串的命令初始化参数
    public boolean setCommand(String name, String params);

    //! 切换到下一命令
    public boolean switchCommand();

    //! 返回线宽，正数表示单位为0.01毫米，零表示1像素宽，负数表示单位为像素
    public int getLineWidth();

    //! 设置线宽，正数表示单位为0.01毫米，零表示1像素宽，负数表示单位为像素
    public void setLineWidth(int w);

    //! 返回像素单位的线宽，总是为正数
    public int getStrokeWidth();

    //! 设置像素单位的线宽，总是为正数
    public void setStrokeWidth(int w);

    public final static int SOLID_LINE  = 0;
    public final static int DASH_LINE   = 1;
    public final static int DOT_LINE    = 2;
    public final static int DASH_DOT    = 3;
    public final static int DASH_DOT_DOT = 4;
    public final static int NULL_LINE   = 5;
    public static final int MAX_LINESTYLE = 5;

    //! 返回线型(SOLID_LINE,DASH_LINE..NULL_LINE)
    public int getLineStyle();

    //! 设置线型(SOLID_LINE,DASH_LINE..NULL_LINE)
    public void setLineStyle(int style);

    //! 返回线条颜色，忽略透明度分量，0 表示不画线条
    public int getLineColor();

    //! 设置线条颜色，忽略透明度分量，0 表示不画线条
    public void setLineColor(int argb);

    //! 返回线条透明度, 0-255
    public int getLineAlpha();

    //! 设置线条透明度, 0-255
    public void setLineAlpha(int alpha);

    //! 返回填充颜色，忽略透明度分量，0 表示不填充
    public int getFillColor();

    //! 设置填充颜色，忽略透明度分量，0 表示不填充
    public void setFillColor(int argb);

    //! 返回填充透明度, 0-255
    public int getFillAlpha();

    //! 设置填充透明度, 0-255
    public void setFillAlpha(int alpha);

    //! 绘图属性是否正在动态修改. 拖动时先设为true，然后改变绘图属性，完成后设为false.
    public void setContextEditing(boolean editing);

    //! 添加测试图形
    public int addShapesForTest();

    //! 释放临时缓存
    public void clearCachedData();

    //! 放缩显示全部内容
    public boolean zoomToExtent();

    //! 放缩显示指定范围到视图区域
    public boolean zoomToModel(float x, float y, float w, float h);

    //! 视图坐标转为模型坐标
    public PointF displayToModel(float x, float y);

    //! 视图坐标转为模型坐标
    public RectF displayToModel(RectF rect);

    //! 开始Undo录制
    public boolean startUndoRecord(String path);

    //! 停止Undo录制
    public void stopUndoRecord();

    //! 能否撤销
    public boolean canUndo();

    //! 能否重做
    public boolean canRedo();

    //! 撤销
    public void undo();

    //! 重做
    public void redo();

    //! 是否正在录屏
    public boolean isRecording();

    //! 开始录屏
    public boolean startRecord(String path);

    //! 停止录屏
    public void stopRecord();

    //! 是否已暂停
    public boolean isPaused();

    //! 是否正在播放
    public boolean isPlaying();

    //! 返回已录制的相对毫秒数
    public int getRecordTicks();

    //! 返回是否允许触摸交互
    public boolean getGestureEnabled();

    //! 设置是否允许触摸交互
    public void setGestureEnabled(boolean enabled);

    //! 是否允许放缩显示
    public void setZoomEnabled(boolean enabled);

    //! 设置背景色，普通视图默认透明，SurfaceView默认白色背景
    public void setBackgroundColor(int color);

    //! 设置背景图，SurfaceView不透明时使用
    public void setBackgroundDrawable(Drawable background);

    //! 得到静态图形的快照，支持多线程
    public Bitmap snapshot(boolean transparent);

    //! 得到当前显示的静态图形快照，自动去掉周围空白，支持多线程
    public Bitmap extentSnapshot(int spaceAround, boolean transparent);

    //! 保存当前显示的静态图形快照(去掉周围空白)到PNG文件，自动添加后缀名.png，支持多线程
    public boolean exportExtentAsPNG(String filename, int spaceAround);

    //! 保存静态图形的快照到PNG文件，自动添加后缀名.png，支持多线程
    public boolean exportPNG(String filename, boolean transparent);

    //! 保存静态图形的透明背景快照到PNG文件，自动添加后缀名.png，支持多线程
    public boolean exportPNG(String filename);

    //! 导出静态图形到SVG文件，自动添加后缀名.svg
    public boolean exportSVG(String filename);

    //! 返回图形总数
    public int getShapeCount();

    //! 返回选中的图形个数
    public int getSelectedCount();

    //! 返回选中的图形的类型, MgShapeType
    public int getSelectedType();

    //! 返回当前选中的图形的ID，选中多个时只取第一个
    public int getSelectedShapeID();

    //! 返回图形改变次数，可用于检查是否需要保存
    public int getChangeCount();

    //! 返回图形改变次数，可用于检查是否需要保存
    public int getDrawCount();

    //! 返回图形显示范围
    public Rect getDisplayExtent();

    //! 返回图形显示范围，支持多线程
    public Rect getDisplayExtent(int doc, int gs);

    //! 返回选择包络框
    public Rect getBoundingBox();

    //! 得到指定ID的图形的包络框显示坐标
    public Rect getShapeBox(int sid);

    //! 得到图形的JSON内容，支持多线程
    public String getContent();

    //! 从JSON内容中加载图形
    public boolean setContent(String content);

    //! 从JSON文件中加载图形，自动添加后缀名.vg
    public boolean loadFromFile(String vgfile);

    //! 从JSON文件中以只读方式加载图形，自动添加后缀名.vg
    public boolean loadFromFile(String vgfile, boolean readOnly);

    //! 保存图形到JSON文件，自动添加后缀名.vg，支持多线程
    public boolean saveToFile(String vgfile);

    //! 清除所有图形
    public void clearShapes();

    //! 在默认位置插入一个程序资源中的SVG图像(id=R.raw.name)
    public int insertSVGFromResource(String name);

    //! 在默认位置插入一个程序资源中的SVG图像(id=R.raw.name)
    public int insertSVGFromResource(int id);

    //! 插入一个程序资源中的SVG图像(id=R.raw.name)，并指定图像的中心位置
    public int insertSVGFromResource(String name, int xc, int yc);

    //! 插入一个程序资源中的SVG图像(id=R.raw.name)，并指定图像的中心位置
    public int insertSVGFromResource(int id, int xc, int yc);

    //! 在默认位置插入一个程序资源中的位图图像(id=R.drawable.name)
    public int insertBitmapFromResource(String name);

    //! 在默认位置插入一个程序资源中的位图图像(id=R.drawable.name)
    public int insertBitmapFromResource(int id);

    //! 插入一个程序资源中的位图图像(id=R.drawable.name)，并指定图像的中心位置
    public int insertBitmapFromResource(String name, int xc, int yc);

    //! 插入一个程序资源中的位图图像(id=R.drawable.name)，并指定图像的中心位置
    public int insertBitmapFromResource(int id, int xc, int yc);

    //! 在默认位置插入一个PNG、JPEG或SVG等文件的图像
    public int insertImageFromFile(String filename);

    //! 插入一个图像文件，并指定图像的中心位置和自定义数据
    public int insertImageFromFile(String filename, int xc, int yc, int tag);

    //! 返回是否有容纳图像的图形对象
    public boolean hasImageShape();

    //! 查找指定名称的图像对应的图形对象ID
    public int findShapeByImageID(String name);

    //! 查找指定Tag的图形对象ID
    public int findShapeByTag(int tag);

    //! 遍历有容纳图像的图形对象
    public ArrayList<Bundle> getImageShapes();

    //! 返回图像文件的默认路径
    public String getImagePath();

    //! 设置图像文件的默认路径(可以没有末尾的分隔符)，自动加载时用
    public void setImagePath(String path);

    //! 关闭视图
    public void close();

    //! 所属的Activity销毁前或关闭视图时调用
    public void onDestroy();

    //! 所属的Activity暂停时调用
    public boolean onPause();

    //! 所属的Activity恢复时调用
    public boolean onResume();

    //! 所属的Activity保存状态时调用
    public void onSaveInstanceState(Bundle outState, String path);

    //! 所属的Activity恢复状态时调用
    public void onRestoreInstanceState(Bundle savedState);
}
