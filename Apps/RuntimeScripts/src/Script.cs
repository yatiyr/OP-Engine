using System;
using System.Threading.Tasks;
using OP;


internal class Script : Entity
{
    public float publicField1 = 15.0f;
    public int publicField2 = 0;
    public int publicField3 = 0;
    public int publicField4 = 0;

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

    public void ImScript()
    {
        Console.WriteLine("ImScript");
    }

    public void OnCollisionStarted(uint otherEntityID)
    {
        TagComponent tC = new TagComponent();
        tC.Entity = new Entity();
        tC.Entity.EntityID = otherEntityID;
        tC.Entity.SceneID = SceneID;

        Console.WriteLine("Collision started with {0}", tC.Tag);
    }

    public void OnCollisionEnded(uint otherEntityID)
    {
        TagComponent tC = new TagComponent();
        tC.Entity = new Entity();
        tC.Entity.EntityID = otherEntityID;
        tC.Entity.SceneID = SceneID;

        Console.WriteLine("Collision ended with {0}", tC.Tag);
    }

    public void OnUpdate(float ts)
    {

    }
}

