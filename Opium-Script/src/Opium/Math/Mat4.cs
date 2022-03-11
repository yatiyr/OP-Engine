using System;
using System.Runtime.InteropServices;


namespace Opium
{


    // Column major structure for matrices
    [StructLayout(LayoutKind.Explicit)]
    public struct Mat4
    {
        // First Column
        [FieldOffset( 0)] public float D00;
        [FieldOffset( 4)] public float D10;
        [FieldOffset( 8)] public float D20;
        [FieldOffset(12)] public float D30;

        // Second Column
        [FieldOffset(16)] public float D01;
        [FieldOffset(20)] public float D11;
        [FieldOffset(24)] public float D21;
        [FieldOffset(28)] public float D31;

        // Third Column
        [FieldOffset(32)] public float D02;
        [FieldOffset(36)] public float D12;
        [FieldOffset(40)] public float D22;
        [FieldOffset(44)] public float D32;

        // Fourth Column
        [FieldOffset(48)] public float D03;
        [FieldOffset(52)] public float D13;
        [FieldOffset(56)] public float D23;
        [FieldOffset(60)] public float D33;

        public Mat4(float value)
        {
            D00 = value;
            D10 = 0.0f;
            D20 = 0.0f;
            D30 = 0.0f;

            D01 = 0.0f;
            D11 = value;
            D21 = 0.0f;
            D31 = 0.0f;

            D02 = 0.0f;
            D12 = 0.0f;
            D22 = value;
            D32 = 0.0f;

            D03 = 0.0f;
            D13 = 0.0f;
            D23 = 0.0f;
            D33 = value;
        }

        public Vec3 Translation
        {
            get { return new Vec3(D03, D13, D23); }
            set { D03 = value.x; D13 = value.y; D23 = value.z; }
        }

        public static Mat4 Translate(Vec3 translation)
        {
            Mat4 result = new Mat4(1.0f);
            result.D03 = translation.x;
            result.D13 = translation.y;
            result.D23 = translation.z;
            return result;
        }

        public static Mat4 Scale(Vec3 scale)
        {
            Mat4 result = new Mat4(1.0f);
            result.D00 = scale.x;
            result.D11 = scale.y;
            result.D22 = scale.z;
            return result;
        }

        public static Mat4 Scale(float scale)
        {
            Mat4 result = new Mat4(1.0f);
            result.D00 = scale;
            result.D11 = scale;
            result.D22 = scale;
            return result;
        }

        public static Mat4 operator +(Mat4 a, Mat4 b)
        {
            Mat4 result = new Mat4(1.0f);

            result.D00 = a.D00 + b.D00;
            result.D01 = a.D01 + b.D01;
            result.D02 = a.D02 + b.D02;
            result.D03 = a.D03 + b.D03;
            result.D10 = a.D10 + b.D10;
            result.D11 = a.D11 + b.D11;
            result.D12 = a.D12 + b.D12;
            result.D13 = a.D13 + b.D13;
            result.D20 = a.D20 + b.D20;
            result.D21 = a.D21 + b.D21;
            result.D22 = a.D22 + b.D22;
            result.D23 = a.D23 + b.D23;
            result.D30 = a.D30 + b.D30;
            result.D31 = a.D31 + b.D31;
            result.D32 = a.D32 + b.D32;
            result.D33 = a.D33 + b.D33;

            return result;
        }

        public void d_PrintMat4()
        {
            Console.WriteLine("{0:0.00} {1:0.00} {2:0.00} {3:0.00}", D00, D01, D02, D03);
            Console.WriteLine("{0:0.00} {1:0.00} {2:0.00} {3:0.00}", D10, D11, D12, D13);
            Console.WriteLine("{0:0.00} {1:0.00} {2:0.00} {3:0.00}", D20, D21, D22, D23);
            Console.WriteLine("{0:0.00} {1:0.00} {2:0.00} {3:0.00}", D30, D31, D32, D33);
        }
    }
}
