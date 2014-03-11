package rhcad.touchvg.view.internal;

import android.util.Log;

public class LogHelper {
    private static final String TAG = "vgstack";
    private static int level;

    public LogHelper() {
        Log.d(TAG, addHead(getStackTrace(2).insert(0, "Enter method")).toString());
        level++;
    }

    public LogHelper(String info) {
        final StringBuffer buf = new StringBuffer("Enter method (").append(info).append(") ");
        Log.d(TAG, addHead(getStackTrace(buf, 2)).toString());
        level++;
    }

    public void r() {
        level--;
        Log.d(TAG, addHead(getStackTrace(1).insert(0, "Return")).toString());
    }

    public boolean r(boolean ret) {
        level--;
        Log.d(TAG, addHead(getStackTrace(1).insert(0, "Return " + ret)).toString());
        return ret;
    }

    public int r(int ret) {
        level--;
        Log.d(TAG, addHead(getStackTrace(1).insert(0, "Return " + ret)).toString());
        return ret;
    }

    public String r(String ret) {
        level--;
        Log.d(TAG, addHead(getStackTrace(1).insert(0, "Return")).toString());
        return ret;
    }

    private StringBuffer addHead(StringBuffer buf) {
        for (int i = 0; i < level; i++)
            buf.insert(0, "  ");
        return buf;
    }

    public static StringBuffer getStackTrace(int maxCount) {
        final StringBuffer buf = new StringBuffer(maxCount > 1 ? " " : " in ");
        return getStackTrace(buf, maxCount);
    }

    public static StringBuffer getStackTrace(StringBuffer buf, int maxCount) {
        final StackTraceElement[] ste = Thread.currentThread().getStackTrace();
        final String thisClassName = LogHelper.class.getName();
        int i = 0;

        while (!ste[i].getClassName().equals(thisClassName))
            i++;
        while (ste[i].getClassName().equals(thisClassName))
            i++;
        for (int n = 0; n < maxCount && i < ste.length; n++, i++) {
            if (n > 0) {
                buf.append(", ");
            }
            final String className = ste[i].getClassName();
            int pos = Math.max(className.lastIndexOf('.'), className.lastIndexOf('$'));
            buf.append(className.substring(pos + 1)).append('.').append(ste[i].getMethodName());
            buf.append(" L").append(ste[i].getLineNumber());
        }

        return buf;
    }
}
