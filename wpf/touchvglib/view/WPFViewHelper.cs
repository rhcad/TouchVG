//! \file WPFViewHelper.cs
//! \brief 定义WPF绘图视图辅助类
// Copyright (c) 2013, https://github.com/rhcad/touchvg

using System;
using System.Windows;
using System.Windows.Media;
using System.Text;
using touchvg.core;
using System.Windows.Media.Imaging;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;

namespace touchvg.view
{
    //! WPF绘图视图辅助类
    /*! \ingroup GROUP_WPF
     */
    public class WPFViewHelper : IDisposable
    {
        private static int LIB_RELEASE = 8; // TODO: 在本工程接口变化后增加此数
        public delegate void GiAction();
        private WPFGraphView View;
        private GiCoreView CoreView { get { return View.CoreView; } }
        public GiView ViewAdapter { get { return View.ViewAdapter; } }
        public static WPFGraphView ActiveView { get { return WPFGraphView.ActiveView; } }

        public WPFViewHelper()
        {
            this.View = WPFGraphView.ActiveView;
        }

        public WPFViewHelper(WPFGraphView view)
        {
            this.View = view != null ? view : WPFGraphView.ActiveView;
        }

        public void Dispose()
        {
            if (this.View != null)
            {
                this.View.Dispose();
                this.View = null;
            }
        }

        //! 返回本库的版本号, 1.1.cslibver.corelibver
        public string Version
        {
            get
            {
                return string.Format("1.1.%d.%d", LIB_RELEASE, GiCoreView.getVersion());
            }
        }

        //! 返回内核视图的句柄, MgView 指针
        public int CmdViewHandle()
        {
            return CoreView.viewAdapterHandle();
        }

        //! 返回内核命令视图
        public MgView CmdView()
        {
            return MgView.fromHandle(CmdViewHandle());
        }

        //! 返回图形工厂对象
        public MgShapeFactory ShapeFactory()
        {
            return CmdView().getShapeFactory();
        }

        public class StringCallback : MgStringCallback
        {
            private string text;

            public override void onGetString(string text)
            {
                this.text = text;
            }

            public override string ToString()
            {
                return text;
            }
        }

        //! 当前命令名称
        public string Command
        {
            get { StringCallback c = new StringCallback(); CoreView.getCommand(c); return c.ToString(); }
            set { CoreView.setCommand(value); }
        }

        //! 指定名称和JSON串参数，启动命令
        public bool SetCommand(string name, string param)
        {
            return CoreView.setCommand(name, param);
        }

        //! 切换到下一命令
        public bool SwitchCommand()
        {
            return CoreView.switchCommand();
        }

        //! 当前是否为指定名称的命令
        public bool IsCommand(string name)
        {
            return CoreView.isCommand(name);
        }

        //! 线宽，正数表示0.1毫米单位，零表示1像素宽，负数表示像素单位
        public int LineWidth
        {
            get
            {
                float w = CoreView.getContext(false).getLineWidth();
                return (int)(w > 1e-5f ? w / 10.0f : w);
            }
            set
            {
                CoreView.getContext(true).setLineWidth(
                    value > 0 ? (float)value * 10.0f : value, true);
                CoreView.setContext(GiContext.kLineWidth);
            }
        }

        //! 像素单位的线宽，总是为正数
        public int StrokeWidth
        {
            get
            {
                GiContext ctx = CoreView.getContext(false);
                return (int)CoreView.calcPenWidth(ViewAdapter, ctx.getLineWidth());
            }
            set
            {
                CoreView.getContext(true).setLineWidth(
                    -Math.Abs((float)value), true);
                CoreView.setContext(GiContext.kLineWidth);
            }
        }

        //! 线型, 0-5:实线,虚线,点线,点划线,双点划线,空线
        public int LineStyle
        {
            get { return CoreView.getContext(false).getLineStyle(); }
            set
            {
                CoreView.getContext(true).setLineStyle(value);
                CoreView.setContext(GiContext.kLineStyle);
            }
        }

        //! 起端箭头, 0-6:无箭头,实心尖头,简单线条尖头,T型,斜线,实圆头,空圆头
        public int StartArrayHead
        {
            get { return CoreView.getContext(false).getStartArrayHead(); }
            set
            {
                CoreView.getContext(true).setStartArrayHead(value);
                CoreView.setContext(GiContext.kLineArrayHead);
            }
        }

        //! 起端箭头, 0-6:无箭头,实心尖头,简单线条尖头,T型,斜线,实圆头,空圆头
        public int EndArrayHead
        {
            get { return CoreView.getContext(false).getEndArrayHead(); }
            set
            {
                CoreView.getContext(true).setEndArrayHead(value);
                CoreView.setContext(GiContext.kLineArrayHead);
            }
        }

        //! 线条颜色，忽略透明度，Colors.Transparent不画线条
        public Color LineColor
        {
            get
            {
                GiColor c = CoreView.getContext(false).getLineColor();
                return c.a > 0 ? Color.FromRgb(c.r, c.g, c.b) : Colors.Transparent;
            }
            set
            {
                CoreView.getContext(true).setLineColor(
                    value.R, value.G, value.B, value.A);
                if (value.A > 0)
                    CoreView.setContext(GiContext.kLineRGB);
                else
                    CoreView.setContext(GiContext.kLineARGB);
            }
        }

        //! 线条透明度, 0-255
        public int LineAlpha
        {
            get { return CoreView.getContext(false).getLineAlpha(); }
            set
            {
                CoreView.getContext(true).setLineAlpha(value);
                CoreView.setContext(GiContext.kLineAlpha);
            }
        }

        //! 填充颜色，忽略透明度，Colors.Transparent不填充
        public Color FillColor
        {
            get
            {
                GiColor c = CoreView.getContext(false).getFillColor();
                return c.a > 0 ? Color.FromRgb(c.r, c.g, c.b) : Colors.Transparent;
            }
            set
            {
                CoreView.getContext(true).setFillColor(
                    value.R, value.G, value.B, value.A);
                if (value.A > 0)
                    CoreView.setContext(GiContext.kFillRGB);
                else
                    CoreView.setContext(GiContext.kFillARGB);
            }
        }

        //! 填充透明度, 0-255
        public int FillAlpha
        {
            get { return CoreView.getContext(false).getFillAlpha(); }
            set
            {
                CoreView.getContext(true).setFillAlpha(value);
                CoreView.setContext(GiContext.kFillAlpha);
            }
        }

        //! 绘图属性是否正在动态修改. 拖动时先设为true，然后改变绘图属性，完成后设为false.
        public bool ContextEditing
        {
            set { CoreView.setContextEditing(value); }
        }

        //! 绘图命令选项
        public Dictionary<string, IConvertible> Options
        {
            get
            {
                OptionCallback c = new OptionCallback();
                GetOptions(c);
                return c.Options;
            }
            set
            {
                if (value != null && value.Count > 0)
                {
                    foreach (var item in value)
                    {
                        SetOption(item.Key, item.Value);
                    }
                }
            }
        }

        //! 以自定义类型(例如Json类)遍历选项
        public void GetOptions(MgOptionCallback c)
        {
            CoreView.traverseOptions(c);
            c.onGetOptionBool("contextActionEnabled", View.ContextActionEnabled);
            c.onGetOptionBool("zoomEnabled", ZoomEnabled);
        }

        //! 重置所有选项
        public void ClearOptions()
        {
            CoreView.setOptionBool(null, false);
        }

        //! 设置绘图命令选项(bool/int/float类型的值)
        public bool SetOption(string key, IConvertible value)
        {
            if (key == null || value == null)
            {
                return false;
            }

            if (value.GetTypeCode() == TypeCode.String)
            {
                string str = Convert.ToString(value).ToLower();
                int intValue;
                double doubleValue;

                if (str.CompareTo("true") == 0)
                    return SetOption(key, true);
                if (str.CompareTo("false") == 0)
                    return SetOption(key, false);
                if (int.TryParse(str, out intValue))
                    return SetOption(key, intValue);
                if (double.TryParse(str, out doubleValue))
                    return SetOption(key, doubleValue);

                CoreView.setOptionString(key, Convert.ToString(value));
                return true;
            }
            
            if (key == "contextActionEnabled") {
                View.ContextActionEnabled = Convert.ToBoolean(value);
            }
            else if (key == "zoomEnabled") {
                ZoomEnabled = Convert.ToBoolean(value);
            }
            else switch (value.GetTypeCode()) {
                case TypeCode.Boolean:
                    CoreView.setOptionBool(key, Convert.ToBoolean(value));
                    break;

                case TypeCode.Int32:
                case TypeCode.UInt32:
                    CoreView.setOptionInt(key, Convert.ToInt32(value));
                    break;

                case TypeCode.Single:
                case TypeCode.Double:
                    CoreView.setOptionFloat(key, Convert.ToSingle(value));
                    break;

                default:
                    Debug.Assert(false, key);
                    return false;
            }

            return true;
        }

        private class OptionCallback : MgOptionCallback
        {
            public Dictionary<string, IConvertible> Options = new Dictionary<string, IConvertible>();

            public override void onGetOptionBool(string name, bool value)
            {
                Options.Remove(name);
                Options.Add(name, value);
            }

            public override void onGetOptionInt(string name, int value)
            {
                Options.Remove(name);
                Options.Add(name, value);
            }

            public override void onGetOptionFloat(string name, float value)
            {
                Options.Remove(name);
                Options.Add(name, value);
            }

            public override void onGetOptionString(string name, string text)
            {
                Options.Remove(name);
                Options.Add(name, text);
            }
        }

        //! 图形总数
        public int ShapeCount
        {
            get { return CoreView.getShapeCount(); }
        }

        //! 未锁定的可见图形的个数
        public int UnlockedShapeCount
        {
            get { return CoreView.getUnlockedShapeCount(); }
        }

        //! 可见图形的个数
        public int VisibleShapeCount
        {
            get { return CoreView.getVisibleShapeCount(); }
        }

        //! 选中的图形个数
        public int SelectedCount
        {
            get { return CoreView.getSelectedShapeCount(); }
        }

        //! 当前选中的图形的ID，选中多个时只取第一个
        public int SelectedShapeID
        {
            get { return CoreView.getSelectedShapeID(); }
            set {
                Ints ids = new Ints(value, 0);
                CoreView.setSelectedShapeIDs(ids);
            }
        }

        //! 当前选中的多个图形的ID
        public int[] SelectedID
        {
            get
            {
                Ints ids = new Ints();
                CoreView.getSelectedShapeIDs(ids);
                int[] ret = new int[ids.count()];
                for (int i = 0; i < ret.Length; i++)
                    ret[i] = ids.get(i);
                return ret;
            }
            set
            {
                int n = value != null ? value.Length : 0;
                Ints ids = new Ints(n);
                while (--n >= 0)
                    ids.set(n, value[n]);
                CoreView.setSelectedShapeIDs(ids);
            }
        }

        //! 当前线性图形中当前控制点序号
        public int SelectedHandle
        {
            get { return CoreView.getSelectedHandle(); }
        }

        //! 选中的图形的类型, MgShapeType
        public int SelectedType
        {
            get { return CoreView.getSelectedShapeType(); }
        }

        //! 图形改变次数，可用于检查是否需要保存
        public int ChangeCount
        {
            get { return CoreView.getChangeCount(); }
        }

        //! 显示次数
        public int DrawCount
        {
            get { return CoreView.getDrawCount(); }
        }

        //! 当前视图区域的模型坐标范围，模型坐标
        public Rect ViewBox
        {
            get
            {
                Floats box = new Floats(4);
                return ToBox(box, CoreView.getViewModelBox(box));
            }
        }

        //! 文档的模型坐标范围
        public Rect ModelBox
        {
            get
            {
                Floats box = new Floats(4);
                return ToBox(box, CoreView.getModelBox(box));
            }
        }

        //! 图形显示范围，像素坐标
        public Rect DisplayExtent
        {
            get
            {
                Floats box = new Floats(4);
                return ToBox(box, CoreView.getDisplayExtent(box));
            }
        }

        //! 选择包络框，像素坐标
        public Rect BoundingBox
        {
            get
            {
                Floats box = new Floats(4);
                return ToBox(box, CoreView.getBoundingBox(box));
            }
        }

        //! 得到指定ID的图形的包络框显示坐标
        public Rect GetShapeBox(int sid)
        {
            Floats box = new Floats(4);
            return ToBox(box, CoreView.getBoundingBox(box, sid));
        }

        //! 得到指定ID的图形的模型坐标范围
        public Rect getModelBox(int sid)
        {
            Floats box = new Floats(4);
            return ToBox(box, CoreView.getModelBox(box, sid));
        }

        private Rect ToBox(Floats box, bool ret)
        {
            return !ret ? new Rect() : new Rect(box.get(0), box.get(1),
                    box.get(2) - box.get(0), box.get(3) - box.get(1));
        }

        //! 所有图形的JSON内容
        public string Content
        {
            get
            {
                StringCallback c = new StringCallback();
                CoreView.getContent(c);
                return c.ToString();
            }
            set { CoreView.setContent(value); }
        }

        //! 查找指定Tag的图形对象ID
        public int FindShapeByTag(int tag)
        {
            int doc = CoreView.acquireFrontDoc();
            int sid = CoreView.findShapeByTag(doc, tag);
            GiCoreView.releaseDoc(doc);
            return sid;
        }

        //! 放缩显示全部内容到视图区域
        public bool ZoomToExtent()
        {
            return CoreView.zoomToExtent();
        }

        //! 放缩显示全部内容到视图区域
        public bool ZoomToExtent(float margin)
        {
            return CoreView.zoomToExtent(margin);
        }

        //! 放缩显示指定范围到视图区域
        public bool ZoomToModel(Rect rect)
        {
            return CoreView.zoomToModel((float)rect.X, (float)rect.Y,
                (float)rect.Width, (float)rect.Height);
        }

        //! 放缩显示指定范围到视图区域
        public bool ZoomToModel(float x, float y, float w, float h)
        {
            return CoreView.zoomToModel(x, y, w, h);
        }

        //! 放缩显示指定范围到视图区域
        public bool ZoomToModel(float x, float y, float w, float h, float margin)
        {
            return CoreView.zoomToModel(x, y, w, h, margin);
        }

        //! 图形向右上平移显示，像素单位
        public bool ZoomPan(float dxPixel, float dyPixel)
        {
            return CoreView.zoomPan(dxPixel, dyPixel);
        }

        //! 是否允许放缩显示
        public bool ZoomEnabled
        {
            get { return CoreView.isZoomEnabled(ViewAdapter); }
            set { CoreView.setZoomEnabled(ViewAdapter, value); }
        }

        //! 视图坐标转为模型坐标
        public bool DisplayToModel(Point pt)
        {
            Floats p = new Floats((float)pt.X, (float)pt.Y);
            bool ret = CoreView.displayToModel(p);

            pt.X = p.get(0);
            pt.Y = p.get(1);
            return ret;
        }

        //! 视图坐标转为模型坐标
        public bool DisplayToModel(Rect rect)
        {
            Floats p = new Floats((float)rect.Left, (float)rect.Top,
                (float)rect.Right, (float)rect.Bottom);
            bool ret = CoreView.displayToModel(p);

            rect.X = p.get(0);
            rect.Y = p.get(1);
            rect.Width = p.get(2) - rect.X;
            rect.Height = p.get(3) - rect.Y;

            return ret;
        }

        //! 显示比例
        public float ViewScale
        {
            get {
                return CmdView().xform().getViewScale();
            }
            set {
                if (CmdView().xform().zoomScale(value))
                    CmdView().regenAll(false);
            }
        }

        //! 添加测试图形
        public int AddShapesForTest()
        {
            return CoreView.addShapesForTest();
        }

        //! 从JSON文件中加载图形
        public bool Load(string vgfile)
        {
            return CoreView.loadFromFile(vgfile);
        }

        //! 从JSON文件中只读加载图形
        public bool Load(string vgfile, bool readOnly)
        {
            return CoreView.loadFromFile(vgfile, readOnly);
        }

        //! 保存图形到JSON文件
        public bool Save(string vgfile)
        {
            try
            {
                var dir = new DirectoryInfo(vgfile).Parent;
                if (!dir.Exists)
                    dir.Create();
            }
            catch (IOException)
            {
                return false;
            }
            return CoreView.saveToFile(vgfile);
        }

        //! 清除所有图形，含锁定的图形
        public void Clear()
        {
            CoreView.clear();
        }

        //! 清除当前视图区域内的未锁定的图形
        public void EraseView()
        {
            CoreView.setCommand("erasewnd");
        }

        //! 得到静态图形的快照
        public BitmapSource Snapshot()
        {
            int w = (int)View.MainCanvas.ActualWidth;
            int h = (int)View.MainCanvas.ActualHeight;

            RenderTargetBitmap bmp = new RenderTargetBitmap(
                w, h, 96, 96, PixelFormats.Pbgra32);
            bmp.Render(View.MainCanvas);

            return bmp;
        }

        //! 导出静态图形到SVG文件
        public bool ExportSVG(string filename)
        {
            return CoreView.exportSVG(ViewAdapter, filename) > 0;
        }

        //! 保存静态图形的快照到PNG、JPG或GIF文件，其他后缀名则自动改为.png
        public bool ExportPNG(string filename)
        {
            return SaveImage(Snapshot(), filename);
        }

        private bool SaveImage(BitmapSource image, string filename)
        {
            string ext = Path.GetExtension(filename).ToLower();
            BitmapEncoder encoder;

            if (ext == ".gif")
            {
                encoder = new GifBitmapEncoder();
            }
            else if (ext == ".png")
            {
                encoder = new PngBitmapEncoder();
            }
            else if (ext == ".jpg")
            {
                encoder = new JpegBitmapEncoder();
            }
            else
            {
                filename = Path.ChangeExtension(filename, ".png");
                encoder = new PngBitmapEncoder();
            }

            try
            {
                encoder.Frames.Add(BitmapFrame.Create(image));

                using (Stream stm = File.Create(filename))
                {
                    encoder.Save(stm);
                }
            }
            catch (SystemException)
            {
                return false;
            }

            return true;
        }

        //! 开始Undo录制
        public bool StartUndoRecord(string path)
        {
            if (CoreView.isUndoRecording())
                return false;

            try
            {
                var dir = new DirectoryInfo(path);
                if (dir.Exists)
                    dir.Delete(true);
                dir.Create();
            }
            catch (IOException)
            {
                return false;
            }

            return CoreView.startRecord(path, CoreView.acquireFrontDoc(),
                true, WPFGraphView.getTick());
        }

        //! 停止Undo录制
        public void StopUndoRecord()
        {
            CoreView.stopRecord(true);
        }

        //! 能否撤销
        public bool CanUndo()
        {
            return CoreView.canUndo();
        }

        //! 能否重做
        public bool CanRedo()
        {
            return CoreView.canRedo();
        }

        //! 撤销
        public bool Undo()
        {
            return CoreView.undo(ViewAdapter);
        }

        //! 重做
        public bool Redo()
        {
            return CoreView.redo(ViewAdapter);
        }

        //! 在块中批量操作，最后才重新生成.
        //! 例: hlp.CombineRegen(delegate(){ .... });
        public void CombineRegen(GiAction action)
        {
            MgRegenLocker locker = new MgRegenLocker(this.CmdView());
            action();
            locker.Dispose();
        }

        //! 是否正在录屏
        public bool IsRecording()
        {
            return CoreView.isRecording();
        }

        //! 返回已录制的相对毫秒数
        public int GetRecordTicks()
        {
            return CoreView.getRecordTick(false, WPFGraphView.getTick());
        }

        //! 开始录屏
        public bool StartRecord(String path)
        {
            if (CoreView.isRecording())
                return false;

            try
            {
                var dir = new DirectoryInfo(path);
                if (dir.Exists)
                    dir.Delete(true);
                dir.Create();
            }
            catch (IOException)
            {
                return false;
            }

            return CoreView.startRecord(path, CoreView.acquireFrontDoc(),
                false, WPFGraphView.getTick());
        }

        //! 停止录屏
        public void StopRecord()
        {
            CoreView.stopRecord(false);
        }

        //! 显示消息文字
        public void ShowMessage(string text)
        {
            ViewAdapter.showMessage(text);
        }

        //! 得到本地化文字
        public string GetLocalizedString(string name)
        {
            return WPFImageSourceHelper.Instance.GetLocalizedString(name);
        }
    }
}
