using System;
using System.Runtime.InteropServices;

namespace JM.Core
{
    public class LiveData
    {
        private IntPtr p;

        public LiveData(IntPtr p)
        {
            this.p = p;
        }

        [DllImport("JMCore", EntryPoint = "live_data_short_name", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]
        private static extern string GetShortName(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_content", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]
        private static extern string GetContent(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_unit", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]
        private static extern string GetUnit(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_default_value", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]
        private static extern string GetDefaultValue(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_value", CallingConvention = CallingConvention.Cdecl)]
        [return:MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]
        private static extern string GetValue(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_cmd_id", CallingConvention = CallingConvention.Cdecl)]
        private static extern int GetCmdID(IntPtr p);
		
        public string ShortName
        {
            get
            {
                return  GetShortName(p);
            }
        }
		
        public string Content
        {
            get
            {
                return GetContent(p);
            }
        }
		
		public string Unit
		{
			get
			{
				return GetUnit(p);
			}
		}
		
		public string DefaultValue
		{
			get
			{
				return GetDefaultValue(p);
			}
		}
		
		public string Value
		{
			get
			{
				return GetValue(p);
			}
		}
		
		public int CmdID
		{
			get
			{
				return GetCmdID(p);
			}
		}
		
    }
}

