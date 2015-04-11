//! \file WPFImageSourceHelper.cs
//! \brief 定义WPF绘图图像源的缓存类
// Copyright (c) 2013, https://github.com/rhcad/touchvg

using System;
using System.Collections;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Interop;
using System.Drawing;
using touchvg.core;
using System.Resources;
using System.Collections.Generic;

namespace touchvg.view
{
    public delegate string StringCallback(string name);

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
            try
            {
                bitmapSource = Imaging.CreateBitmapSourceFromHBitmap(ip, IntPtr.Zero,
                    Int32Rect.Empty, BitmapSizeOptions.FromEmptyOptions());
            }
            finally
            {
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
        private StringCallback StringCallback;

        private WPFImageSourceHelper()
        {
            _actionImageDict = new Dictionary<int, ImageSource>();
            AddActionImage(MgContextAction.kMgActionSelAll, Resource1.vg_selall);
            AddActionImage(MgContextAction.kMgActionCancel, Resource1.vg_back);
            AddActionImage(MgContextAction.kMgActionDelete, Resource1.vg_delete);
            AddActionImage(MgContextAction.kMgActionClone, Resource1.vg_clone);
            AddActionImage(MgContextAction.kMgActionFixedLength, Resource1.vg_fixlen);
            AddActionImage(MgContextAction.kMgActionFreeLength, Resource1.vg_freelen);
            AddActionImage(MgContextAction.kMgActionLocked, Resource1.vg_lock);
            AddActionImage(MgContextAction.kMgActionUnlocked, Resource1.vg_unlock);
            AddActionImage(MgContextAction.kMgActionEditVertex, Resource1.vg_edit);
            AddActionImage(MgContextAction.kMgActionHideVertex, Resource1.vg_back);
            AddActionImage(MgContextAction.kMgActionGroup, Resource1.vg_group);
            AddActionImage(MgContextAction.kMgActionUngroup, Resource1.vg_ungroup);
            AddActionImage(MgContextAction.kMgActionOverturn, Resource1.vg_overturn);

            int i = 0;
            _handleImages = new ImageSource[17]; // GiHandleTypes
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot1);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot2);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgdot3);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_lock);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_unlock);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_back);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_endedit);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgnode);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgcen);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgmid);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgquad);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgtangent);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgcross);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgparallel);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgnear);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vgpivot);
            _handleImages[i++] = WPFImageSourceHelper.BitmapToImageSource(Resource1.vg_overturn);
        }

        private void AddActionImage(MgContextAction key, Bitmap bitmap)
        {
            AddActionImage((int)key, bitmap);
        }

        public void AddActionImage(int key, Bitmap bitmap)
        {
            ImageSource s = BitmapToImageSource(bitmap);
            if (s == null)
                _actionImageDict.Remove(key);
            else if (!_actionImageDict.ContainsKey(key))
                _actionImageDict.Add(key, s);
            else
                _actionImageDict[key] = s;
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
            if (type >= 0 && type < _handleImages.Length
                && _handleImages[type] != null)
            {
                return _handleImages[type].Clone();
            }
            return null;
        }

        public void AddLocalizedStringCallback(StringCallback StringCallback)
        {
            this.StringCallback = StringCallback;
        }

        public string GetLocalizedString(string name)
        {
            string str = Resource1.ResourceManager.GetString(name, Resource1.Culture);
            str = str != null ? str : name;
            return StringCallback != null && str.Equals(name) ? StringCallback(name) : str;
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
