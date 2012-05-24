using System;

namespace JM.Diag
{
    public class Mikuni : IProtocol
    {
        public const byte HEAD_FORMAT = 0x48;

        public Mikuni()
        {
        }

        public byte[] Pack(byte[] data, int offset, int count)
        {
            byte[] result = new byte[count + 3];
            result[0] = HEAD_FORMAT;
            Array.Copy(data, offset, result, 1, count);
            result[count + 1] = 0x0D;
            result[count + 2] = 0x0A;

            return result;
        }

        public byte[] Unpack(byte[] data, int offset, int count)
        {
            byte[] result = new byte[count - 3];
            Array.Copy(data, offset + 1, result, 0, count - 3);

            return result;
        }
    }
}