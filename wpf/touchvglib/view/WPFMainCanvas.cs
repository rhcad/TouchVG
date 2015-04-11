//! \file WPFMainCanvas.cs
//! \brief 定义WPF静态图形画布视图类
// Copyright (c) 2013, https://github.com/rhcad/touchvg

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Controls;
using System.Windows.Input;
using touchvg.core;

namespace touchvg.view
{
    //! WPF静态图形画布视图类
    public class WPFMainCanvas : Canvas
    {
        GiCoreView _coreView;
        GiView _view;
        WPFCanvasAdapter _adapter;

        public WPFMainCanvas(GiCoreView coreView, GiView view)
        {
            this._coreView = coreView;
            this._view = view;
            this._adapter = new WPFCanvasAdapter();
        }

        public void clean()
        {
            _coreView = null;
            _view = null;
            if (_adapter != null)
            {
                _adapter.Dispose();
                _adapter = null;
            }
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);
            if (this._adapter != null)
            {
                this._adapter.BeginDraw(dc);
                this._coreView.drawAll(this._view, this._adapter);
                this._adapter.EndDraw();
            }
        }
    }

    //! WPF动态图形画布视图类
    public class WPFTempCanvas : Canvas
    {
        GiCoreView _coreView;
        GiView _view;
        WPFCanvasAdapter _adapter;
        GiMouseHelper _helper;

        public WPFTempCanvas(GiCoreView coreView, GiView view)
        {
            this._coreView = coreView;
            this._view = view;
            this._adapter = new WPFCanvasAdapter();
            _helper = new GiMouseHelper(this._view, this._coreView);
            this.MouseDown += new MouseButtonEventHandler(TempCanvas_MouseDown);
            this.MouseUp += new MouseButtonEventHandler(TempCanvas_MouseUp);
        }

        public void clean()
        {
            _coreView = null;
            _view = null;
            if (_adapter != null)
            {
                _adapter.Dispose();
                _adapter = null;
            }
            if (_helper != null)
            {
                _helper.Dispose();
                _helper = null;
            }
        }

        private int lastClickTime = 0;

        void TempCanvas_MouseUp(object sender, MouseButtonEventArgs e)
        {
            Point pt = e.GetPosition(this);

            _helper.onMouseUp((float)pt.X, (float)pt.Y);
            if (e.Timestamp - lastClickTime < 300)
            {
                _helper.onLButtonDblClk((float)pt.X, (float)pt.Y);
            }
            lastClickTime = e.Timestamp;
            this.ReleaseMouseCapture();
            this.MouseMove -= new MouseEventHandler(TempCanvas_MouseMove);
        }

        void TempCanvas_MouseMove(object sender, MouseEventArgs e)
        {
            Point pt = e.GetPosition(this);

            if (pt.X >= 0 && pt.X <= this.Width
                && pt.Y >= 0 && pt.Y <= this.Height)
            {
                _helper.onMouseMove((float)pt.X, (float)pt.Y,
                    e.LeftButton == MouseButtonState.Pressed,
                    e.RightButton == MouseButtonState.Pressed);
            }
            lastClickTime = 0;
        }

        void TempCanvas_MouseDown(object sender, MouseButtonEventArgs e)
        {
            this.CaptureMouse();
            this.MouseMove += new MouseEventHandler(TempCanvas_MouseMove);

            if (e.LeftButton == MouseButtonState.Pressed)
            {
                Point pt = e.GetPosition(this);
                bool ctrl = Keyboard.IsKeyDown(Key.LeftCtrl) || Keyboard.IsKeyDown(Key.RightCtrl);
                bool shift = Keyboard.IsKeyDown(Key.LeftShift) || Keyboard.IsKeyDown(Key.RightShift);
                _helper.onLButtonDown((float)pt.X, (float)pt.Y, ctrl, shift);
            }
            else if (e.RightButton == MouseButtonState.Pressed)
            {
                Point pt = e.GetPosition(this);
                _helper.onRButtonDown((float)pt.X, (float)pt.Y);
            }
        }

        protected override void OnRender(DrawingContext dc)
        {
            base.OnRender(dc);
            if (this._adapter != null)
            {
                this._adapter.BeginDraw(dc);
                this._coreView.dynDraw(this._view, this._adapter);
                this._adapter.EndDraw();
            }
        }
    }
}
