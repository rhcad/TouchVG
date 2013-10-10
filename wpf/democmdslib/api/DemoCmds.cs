using System;
using democmds.core;
using System.Text;

namespace democmds.api
{
    public class DemoCmds
    {
        // mgView: WPFViewHelper.cmdViewHandle()
        public static int registerCmds(int mgView)
        {
            return DemoCmdsGate.registerCmds(mgView);
        }

        public static string getDimensions(int mgView, float[] vars)
        {
            Floats v = new Floats(vars.Length);
            Chars types = new Chars(vars.Length);
            int n = DemoCmdsGate.getDimensions(mgView, v, types);

            if (n > 0)
            {
                StringBuilder buf = new StringBuilder(n);
                for (int i = 0; i < n; i++)
                {
                    buf.Append(v.get(i));
                    vars[i] = v.get(i);
                }
                return buf.ToString();
            }
            return "";
        }
    }
}
