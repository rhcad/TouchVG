using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using touchvg.view;
using System.Windows.Media;
using democmds.api;

namespace WpfDemo
{
    /// <summary>
    /// Interaction logic for Window1.xaml
    /// </summary>
    public partial class Window1 : Window
    {
        public Window1()
        {
            InitializeComponent();
            this.Loaded += new RoutedEventHandler(Window1_Loaded);
            this.Unloaded += new RoutedEventHandler(Window1_Unloaded);
        }
        string[] _commands = new string[] {
            "选择",     "select",
            "删除",      "erase",
            "随手画",    "splines",
            "直线段",    "line",
            "矩形",      "rect",
            "正方形",    "square",
            "椭圆",      "ellipse",
            "圆",        "circle",
            "三角形",    "triangle",
            "菱形",      "diamond",
            "多边形",    "polygon",
            "四边形",    "quadrangle",
            "折线",      "lines",
            "曲线",      "spline_mouse",
            "平行四边形", "parallel",
            "网格",      "grid",
            "三点圆弧",  "arc3p",
            "圆心圆弧",  "arc_cse",
            "切线圆弧",  "arc_tan",
            "点击测试(in DemoCmds)",  "hittest"
        };
        string[] _lineStyles = new string[] {
            "实线",     "0",
            "虚线",     "1",
            "点线",     "2",
            "点划线",   "3",
            "双点划线", "4",
        };

        private WPFGraphView _view;
        private WPFViewHelper _helper;
        private bool _updateLocked = false;

        void Window1_Loaded(object sender, RoutedEventArgs e)
        {
            _view = new WPFGraphView(canvas1);
            _view.OnCommandChanged += new CommandChangedEventHandler(View_OnCommandChanged);
            _view.OnSelectionChanged += new touchvg.view.SelectionChangedEventHandler(View_OnSelectionChanged);

            List<KeyValuePair<string, string>> commandSource = new List<KeyValuePair<string, string>>();
            for (int i = 0; i < _commands.Length; i += 2)
            {
                commandSource.Add(new KeyValuePair<string, string>(_commands[i], _commands[i + 1]));
            }
            this.cboCmd.DisplayMemberPath = "Key";
            this.cboCmd.SelectedValuePath = "Value";
            this.cboCmd.ItemsSource = commandSource;
            this.cboCmd.SelectedIndex = 0;

            List<KeyValuePair<string, string>> lineStyleSource = new List<KeyValuePair<string, string>>();
            for (int i = 0; i < _lineStyles.Length; i += 2)
            {
                lineStyleSource.Add(new KeyValuePair<string, string>(_lineStyles[i], _lineStyles[i + 1]));
            }
            this.cboLineStyle.DisplayMemberPath = "Key";
            this.cboLineStyle.SelectedValuePath = "Value";
            this.cboLineStyle.ItemsSource = lineStyleSource;
            this.cboLineStyle.SelectedIndex = 0;

            _helper = new WPFViewHelper(_view);
            DemoCmds.registerCmds(_helper.CmdViewHandle());
            _helper.Load("C:\\Test\\page.vg");
            _helper.StartUndoRecord("C:\\Test\\undo");
            _helper.Command = "select";
        }

        void Window1_Unloaded(object sender, RoutedEventArgs e)
        {
            _helper.Dispose();
        }

        private void cboCmd_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_helper != null && !_updateLocked)
            {
                _helper.Command = cboCmd.SelectedValue.ToString();
            }
        }

        void View_OnCommandChanged(object sender, EventArgs e)
        {
            string cmdname = cboCmd.SelectedValue.ToString();
            if (!_helper.Command.Equals(cmdname))
            {
                _updateLocked = true;
                cboCmd.SelectedValue = _helper.Command;
                _updateLocked = false;
            }
            View_OnSelectionChanged(sender, e);
        }

        private void cboLineStyle_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_helper != null && !_updateLocked)
            {
                _helper.LineStyle = int.Parse(cboLineStyle.SelectedValue.ToString());
            }
        }

        void View_OnSelectionChanged(object sender, EventArgs e)
        {
            if (!_updateLocked)
            {
                _updateLocked = true;
                alphaSlider.Value = (double)_helper.LineAlpha;
                widthSlider.Value = (double)_helper.LineWidth;
                cboLineStyle.SelectedValue = _helper.LineStyle.ToString();
                _updateLocked = false;
            }
        }

        private void alphaSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_helper != null && !_updateLocked)
                _helper.LineAlpha = (int)(sender as Slider).Value;
        }

        private void widthSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (_helper != null && !_updateLocked)
                _helper.LineWidth = (int)(sender as Slider).Value;
        }

        private void redBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.LineColor = Colors.Red;
        }

        private void blueBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.LineColor = Colors.Blue;
        }

        private void saveBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.Save("C:\\Test\\page.vg");
            _helper.ExportSVG("C:\\Test\\page.svg");
            _helper.ExportPNG("C:\\Test\\page.---");
        }

        private void undoBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.Undo();
        }

        private void redoBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.Redo();
        }

        private void zoomExtentBtn_Click(object sender, RoutedEventArgs e)
        {
            _helper.ZoomToExtent();
        }
    }
}
