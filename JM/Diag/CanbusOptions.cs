namespace JM.Diag
{
    public struct CanbusOptions
    {
        public int TargetID;
        public CanbusBaud Baud;
        public CanbusIDMode IDMode;
        public CanbusFilterMask FilterMask;
        public CanbusFrameType FrameType;
        public int HighPin;
        public int LowPin;
        public int[] IDVector;
    }
}