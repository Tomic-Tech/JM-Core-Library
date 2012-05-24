using System;
using System.Text;
using System.Runtime.InteropServices;

namespace JM.Core
{
    public class VehicleDB : IDisposable
    {
        IntPtr p;
        bool disposed = false;

        [DllImport("JMCore", EntryPoint = "database_new", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr New([MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]string filePath);

        [DllImport("JMCore", EntryPoint = "database_dispose", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Dispose(IntPtr p);

        [DllImport("JMCore", EntryPoint = "database_set_tc_catalog", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetTCCatalog(IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string text);

        [DllImport("JMCore", EntryPoint = "database_set_ld_catalog", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetLDCatalog(IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string text);

        [DllImport("JMCore", EntryPoint = "database_set_cmd_catalog", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool SetCMDCatalog(IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string text);

        [DllImport("JMCore", EntryPoint = "database_get_text", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetText(IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string name, [Out, MarshalAs(UnmanagedType.LPWStr)]StringBuilder text);

        [DllImport("JMCore", EntryPoint = "database_get_trouble_code", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetTroubleCode(IntPtr p, [MarshalAs(UnmanagedType.LPWStr)]string code, [Out, MarshalAs(UnmanagedType.LPWStr)]StringBuilder text);

        [DllImport("JMCore", EntryPoint = "database_get_live_data", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetLiveData(IntPtr p, IntPtr vec);

        [DllImport("JMCore", EntryPoint = "database_get_command", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetCommand(IntPtr p, [MarshalAs(UnmanagedType.LPStr)]string name, byte[]buffer, out UInt32 count);

        [DllImport("JMCore", EntryPoint = "database_get_command_by_id", CallingConvention = CallingConvention.Cdecl)]
        private static extern bool GetCommand(IntPtr p, Int32 id, byte[]buffer, out UInt32 count);

        public VehicleDB(string filePath)
        {
            p = New(filePath);
        }

        void IDisposable.Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool isDisposing)
        {
            if (!disposed)
            {
                if (isDisposing)
                {
                    Dispose(p);
                    disposed = true;
                }
            }
        }

        public void Close()
        {
            ((IDisposable)this).Dispose();
        }

        ~VehicleDB()
        {
            Dispose(false);
        }

        public string TCCatalog
        {
            set
            {
                SetTCCatalog(p, value);
            }
        }

        public string LDCatalog
        {
            set
            {
                SetLDCatalog(p, value);
            }
        }

        public string CMDCatalog
        {
            set
            {
                SetCMDCatalog(p, value);
            }
        }

        public string Text(string name)
        {
            StringBuilder builder = new StringBuilder(1024);
            if (GetText(p, name, builder))
            {
                return builder.ToString();
            }

            return null;
        }

        public string TroubleCode(string code)
        {
            StringBuilder builder = new StringBuilder(100);
            if (GetTroubleCode(p, code, builder))
            {
                return builder.ToString();
            }

            return null;
        }

        public LiveDataVector LiveData()
        {
            LiveDataVector vec = new LiveDataVector();
            if (!GetLiveData(p, vec.UnmanagedPointer))
            {
                vec.Close();
                return null;
            }

            return vec;
        }

        public byte[] Command(string name)
        {
            byte[] buffer = new byte[1024];
            UInt32 count;
            if (GetCommand(p, name, buffer, out count))
            {
                byte[] result = new byte[count];
                Array.Copy(buffer, result, count);
                return result;
            }

            return null;
        }

        public byte[] Command(int id)
        {
            byte[] buffer = new byte[1024];
            UInt32 count;
            if (GetCommand(p, id, buffer, out count))
            {
                byte[] result = new byte[count];
                Array.Copy(buffer, result, count);
                return result;
            }

            return null;
        }

    }
}

