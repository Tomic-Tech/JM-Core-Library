using System;
using System.Collections.Generic;

namespace JM.Diag
{
    public class Stream
    {
        private object lockForQueue;
        private Queue<byte> queue;

        public Stream()
        {
            lockForQueue = new object();
            queue = new Queue<byte>();
        }

        public int BytesAvailable()
        {
            lock (lockForQueue)
            {
                return queue.Count;
            }
        }

        public void Clear()
        {
            lock (lockForQueue)
            {
                queue.Clear();
            }
        }

        byte[] Read(int count)
        {
            lock (lockForQueue)
            {
                if (queue.Count == 0)
                {
                    return null;
                }

                int realCount = count > queue.Count ? queue.Count : count;
                byte[] result = new byte[realCount];

                for (int i = 0; i < realCount; i++)
                {
                    result[i] = queue.Dequeue();
                }
                return result;
            }
        }

        public int Write(byte[] data, int offset, int count)
        {
            lock (lockForQueue)
            {
                for (int i = 0; i < count; i++)
                {
                    queue.Enqueue(data[offset + i]);
                }
            }
            return count;
        }
    }
}