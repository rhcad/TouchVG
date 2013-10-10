using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Input;
using touchvg.core;

namespace touchvg.view
{
    public delegate void CommandChangedEventHandler(object sender, EventArgs e);
    public delegate void SelectionChangedEventHandler(object sender, EventArgs e);
    public delegate void ContentChangedEventHandler(object sender, EventArgs e);

    //! WPF绘图视图类
    /*! \ingroup GROUP_WPF
     */
    public class WPFGraphView : IDisposable
    {
        public GiCoreView CoreView { get; private set; }
        private WPFViewAdapter _view;
        public GiView ViewAdapter { get { return _view; } }
        public WPFMainCanvas MainCanvas { get; private set; }
        public WPFTempCanvas TempCanvas { get; private set; }
        public event CommandChangedEventHandler OnCommandChanged;
        public event SelectionChangedEventHandler OnSelectionChanged;
        public event ContentChangedEventHandler OnContentChanged;

        public WPFGraphView(Panel container)
        {
            this.CoreView = new GiCoreView();
            this._view = new WPFViewAdapter(this);
            this.CoreView.createView(this._view);

            MainCanvas = new WPFMainCanvas(this.CoreView, _view) { Width = container.ActualWidth, Height = container.ActualHeight };
            TempCanvas = new WPFTempCanvas(this.CoreView, _view) { Width = container.ActualWidth, Height = container.ActualHeight };
            TempCanvas.Background = new SolidColorBrush(Colors.Transparent);

            container.Children.Add(MainCanvas);
            container.Children.Add(TempCanvas);
            Panel.SetZIndex(TempCanvas, 1);

            container.SizeChanged += new SizeChangedEventHandler(container_SizeChanged);
            this.CoreView.onSize(_view, (int)container.ActualWidth, (int)container.ActualHeight);
        }

        private void container_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            var container = sender as Panel;
            TempCanvas.Width = container.ActualWidth;
            TempCanvas.Height = container.ActualHeight;
            MainCanvas.Width = container.ActualWidth;
            MainCanvas.Height = container.ActualHeight;
            this.CoreView.onSize(_view, (int)container.ActualWidth, (int)container.ActualHeight);
            _view.ClearActions();
        }

        public void Dispose()
        {
            if (this.MainCanvas != null)
            {
                this.MainCanvas.clean();
                this.MainCanvas = null;
            }
            if (this.TempCanvas != null)
            {
                this.TempCanvas.clean();
                this.TempCanvas = null;
            }
            if (_view != null)
            {
                this.CoreView.destoryView(_view);
                _view.Dispose();
                _view = null;
            }
            if (this.CoreView != null)
            {
                this.CoreView.Dispose();
                this.CoreView = null;
            }
            WPFImageSourceHelper.Clean();
        }

        //! WPF绘图视图适配器类
        private class WPFViewAdapter : GiView
        {
            WPFGraphView _owner;
            GiCoreView CoreView { get { return _owner.CoreView; } }
            List<Image> ActionImages;
            List<Button> ActionButtons;

            public WPFViewAdapter(WPFGraphView owner)
            {
                this._owner = owner;
                ActionImages = new List<Image>();
            }

            public override void Dispose()
            {
                _owner = null;
                base.Dispose();
            }

            public override void regenAll()
            {
                _owner.MainCanvas.InvalidateVisual();
                _owner.TempCanvas.InvalidateVisual();
            }

            public override void regenAppend()
            {
                regenAll();
            }

            public override void redraw()
            {
                _owner.TempCanvas.InvalidateVisual();
            }

            public override void commandChanged()
            {
                if (_owner.OnCommandChanged != null)
                    _owner.OnCommandChanged.Invoke(_owner, null);
            }

            public override void selectionChanged()
            {
                if (_owner.OnSelectionChanged != null)
                    _owner.OnSelectionChanged.Invoke(_owner, null);
            }

            public override void contentChanged()
            {
                if (_owner.OnContentChanged != null)
                    _owner.OnContentChanged.Invoke(_owner, null);
            }

            public override bool useFinger()
            {
                return false;
            }

            private static string[] buttonCaptions = { null, "全选", "重选", "绘图",
            "取消", "删除", "克隆", "定长", "不定长", "锁定", "解锁", "编辑","返回",
            "闭合", "不闭合", "加点", "删点", "成组", "解组", "翻转" };

            public override bool isContextActionsVisible()
            {
                return ActionImages.Count > 0
                    || (ActionButtons != null && ActionButtons.Count > 0);
            }

            public override bool showContextActions(Ints actions, Floats buttonXY,
                float x, float y, float w, float h)
            {
                ClearActions();
                if (!createActionImages(actions, buttonXY))
                    createActionButtons(actions, buttonXY);
                return isContextActionsVisible();
            }

            private bool createActionImages(Ints actions, Floats buttonXY)
            {
                int actionCount = actions.count();
                for (int i = 0; i < actionCount; i++)
                {
                    int cmdIndex = actions.get(i);
                    if (cmdIndex >= buttonCaptions.Length)
                        continue;

                    ImageSource imageSource = WPFImageSourceHelper.Instance.ActionImageSource(cmdIndex);
                    if (imageSource == null)
                        continue;

                    string cmdName = buttonCaptions[cmdIndex];
                    System.Diagnostics.Trace.WriteLine(string.Format("{0},{1}", cmdIndex, cmdName));

                    Image image = new Image()
                    {
                        Source = imageSource,
                        ToolTip = cmdName,
                        Tag = cmdIndex,
                        Width = imageSource.Width,
                        Height = imageSource.Height
                    };
                    image.MouseDown += new MouseButtonEventHandler(image_MouseDown);
                    _owner.TempCanvas.Children.Add(image);
                    Canvas.SetLeft(image, buttonXY.get(2 * i) - image.Width / 2);
                        Canvas.SetTop(image, buttonXY.get(2 * i + 1) - image.Height / 2);
                        ActionImages.Add(image);
                    ActionImages.Add(image);
                }

                return actionCount == 0 || ActionImages.Count > 0;
            }

            private void createActionButtons(Ints actions, Floats buttonXY)
            {
                if (ActionButtons == null)
                    ActionButtons = new List<Button>();

                int actionCount = actions.count();
                for (int i = 0; i < actionCount; i++)
                {
                    int cmdIndex = actions.get(i);
                    if (cmdIndex >= buttonCaptions.Length)
                        continue;

                    string cmdName = buttonCaptions[cmdIndex];

                    Button button = new Button()
                    {
                        Content = cmdName,
                        Tag = cmdIndex,
                        Width = cmdName.Length * 14 + 8,
                        Height = 24
                    };
                    button.Click += new RoutedEventHandler(button_Click);
                    _owner.TempCanvas.Children.Add(button);
                    Canvas.SetLeft(button, buttonXY.get(2 * i) - button.Width / 2);
                    Canvas.SetTop(button, buttonXY.get(2 * i + 1) - button.Height / 2);
                    ActionButtons.Add(button);
                }
            }

            private void image_MouseDown(object sender, MouseButtonEventArgs e)
            {
                int action = Convert.ToInt32((sender as FrameworkElement).Tag);
                ClearActions();
                CoreView.doContextAction(action);
                e.Handled = true;
            }

            private void button_Click(object sender, RoutedEventArgs e)
            {
                int action = Convert.ToInt32((sender as FrameworkElement).Tag);
                ClearActions();
                CoreView.doContextAction(action);
                e.Handled = true;
            }

            public void ClearActions()
            {
                for (int i = 0; i < ActionImages.Count; i++)
                {
                    _owner.TempCanvas.Children.Remove(ActionImages[i]);
                }
                ActionImages.Clear();

                if (ActionButtons != null)
                {
                    for (int i = 0; i < ActionButtons.Count; i++)
                    {
                        _owner.TempCanvas.Children.Remove(ActionButtons[i]);
                    }
                    ActionButtons.Clear();
                }
            }
        }
    }
}
