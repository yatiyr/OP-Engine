using System;
using System.Threading.Tasks;
using OP;


internal class Script : Entity
{
    public float publicField1 = 15.0f;
    public int publicField2 = 0;

    private TransformComponent tC;
    private Physics3DMaterial pC;

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
        tC = GetComponent<TransformComponent>();
        pC = GetComponent<Physics3DMaterial>();
    }

    public void OnCollisionStarted(Entity otherEntity)
    {
        Console.WriteLine("CollisionStarted");
    }

    public void OnCollisionEnded(Entity otherEntity)
    {
        Console.WriteLine("CollisionEnded");
    }

    public void OnCollision(Entity otherEntity, Vec3 collisionPoint)
    {
        Console.WriteLine("Collision");
    }


    public void OnUpdate(float ts)
    {
        Mat4 transform = GetTransform();
        Vec3 translationVec = new Vec3(2.0f * ts, 2.0f * ts, 5.0f * ts);
        Mat4 translationMatrix = Mat4.Translate(translationVec);
        Mat4 newTransform = transform + translationMatrix;

        if(OP.Input.IsKeyPressed(KeyCode.Q))
        {
            Scene.Exposure += 0.5f * ts;
        }

        if(OP.Input.IsKeyPressed(KeyCode.E))
        {
            Scene.Exposure -= 0.5f * ts;
        }

        if(OP.Input.IsKeyPressed(KeyCode.Z))
        {
            Console.WriteLine(Scene.Skybox);
        }

        if(OP.Input.IsKeyPressed(KeyCode.W))
        {
            pC.LinearVelocity = new Vec3(0.0f, 1.0f, 0.0f);
        }
        // tC.Translation = new Vec3(5.0f, 5.0f, 5.0f);
        // Console.WriteLine(tC.Translation.x);
        //SetTransform(newTransform);
    }
}

