//! \file ImageCache.java
//! \brief Image cache and map class
// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.internal;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.content.res.Resources;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.PictureDrawable;
import android.support.v4.util.LruCache;
import android.util.Log;
import android.view.View;

//! Image cache and map class
public class ImageCache extends Object {
    // USE_SVG=true: Uses androidsvg-xxx.jar
    public static final boolean USE_SVG = false;
    private static final String TAG = "touchvg";
    public static final String BITMAP_PREFIX = "bmp:";
    public static final String SVG_PREFIX = "svg:";
    private static final int CACHE_SIZE = 2 * 1024 * 1024;
    private LruCache<String, Drawable> mCache;
    private String mPath;
    private String mPlayPath;

    public ImageCache() {
    }

    private boolean createCache() {
        try {
            mCache = new LruCache<String, Drawable>(CACHE_SIZE) {
                @Override
                protected int sizeOf(String key, Drawable d) {
                    // TODO: SVG size?
                    int size = 1;
                    if (d.getClass().isInstance(BitmapDrawable.class)) {
                        size = ((BitmapDrawable) d).getBitmap().getByteCount();
                    }
                    return size;
                }
            };
        } catch (NoClassDefFoundError e) {
            Log.e(TAG, "Need android-support-v4.jar in application", e);
        }
        return mCache != null;
    }

    private void addToCache(String name, Drawable drawable) {
        if (mCache != null || createCache()) {
            mCache.put(name, drawable);
        }
    }

    public String getImagePath() {
        return mPath;
    }

    public void setImagePath(String path) {
        this.mPath = path;
    }

    public void setPlayPath(String path) {
        this.mPlayPath = path;
    }

    public static int getWidth(Drawable drawable) {
        int w = 0;
        try {
            w = ((BitmapDrawable) drawable).getBitmap().getWidth();
        } catch (ClassCastException e) {
            Log.v(TAG, "Not BitmapDrawable(w)", e);
            try {
                w = ((PictureDrawable) drawable).getPicture().getWidth();
            } catch (ClassCastException e2) {
                Log.i(TAG, "Not PictureDrawable(w)", e);
            }
        }
        return w;
    }

    public static int getHeight(Drawable drawable) {
        int h = 0;
        try {
            h = ((BitmapDrawable) drawable).getBitmap().getHeight();
        } catch (ClassCastException e) {
            Log.v(TAG, "Not BitmapDrawable(h)", e);
            try {
                h = ((PictureDrawable) drawable).getPicture().getHeight();
            } catch (ClassCastException e2) {
                Log.i(TAG, "Not PictureDrawable(h)", e);
            }
        }
        return h;
    }

    //! Clear all cached objects
    public void clear() {
        if (mCache != null) {
            mCache.evictAll();
        }
    }

    //! Find image object, not automatically loaded
    public final Bitmap getBitmap(String name) {
        BitmapDrawable drawable = null;
        try {
            drawable = (BitmapDrawable)(mCache != null ? mCache.get(name) : null);
        } catch (ClassCastException e) {
            Log.v(TAG, "Not BitmapDrawable(getImage)", e);
        }
        return drawable != null ? drawable.getBitmap() : null;
    }

    //! Find or load image object
    public final Drawable getImage(View view, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && view != null) {
            if (name.indexOf(BITMAP_PREFIX) == 0) {
                // is R.drawable.resName
                final String resName = name.substring(BITMAP_PREFIX.length());
                int id = view.getResources().getIdentifier(resName, "drawable",
                        view.getContext().getPackageName());
                drawable = this.addBitmap(view.getResources(), id, name);
            } else if (name.indexOf(SVG_PREFIX) == 0) {
                // is R.raw.resName
                final String resName = name.substring(SVG_PREFIX.length());
                int id = view.getResources().getIdentifier(resName, "raw",
                        view.getContext().getPackageName());
                drawable = this.addSVG(view.getResources(), id, name);
            } else if (name.endsWith(".svg")) {
                drawable = this.addSVGFile(getImagePath(name), name);
            } else {
                drawable = this.addBitmapFile(view.getResources(), getImagePath(name), name);
            }
        }

        return drawable;
    }

    private String getImagePath(String name) {
        if (mPlayPath != null && !mPlayPath.isEmpty()) {
            final File f = new File(mPlayPath, name);
            if (f.exists()) {
                return f.getPath();
            }
        }
        if (mPath != null && !mPath.isEmpty()) {
            final File f = new File(mPath, name);
            if (!f.exists()) {
                Log.d(TAG, "File not exist: " + f.getPath());
            }
            return f.getPath();
        }
        return name;
    }

    //! Add image object in resource
    public final Drawable addBitmap(Resources res, int id, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && id != 0) {
            final Bitmap bitmap = BitmapFactory.decodeResource(res, id);

            if (bitmap != null && bitmap.getWidth() > 0) {
                drawable = new BitmapDrawable(res, bitmap);
                addToCache(name, drawable);
            }
        }

        return drawable;
    }

    //! Add SVG object in resource
    @SuppressWarnings("unused")
    public final Drawable addSVG(Resources res, int id, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && id != 0 && USE_SVG) {/*
            try {
                final Picture picture = SVG.getFromResource(res, id).renderToPicture();

                if (picture != null && picture.getWidth() > 0) {
                    drawable = new PictureDrawable(picture);
                    addToCache(name, drawable);
                }
            } catch (SVGParseException e) {
                Log.e(TAG, "Parse resource fail", e);
            }*/
        }

        return drawable;
    }

    //! Add image object from storage
    public final Drawable addBitmapFile(Resources res, String filename, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && new File(filename).exists()) {
            final BitmapFactory.Options opts = new BitmapFactory.Options();

            opts.inJustDecodeBounds = true;
            BitmapFactory.decodeFile(filename, opts);
            opts.inJustDecodeBounds = false;

            if (opts.outWidth > 1600 || opts.outHeight > 1600) {
                opts.inSampleSize = 4;
            } else if (opts.outWidth > 600 || opts.outHeight > 600) {
                opts.inSampleSize = 2;
            }

            final Bitmap bitmap = BitmapFactory.decodeFile(filename, opts);

            if (bitmap != null && bitmap.getWidth() > 0) {
                drawable = new BitmapDrawable(res, bitmap);
                addToCache(name, drawable);
            }
        }

        return drawable;
    }

    //! Add SVG object from storage
    @SuppressWarnings("unused")
    public final Drawable addSVGFile(String filename, String name) {
        Drawable drawable = mCache != null ? mCache.get(name) : null;

        if (drawable == null && name.endsWith(".svg") && USE_SVG) {/*
            try {
                final InputStream data = new FileInputStream(new File(filename));
                final Picture picture = SVG.getFromInputStream(data).renderToPicture();

                if (picture != null && picture.getWidth() > 0) {
                    drawable = new PictureDrawable(picture);
                    addToCache(name, drawable);
                }
                data.close();
            } catch (FileNotFoundException e) {
                Log.e(TAG, "File not found", e);
            } catch (IOException e) {
                Log.e(TAG, "SVG read fail", e);
            } catch (SVGParseException e) {
                Log.e(TAG, "Parse file fail", e);
            }*/
        }

        return drawable;
    }

    public static boolean copyFileTo(String srcpath, String destpath) {
        final File srcfile = new File(srcpath);
        return copyFile(srcfile, new File(destpath, srcfile.getName()));
    }

    public static boolean copyFileTo(String srcpath, String name, String destpath) {
        final File srcfile = new File(srcpath, name);
        return copyFile(srcfile, new File(destpath, name));
    }

    public static boolean copyFile(String srcpath, String destpath) {
        return copyFile(new File(srcpath), new File(destpath));
    }

    public static boolean copyFile(File srcfile, File destfile) {
        FileInputStream fis = null;
        FileOutputStream fos = null;
        boolean ret = false;

        if (srcfile.exists() && !destfile.exists()) {
            if (!destfile.getParentFile().exists()) {
                destfile.getParentFile().mkdir();
            }
            try {
                fis = new FileInputStream(srcfile);
                fos = new FileOutputStream(destfile);
                final byte[] buf = new byte[1024];
                int len;

                while ((len = fis.read(buf)) > 0) {
                    fos.write(buf, 0, len);
                }
                ret = true;
            } catch (Exception e) {
                Log.e(TAG, "IO exception", e);
            } finally {
                if (fis != null) {
                    try {
                        fis.close();
                    } catch (IOException e) {
                        Log.e(TAG, "close fis", e);
                    }
                }
                if (fos != null) {
                    try {
                        fos.close();
                    } catch (IOException e) {
                        Log.e(TAG, "close fos", e);
                    }
                }
            }
        }

        return ret;
    }
}
