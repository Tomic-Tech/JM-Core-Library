using System;
using System.Runtime.InteropServices;

namespace JM.Core
{
    public class MustCallFirst : IDisposable
    {
        static MustCallFirst instance;
        private string rootPath;
        private bool disposed = false;

        static MustCallFirst()
        {
            instance = new MustCallFirst();
        }

        private MustCallFirst()
        {
        }

        public static MustCallFirst Instance
        {
            get
            {
                return instance;
            }
        }

        public void Init()
        {
            rootPath = Environment.GetCommandLineArgs() [1];
            RegisterInit(rootPath);
            SysDBInit(rootPath);
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
                    RegisterDispose();
                    SysDBDispose();
                    disposed = true;
                }
            }
        }

        ~MustCallFirst()
        {
            Dispose(true);
        }

        [DllImport("JMCore", EntryPoint = "sys_db_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SysDBInit([MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]string path);

        [DllImport("JMCore", EntryPoint = "sys_db_dispose", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SysDBDispose();

        [DllImport("JMCore", EntryPoint = "register_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern void RegisterInit([MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef=typeof(UTF8Marshaler))]string path);

        [DllImport("JMCore", EntryPoint = "register_dispose", CallingConvention = CallingConvention.Cdecl)]
        private static extern void RegisterDispose();
    }
}

