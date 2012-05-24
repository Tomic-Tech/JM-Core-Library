using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace JM.Core
{
    public static class Utils
    {
        public static byte HighByte<T>(T x) where T : IConvertible
        {
            ushort integer = System.Convert.ToUInt16(x);
            return (byte)((integer >> 8) & 0xFF);
        }

        public static byte LowByte<T>(T x) where T : IConvertible
        {
            ushort integer = System.Convert.ToUInt16(x);
            return (byte)(integer & 0xFF);
        }

        public static ushort HighWord<T>(T x) where T : IConvertible
        {
            uint integer = System.Convert.ToUInt32(x);
            return (ushort)((integer >> 16) & 0xFFFF);
        }

        public static ushort LowWord<T>(T x) where T : IConvertible
        {
            uint integer = System.Convert.ToUInt32(x);
            return (ushort)(integer & 0xFFFF);
        }
    }
}
