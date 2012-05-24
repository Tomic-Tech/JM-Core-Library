using System;
using System.Runtime.InteropServices;
using System.Text;

namespace JM.Core
{
    public static class SysDB
    {

        [DllImport("JMCore", EntryPoint="sys_db_get_text", CallingConvention = CallingConvention.Cdecl)]
        private static extern void GetText([MarshalAs(UnmanagedType.LPStr)]string name, 
                                           [Out, MarshalAs(UnmanagedType.LPWStr)]StringBuilder text);

        public static string Text(string name)
        {
            StringBuilder builder = new StringBuilder(100);
            GetText(name, builder);
            return builder.ToString();
        }
    }
}

