using System;
using System.Windows.Media;
using touchvg.core;
using System.Text;

namespace touchvg.view
{
    //! WPF绘图视图辅助类
    /*! \ingroup GROUP_WPF
     */
    public class WPFViewHelper : IDisposable
    {
        private WPFGraphView View;
        private GiCoreView CoreView { get { return this.View.CoreView; } }

        public WPFViewHelper(WPFGraphView view)
        {
            this.View = view;
        }

        public void Dispose()
        {
            if (this.View != null)
            {
                this.View.Dispose();
                this.View = null;
            }
        }

        //! 返回内核视图的句柄, MgView 指针
        public int cmdViewHandle()
        {
            return CoreView.viewAdapterHandle();
        }

        //! 返回内核命令视图
        public MgView cmdView()
        {
            return CoreView.viewAdapter();
        }

        //! 当前命令名称
        public string Command
        {
            get { return CoreView.getCommand(); }
            set { CoreView.setCommand(View.ViewAdapter, value); }
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
                CoreView.setContext((int)GiContextBits.kContextLineWidth);
            }
        }

        //! 像素单位的线宽，总是为正数
        public int StrokeWidth
        {
            get
            {
                GiContext ctx = CoreView.getContext(false);
                return (int)CoreView.calcPenWidth(ctx.getLineWidth());
            }
            set
            {
                CoreView.getContext(true).setLineWidth(
                    -Math.Abs((float)value), true);
                CoreView.setContext((int)GiContextBits.kContextLineWidth);
            }
        }

        //! 线型, 0-5:实线,虚线,点线,点划线,双点划线,空线
        public int LineStyle
        {
            get { return CoreView.getContext(false).getLineStyle(); }
            set
            {
                CoreView.getContext(true).setLineStyle(value);
                CoreView.setContext((int)GiContextBits.kContextLineStyle);
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
                    CoreView.setContext((int)GiContextBits.kContextLineRGB);
                else
                    CoreView.setContext((int)GiContextBits.kContextLineARGB);
            }
        }

        //! 线条透明度, 0-255
        public int LineAlpha
        {
            get { return CoreView.getContext(false).getLineAlpha(); }
            set
            {
                CoreView.getContext(true).setLineAlpha(value);
                CoreView.setContext((int)GiContextBits.kContextLineAlpha);
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
                    CoreView.setContext((int)GiContextBits.kContextFillRGB);
                else
                    CoreView.setContext((int)GiContextBits.kContextFillARGB);
            }
        }

        //! 填充透明度, 0-255
        public int FillAlpha
        {
            get { return CoreView.getContext(false).getFillAlpha(); }
            set
            {
                CoreView.getContext(true).setFillAlpha(value);
                CoreView.setContext((int)GiContextBits.kContextFillAlpha);
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

        //! 所有图形的JSON内容
        public string Content
        {
            get
            {
                string ret = CoreView.getContent();
                CoreView.freeContent();
                return ret;
            }
            set { CoreView.setContent(value); }
        }

        //! 放缩显示全部内容到视图区域
        public bool ZoomToExtent()
        {
            return CoreView.zoomToExtent();
        }

        //! 放缩显示指定范围到视图区域
        public bool ZoomToModel(double w, double h)
        {
            return CoreView.zoomToModel(0, 0, (float)w, (float)h);
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

        //! 保存图形到JSON文件
        public bool Save(string vgfile)
        {
            return CoreView.saveToFile(vgfile);
        }
    }
}
