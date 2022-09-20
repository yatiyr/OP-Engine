using System.Runtime.InteropServices;

namespace OP
{

    [StructLayout(LayoutKind.Explicit)]
    public struct Vec2
    {
        [FieldOffset(0)] public float x;
        [FieldOffset(4)] public float y;

        public Vec2(float value)
        {
            x = y = value;
        }

        public Vec2(float x, float y)
        {
            this.x = x;
            this.y = y;
        }
    }
}
