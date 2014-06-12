//! \file WPFCanvasAdapter.cs
//! \brief 定义WPF画布接口适配器类
// Copyright (c) 2013, https://github.com/rhcad/touchvg

using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Globalization;
using touchvg.core;

namespace touchvg.view
{
    //! WPF画布接口适配器类
    public class WPFCanvasAdapter : GiCanvas
    {
        private Pen _pen;
        private Brush _brush;
        private PathGeometry _path;
        private PathFigure _pathFigure;
        private DrawingContext _dc;

        public WPFCanvasAdapter()
        {
            _pen = new Pen()
            {
                Brush = new SolidColorBrush(Colors.Black),
                Thickness = 1,
                StartLineCap = PenLineCap.Round,
                EndLineCap = PenLineCap.Round,
            };
            _pen.Freeze();
        }

        public override void Dispose()
        {
            base.Dispose();
        }

        public void BeginDraw(DrawingContext dc)
        {
            this._dc = dc;
        }

        public void EndDraw()
        {
            this._dc = null;
        }

        public override void clearRect(float x, float y, float w, float h)
        {
        }

        private Color toColor(int argb)
        {
            return Color.FromArgb(
                    (byte)((argb >> 24) & 0xFF),
                    (byte)((argb >> 16) & 0xFF),
                    (byte)((argb >> 8) & 0xFF),
                    (byte)(argb & 0xFF));
        }

        public override void setPen(int argb, float width, int style, float phase, float orgw)
        {
            _pen = _pen.Clone();

            if (argb != 0)
            {
                (_pen.Brush as SolidColorBrush).Color = toColor(argb);
            }
            if (width > 0)
            {
                _pen.Thickness = width;
            }
            switch (style)
            {
                case 0:
                    _pen.DashStyle = DashStyles.Solid;
                    break;
                case 1:
                    _pen.DashStyle = DashStyles.Dash;
                    break;
                case 2:
                    _pen.DashStyle = DashStyles.Dot;
                    break;
                case 3:
                    _pen.DashStyle = DashStyles.DashDot;
                    break;
                case 4:
                    _pen.DashStyle = DashStyles.DashDotDot;
                    break;
            }
            if (style > 0 && style < 5)
            {
                _pen.StartLineCap = PenLineCap.Flat;
                _pen.EndLineCap = PenLineCap.Flat;
            }
            else if (style >= 0)
            {
                _pen.StartLineCap = PenLineCap.Round;
                _pen.EndLineCap = PenLineCap.Round;
            }
            _pen.Freeze();
        }

        public override void setBrush(int argb, int style)
        {
            byte alpha = (byte)((argb >> 24) & 0xFF);

            if (style == 0)
            {
                if (alpha == 0)
                {
                    _brush = null;
                }
                else
                {
                    if (_brush != null && _brush is SolidColorBrush)
                    {
                        _brush = _brush.Clone();
                        (_brush as SolidColorBrush).Color = toColor(argb);
                    }
                    else
                    {
                        _brush = new SolidColorBrush(toColor(argb));
                    }
                    _brush.Freeze();
                }
            }
        }

        public override void drawLine(float x1, float y1, float x2, float y2)
        {
            _dc.DrawLine(_pen, new Point(x1, y1), new Point(x2, y2));
        }

        public override void drawEllipse(float x, float y, float w, float h, bool stroke, bool fill)
        {
            if (w > 0 && h > 0)
            {
                var center = new Point(x + w / 2, y + h / 2);
                _dc.DrawEllipse(_brush, _pen, center, w / 2, h / 2);
            }
        }

        public override void drawRect(float x, float y, float w, float h, bool stroke, bool fill)
        {
            if (w > 0 && h > 0)
            {
                _dc.DrawRectangle(_brush, _pen, new Rect(x, y, w, h), null);
            }
        }

        public override void beginPath()
        {
            _path = new PathGeometry();
            _pathFigure = null;
        }

        public override void moveTo(float x, float y)
        {
            _pathFigure = new PathFigure() { StartPoint = new Point(x, y) };
            _path.Figures.Add(_pathFigure);
        }

        public override void lineTo(float x, float y)
        {
            _pathFigure.Segments.Add(new LineSegment(new Point(x, y), false));
        }

        public override void bezierTo(float c1x, float c1y, float c2x, float c2y, float x, float y)
        {
            PolyBezierSegment segment = new PolyBezierSegment();
            segment.Points.Add(new Point(c1x, c1y));
            segment.Points.Add(new Point(c2x, c2y));
            segment.Points.Add(new Point(x, y));
            _pathFigure.Segments.Add(segment);
        }

        public override void quadTo(float cpx, float cpy, float x, float y)
        {
            PolyQuadraticBezierSegment segment = new PolyQuadraticBezierSegment();
            segment.Points.Add(new Point(cpx, cpy));
            segment.Points.Add(new Point(x, y));
            _pathFigure.Segments.Add(segment);
        }

        public override void closePath()
        {
            _pathFigure.IsClosed = true;
        }

        public override void drawPath(bool stroke, bool fill)
        {
            foreach (var figure in _path.Figures)
            {
                figure.IsFilled = fill;
                foreach (var segemnt in figure.Segments)
                    segemnt.IsStroked = stroke;
            }
            _dc.DrawGeometry(_brush, _pen, _path);
        }

        public override bool drawHandle(float x, float y, int type)
        {
            ImageSource source = WPFImageSourceHelper.Instance.HandleImageSource(type);
            if (source != null)
            {
                _dc.DrawImage(source, new Rect(x - source.Width / 2,
                    y - source.Height / 2, source.Width, source.Height));
            }
            return source != null;
        }

        public override bool drawBitmap(string name, float xc, float yc,
                                        float w, float h, float angle)
        {
            if (w > 0 && h > 0)
            {
                BitmapImage imagedata = new BitmapImage(new Uri(name));

                imagedata.DecodePixelWidth = (int)w;
                imagedata.DecodePixelHeight = (int)h;

                var matrix = new Matrix();
                matrix.RotateAt(angle, xc, yc);
                var rect = Rect.Transform(new Rect(xc - w / 2, yc - h / 2, w, h), matrix);
                _dc.DrawImage(imagedata, rect);
            }
            return w > 0 && h > 0;
        }

        public override float drawTextAt(string text, float x, float y, float h, int align)
        {
            if (text == null || text.Length < 1)
                return 0;

            if (text[0] == '@')
            {
                text = WPFImageSourceHelper.Instance.GetLocalizedString(text.Substring(1));
            }

            FormattedText textFormation = new FormattedText(text,
                CultureInfo.CurrentCulture, FlowDirection.LeftToRight,
                new Typeface("宋体"), h, _brush)
                {
                    TextAlignment = (TextAlignment)Enum.Parse(typeof(TextAlignment), align.ToString())
                };
            _dc.DrawText(textFormation, new Point(x, y));
            return (float)textFormation.Width;
        }
    }
}
