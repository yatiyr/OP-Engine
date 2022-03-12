using System;
using System.Threading.Tasks;
using Opium;

namespace Example
{
    internal class Script : Entity
    {
        public float publicField1 = 15.0f;


        public async Task func()
        {
            await Task.Delay(10000);
            Console.WriteLine("Func executed!");
        }

        public void OnCreate()
        {
            Console.WriteLine("Script.OnCreate has been invoked");
            func();
            Console.WriteLine("Life moves on and on and on!");
        }

        public void OnUpdate(float ts)
        {
            Mat4 transform = GetTransform();
            Vec3 translationVec = new Vec3(2.0f * ts, 2.0f * ts, 5.0f * ts);
            Mat4 translationMatrix = Mat4.Translate(translationVec);
            Mat4 newTransform = transform + translationMatrix;

            if(Input.IsKeyPressed(KeyCode.Enter))
            {
                Console.WriteLine("Entera basildi.");
            }
            
            //SetTransform(newTransform);
        }
    }
}
