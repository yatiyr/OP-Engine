using System;

using Opium;

namespace Example
{
    internal class Script : Entity
    {
        public float publicField1 = 15.0f;

        public void OnCreate()
        {
            Console.WriteLine("Script.OnCreate has been invoked");
        }

        public void OnUpdate(float ts)
        {
            Mat4 transform = GetTransform();
            Vec3 translationVec = new Vec3(2.0f * ts, 2.0f * ts, 5.0f * ts);
            Mat4 translationMatrix = Mat4.Translate(translationVec);
            Mat4 newTransform = transform + translationMatrix;

            //Console.WriteLine(translationVec.x);
            newTransform.d_PrintMat4();
            SetTransform(newTransform);
        }
    }
}
