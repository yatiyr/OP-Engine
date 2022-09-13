using System.Runtime.InteropServices;
using System;

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

        public Vec3(Vec3 val)
        {
            this.x = val.x;
            this.y = val.y;
            this.z = val.z;
        }

        public static Vec3 operator -(Vec3 val1, Vec3 val2)
        {
            return new Vec3(val1.x - val2.x, val1.y - val2.y, val1.z - val2.z);
        }

        public static Vec3 operator +(Vec3 val1, Vec3 val2)
        {
            return new Vec3(val1.x + val2.x, val1.y + val2.y, val1.z + val2.z);
        }

        public static Vec3 operator *(Vec3 val1, Vec3 val2)
        {
            return new Vec3(val1.x * val2.x, val1.y * val2.y, val1.z * val2.z);
        }

        public static Vec3 operator /(Vec3 val1, Vec3 val2)
        {
            return new Vec3(val1.x / val2.x, val1.y / val2.y, val1.z / val2.z);
        }

        public static Vec3 operator *(Vec3 val1, float val2)
        {
            return new Vec3(val1.x * val2, val1.y * val2, val1.z * val2);
        }

        public static Vec3 operator /(Vec3 val1, float val2)
        {
            return new Vec3(val1.x / val2, val1.y / val2, val1.z / val2);
        }

        public static float Dot(Vec3 val1, Vec3 val2)
        {
            return val1.x * val2.x + val1.y * val2.y + val1.z * val2.z;
        }

        public static Vec3 Cross(Vec3 val1, Vec3 val2)
        {
            return new Vec3(val1.y * val2.z - val2.y * val1.z, val2.x * val1.z - val1.x * val2.z, val1.x * val2.y - val2.x * val1.y);
        }

        public static float Length(Vec3 val)
        {
            float length = val.x * val.x + val.y * val.y + val.z * val.z;
            return (float)Math.Sqrt(length);
        }

        public static float LengthSquared(Vec3 val)
        {
            return val.x * val.x + val.y * val.y + val.z * val.z;
        }

        public static float Distance(Vec3 val1, Vec3 val2)
        {
            float distanceSquared = (val1.x - val2.x) * (val1.x - val2.x) +
                                    (val1.y - val2.y) * (val1.y - val2.y) +
                                    (val1.z - val2.z) * (val1.z - val2.z);
            return (float)Math.Sqrt(distanceSquared);
        }

        public static float DistanceSquared(Vec3 val1, Vec3 val2)
        {
            float distanceSquared = (val1.x - val2.x) * (val1.x - val2.x) + 
                                    (val1.y - val2.y) * (val1.y - val2.y) +
                                    (val1.z - val2.z) * (val1.z - val2.z);

            return distanceSquared;
        }

        public static Vec3 Mix(Vec3 val1, Vec3 val2, float t)
        {
            if (t > 1)
                return new Vec3(val2.x, val2.y, val2.z);

            float x = val1.x * (1 - t) + val2.x * t;
            float y = val1.y * (1 - t) + val2.y * t;
            float z = val1.z * (1 - t) + val2.z * t;

            return new Vec3(x, y, z);
        }

        public Vec3 Normalize()
        {
            float length = Length(this);

            if(length != 0)
            {
                this.x /= length;
                this.y /= length;
                this.z /= length;
            }

            return this;
        }

        public static bool operator ==(Vec3 val1, Vec3 val2)
        {
            return val1.x == val2.x && val1.y == val2.y && val1.z == val2.z;
        }

        public static bool operator !=(Vec3 val1, Vec3 val2)
        {
            return val1.x != val2.x || val1.y != val2.y || val1.z != val2.z;
        }
    }
}
