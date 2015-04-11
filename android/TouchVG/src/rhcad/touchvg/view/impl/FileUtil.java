// Copyright (c) 2012-2015, https://github.com/rhcad/vgandroid, BSD license

package rhcad.touchvg.view.impl;

import java.io.File;

import android.util.Log;

public class FileUtil {
    public static final String TAG = "touchvg";

    private FileUtil() {
    }

    //! 返回指定后缀名的文件名
    public static String addExtension(String filename, String ext) {
        if (filename != null && !filename.endsWith(ext) && !filename.endsWith(".json")) {
            return filename.substring(0, filename.lastIndexOf('.')) + ext;
        }
        return filename;
    }

    //! 创建指定的文件的上一级文件夹，如果自身是文件夹则也创建
    public static boolean createDirectory(String filename, boolean isDirectory) {
        final File file = new File(filename);
        final File pf = file.getParentFile();

        if (!pf.exists() && !pf.mkdir()) {
            Log.e(TAG, "Fail to create parent folder: " + pf.getPath());
            return false;
        }
        if (isDirectory && !file.exists() && !file.mkdir()) {
            Log.e(TAG, "Fail to create folder: " + filename);
            return false;
        }
        return true;
    }

    //! 删除一个文件夹的所有内容
    public static boolean deleteDirectory(File path) {
        if (path.exists()) {
            final File[] files = path.listFiles();
            if (files != null && !deleteFiles(files)) {
                return false;
            }
        }
        return !path.exists() || path.delete();
    }

    private static boolean deleteFiles(File[] files) {
        for (File f : files) {
            if (f.isDirectory()) {
                if (!deleteDirectory(f)) {
                    Log.e(TAG, "Fail to delete folder: " + f.getPath());
                    return false;
                }
            } else {
                if (!f.delete()) {
                    Log.e(TAG, "Fail to delete file: " + f.getPath());
                    return false;
                }
            }
        }
        return true;
    }
}
