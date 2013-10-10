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
            "平行四边形", "parallelogram",
            "网格",      "grid",
            "三点圆弧",  "arc3p",
            "圆心圆弧",  "arc_cse",
            "切线圆弧",  "arc_tan",
            "点击测试(in DemoCmds)",  "hittest"
        };

        private WPFGraphView _view;
        private WPFViewHelper _helper;
        private bool _updateLocked = false;

        void Window1_Loaded(object sender, RoutedEventArgs e)
        {
            _view = new WPFGraphView(canvas1);
            _helper = new WPFViewHelper(_view);
            _view.OnCommandChanged += new CommandChangedEventHandler(View_OnCommandChanged);
            _view.OnSelectionChanged +=new touchvg.view.SelectionChangedEventHandler(View_OnSelectionChanged);

            List<KeyValuePair<string, string>> commandSource = new List<KeyValuePair<string, string>>();
            for (int i = 0; i < _commands.Length; i += 2)
            {
                commandSource.Add(new KeyValuePair<string, string>(_commands[i], _commands[i + 1]));
            }
            this.comboBox1.DisplayMemberPath = "Key";
            this.comboBox1.SelectedValuePath = "Value";
            this.comboBox1.ItemsSource = commandSource;
            this.comboBox1.SelectedIndex = 0;

            DemoCmds.registerCmds(_helper.cmdViewHandle());
        }

        void Window1_Unloaded(object sender, RoutedEventArgs e)
        {
            _helper.Dispose();
        }

        private void comboBox1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (_helper != null && !_updateLocked)
            {
                _helper.Command = comboBox1.SelectedValue.ToString();
                View_OnSelectionChanged(sender, e);
            }
        }

        void View_OnCommandChanged(object sender, EventArgs e)
        {
            string cmdname = comboBox1.SelectedValue.ToString();
            if (!_helper.Command.Equals(cmdname))
            {
                _updateLocked = true;
                comboBox1.SelectedValue = _helper.Command;
                _updateLocked = false;
            }
        }

        void View_OnSelectionChanged(object sender, EventArgs e)
        {
            if (!_updateLocked)
            {
                _updateLocked = true;
                alphaSlider.Value = (double)_helper.LineAlpha;
                widthSlider.Value = (double)_helper.LineWidth;
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
    }
}
