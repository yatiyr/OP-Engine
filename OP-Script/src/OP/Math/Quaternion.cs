using System.Runtime.InteropServices;
using System;

namespace OP
{

    [StructLayout(LayoutKind.Explicit)]
    public struct Quaternion
    {
        [FieldOffset(0)]  public float w;
        [FieldOffset(4)]  public float x;
        [FieldOffset(8)]  public float y;
        [FieldOffset(12)] public float z;

        public Quaternion(Quaternion q)
        {
            w = q.w;
            x = q.x;
            y = q.y;
            z = q.z;
        }

        public Quaternion(float w, float x, float y, float z)
        {
            this.w = w;
            this.x = x;
            this.y = y;
            this.z = z;
        }

        public static Quaternion operator +(Quaternion q1, Quaternion q2)
        {
            Quaternion result = new Quaternion();
            result.w = q1.w + q2.w;
            result.x = q1.x + q2.x;
            result.y = q1.y + q2.y;
            result.z = q1.z + q2.z;
            return result;
        }

        public static Quaternion operator -(Quaternion q1, Quaternion q2)
        {
            Quaternion result = new Quaternion();
            result.w = q1.w - q2.w;
            result.x = q1.x - q2.x;
            result.y = q1.y - q2.y;
            result.z = q1.z - q2.z;
            return result;
        }

        public static Quaternion operator *(Quaternion q1, Quaternion q2)
        {
            Quaternion result = new Quaternion();
            result.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
            result.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
            result.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
            result.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
            return result;
        }

        public static Quaternion Conjugate(Quaternion q)
        {
            Quaternion result = new Quaternion();
            result.w =  q.w;
            result.x = -q.x;
            result.y = -q.y;
            result.z = -q.z;
            return result;
        }

        public static float Magnitude(Quaternion q)
        {
            return (float)Math.Sqrt(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
        }

        public void Normalize()
        {
            float norm = (float)Math.Sqrt(w * w + x * x + y * y + z * z);
            w = w / norm;
            x = x / norm;
            y = y / norm;
            z = z / norm;
        }

        public static Vec3 RotatePoint(Vec3 point, Quaternion q)
        {
            Vec3 result = new Vec3();
            Quaternion calculationQuat = new Quaternion();
            calculationQuat.w = 0.0f;
            calculationQuat.x = point.x;
            calculationQuat.y = point.y;
            calculationQuat.z = point.z;
            Quaternion qConj = Conjugate(q);

            calculationQuat = q * calculationQuat * qConj;

            result.x = calculationQuat.x;
            result.y = calculationQuat.y;
            result.z = calculationQuat.z;
            return result;
        }

        public static Quaternion AxisAngleToQuat(float angle, Vec3 axis)
        {
            Quaternion result = new Quaternion();
            float sin_half_angle = (float)Math.Sin(angle / 2);
            float cos_half_angle = (float)Math.Cos(angle / 2);

            result.w = cos_half_angle;
            result.x = axis.x * sin_half_angle;
            result.y = axis.y * sin_half_angle;
            result.z = axis.z * sin_half_angle;
            return result;
        }

        public static Quaternion QuaternionFromEuler(float yaw, float pitch, float roll)
        {
            Quaternion result = new Quaternion();

            float c1 = (float)Math.Cos(yaw / 2);
            float s1 = (float)Math.Sin(yaw / 2);

            float c2 = (float)Math.Cos(pitch / 2);
            float s2 = (float)Math.Sin(pitch / 2);

            float c3 = (float)Math.Cos(roll / 2);
            float s3 = (float)Math.Sin(roll / 2);

            float c1c2 = c1 * c2;
            float s1s2 = s1 * s2;

            result.w = c1c2 * c3 - s1s2 * s3;
            result.x = c1c2 * s3 + s1s2 * c3;
            result.y = s1 * c2 * c3 + c1 * s2 * s3;
            result.z = c1 * s2 * c3 - s1 * c2 * s3;

            return result;
        }
    }
}
