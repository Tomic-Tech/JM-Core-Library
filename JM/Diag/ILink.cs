using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JM.Diag
{
    interface ILink<P>
    {
        int SendOneFrame(byte[] data, int offset, int count);
        int SendFrames(byte[] data, int offset, int count);
        byte[] ReadOneFrame();
        byte[] ReadFrames();
        byte[] SendAndRecv(byte[] data, int offset, int count);
        void StartKeepLink(bool run);
        void SetKeepLink(byte[] data, int offset, int count);
        void SetTimeout(int txB2B, int rxB2B, int txF2F, int total);
    }
}
