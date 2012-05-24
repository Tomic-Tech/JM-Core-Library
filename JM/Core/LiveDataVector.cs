using System;
using System.Runtime.InteropServices;

namespace JM.Core
{
    public class LiveDataVector : IDisposable
    {
        private IntPtr p;
        private bool disposed = false;

        [DllImport("JMCore", EntryPoint = "live_data_vector_new", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr New();

        [DllImport("JMCore", EntryPoint = "live_data_vector_dispose", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Dispose(IntPtr p);

        [DllImport("JMCore", EntryPoint = "live_data_vector_index", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr Index(IntPtr p, int index);

        public LiveDataVector()
        {
            p = New();
        }

        internal IntPtr UnmanagedPointer
        {
            get
            {
                return p;
            }
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

        ~LiveDataVector()
        {
            Dispose(true);
        }

        public LiveData Index(int index)
        {
            IntPtr ptr = Index(p, index);
            if (ptr != IntPtr.Zero)
            {
                return new LiveData(ptr);
            }
            return null;
        }
    }
}

