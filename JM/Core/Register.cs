using System;
using System.Runtime.InteropServices;
using System.Text;

namespace JM.Core
{
    public static class Register
    {
        [DllImport("JMCore", EntryPoint = "register_save", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Save([MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(UTF8Marshaler))]string reg);

        [DllImport("JMCore", EntryPoint = "register_query_id_code", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool QueryIDCode([Out, MarshalAs(UnmanagedType.LPStr)]StringBuilder text);

        [DllImport("JMCore", EntryPoint = "register_check")]
        public static extern bool Check();

        public static string QueryIDCode()
        {
            StringBuilder builder = new StringBuilder(100);
            if (QueryIDCode(builder))
                return builder.ToString();

            return null;
        }
    }
}

