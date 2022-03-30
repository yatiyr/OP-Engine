using System.Runtime.InteropServices;

namespace OP
{

    [StructLayout(LayoutKind.Explicit)]
    public struct Vec3
    {
        [FieldOffset(0)] public float x;
        [FieldOffset(4)] public float y;
        [FieldOffset(8)] public float z;

        public Vec3(float value)
        {
            x = y = z = value;
        }

        public Vec3(float x, float y, float z)
        {
            this.x = x;
            this.y = y;
            this.z = z;
        }
    }
}
