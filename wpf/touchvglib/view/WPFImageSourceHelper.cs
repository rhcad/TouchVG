using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Drawing;

namespace touchvg.view
{
    //! WPF绘图图像源的缓存类
    public class WPFImageSourceHelper : IDisposable
    {
        [DllImport("gdi32.dll", SetLastError = true)]
        private static extern bool DeleteObject(IntPtr hObject);

        public static ImageSource BitmapToImageSource(Bitmap bitmap)
        {
            if (bitmap == null)
                return null;
            IntPtr ip = bitmap.GetHbitmap();
            BitmapSource bitmapSource = null;
            try {
                bitmapSource = Imaging.CreateBitmapSourceFromHBitmap(ip, IntPtr.Zero, 
                    Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
            }
            finally {
                DeleteObject(ip);
            }
            return bitmapSource;
        }

        static WPFImageSourceHelper()
        {
            _locker = new object();
        }

        private static object _locker;
        private static WPFImageSourceHelper _instance;

        public static WPFImageSourceHelper Instance
        {
            get
            {
                if (_instance == null)
                {
                    lock (_locker)
                    {
                        if (_instance == null)
                        {
                            _instance = new WPFImageSourceHelper();
                        }
                    }
                }
                return _instance;
            }
            set
            {
                _instance = value;
            }
        }

        private Dictionary<int, ImageSource> _actionImageDict;
        private ImageSource[] _handleImages;

        public WPFImageSourceHelper()
        {
            _actionImageDict = new Dictionary<int, ImageSource>();
            AddActionImage(1, Resource1.vg_selall);
            AddActionImage(3, Resource1.vg_draw);
            AddActionImage(4, Resource1.vg_back);
            AddActionImage(5, Resource1.vg_delete);
            AddActionImage(6, Resource1.vg_clone);
            AddActionImage(7, Resource1.vg_fixlen);
            AddActionImage(8, Resource1.vg_freelen);
            AddActionImage(9, Resource1.vg_lock);
            AddActionImage(10, Resource1.vg_unlock);
            AddActionImage(11, Resource1.vg_edit);
            AddActionImage(12, Resource1.vg_back);
            AddActionImage(15, Resource1.vg_addvertex);
            AddActionImage(16, Resource1.vg_delvertex);
            AddActionImage(17, Resource1.vg_group);
            AddActionImage(18, Resource1.vg_ungroup);
            AddActionImage(19, Resource1.vg_overturn);
            //AddActionImage(40, Resource1.vg_break);

            _handleImages = new ImageSource[7];
            _handleImages[0] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot1);
            _handleImages[1] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot2);
            _handleImages[2] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot3);
            _handleImages[3] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_lock);
            _handleImages[4] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_unlock);
            _handleImages[5] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_back);
            _handleImages[6] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_endedit);
        }

        private void AddActionImage(int key, Bitmap bitmap)
        {
            if (bitmap != null)
            {
                _actionImageDict.Add(key, BitmapToImageSource(bitmap));
            }
        }

        public ImageSource ActionImageSource(int index)
        {
            if (_actionImageDict.ContainsKey(index))
            {
                return _actionImageDict[index].Clone();
            }
            return null;
        }

        public ImageSource HandleImageSource(int type)
        {
            if (type >= 0 && type < _handleImages.Length)
            {
                return _handleImages[type].Clone();
            }
            return null;
        }

        /// <summary>
        /// 释放静态资源
        /// </summary>
        public static void Clean()
        {
            Instance.Dispose();
            Instance = null;
        }

        public void Dispose()
        {
            if (_actionImageDict != null)
            {
                _actionImageDict.Clear();
                _actionImageDict = null;
            }
            if (_handleImages != null)
            {
                for (int i = 0; i < _handleImages.Length; i++)
                {
                    _handleImages[i] = null;
                }
                _handleImages = null;
            }
        }
    }
}
