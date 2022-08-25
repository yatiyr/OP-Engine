using System.Runtime.InteropServices;

namespace OP
{

    [StructLayout(LayoutKind.Explicit)]
    public struct Vec4
    {
        [FieldOffset(0)]  public float x;
        [FieldOffset(4)]  public float y;
        [FieldOffset(8)]  public float z;
        [FieldOffset(12)] public float w;

        public Vec4(float value)
        {
            x = y = z = w = value;
        }

        public Vec4(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }
    }
}