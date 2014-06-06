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

namespace touchvg.view
{
    //! WPF绘图视图辅助类
    /*! \ingroup GROUP_WPF
     */
    public class WPFViewHelper : IDisposable
    {
        private static int LIB_RELEASE = 1; // TODO: 在本工程接口变化后增加此数
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
        public bool isCommand(string name)
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

        //! 图形总数
        public int ShapeCount
        {
            get { return CoreView.getShapeCount(); }
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

        //! 图形显示范围
        public Rect DisplayExtent
        {
            get
            {
                Floats box = new Floats(4);
                if (CoreView.getDisplayExtent(box))
                {
                    return new Rect(box.get(0), box.get(1),
                        box.get(2) - box.get(0), box.get(3) - box.get(1));
                }
                return new Rect();
            }
        }

        //! 选择包络框
        public Rect BoundingBox
        {
            get
            {
                Floats box = new Floats(4);
                if (CoreView.getBoundingBox(box))
                {
                    return new Rect(box.get(0), box.get(1),
                        box.get(2) - box.get(0), box.get(3) - box.get(1));
                }
                return new Rect();
            }
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

        //! 导出静态图形到SVG文件
        public bool ExportSVG(string filename)
        {
            return CoreView.exportSVG(ViewAdapter, filename) > 0;
        }

        //! 放缩显示全部内容到视图区域
        public bool ZoomToExtent()
        {
            return CoreView.zoomToExtent();
        }

        //! 放缩显示指定范围到视图区域
        public bool ZoomToModel(float x, float y, float w, float h)
        {
            return CoreView.zoomToModel(x, y, w, h);
        }

        //! 添加测试图形
        public int AddShapesForTest()
        {
            return CoreView.addShapesForTest();
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

        //! 返回显示比例
        public float GetViewScale()
        {
            return CmdView().xform().getViewScale();
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

        //! 清除所有图形
        public void Clear()
        {
            CoreView.clear();
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
    }
}
