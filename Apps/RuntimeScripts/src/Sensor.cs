using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;


internal class Sensor : Entity
{
    public float publicField1 = 15.0f;
    public int publicField2 = 0;
    public int publicField3 = 0;
    public int publicField4 = 0;

    private TransformComponent tC;
    private Physics3DMaterial pC;

    private List<SceneBody> Obstacles;
    private List<SceneBody> Robots;

    private Entity Parent;


    public List<SceneBody> GetObstacles()
    {
        return Obstacles;
    }

    public List<SceneBody> GetRobots()
    {
        return Robots;
    }

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
        Obstacles = new List<SceneBody>();
        Robots = new List<SceneBody>();
        Parent = GetParentEntity();
    }

    public void OnCollisionStarted(uint otherEntityID)
    {
        Entity entity = new Entity();
        entity.EntityID = otherEntityID;
        entity.SceneID = SceneID;

        TagComponent tC = new TagComponent();
        tC.Entity = entity;
        Physics3DMaterial body = new Physics3DMaterial();
        body.Entity = entity;
        TransformComponent transC = new TransformComponent();
        transC.Entity = entity;

        SceneBody sB = new SceneBody();
        sB.EntityID = otherEntityID;
        sB.TypeEnum = tC.TypeEnum;
        sB.Body = body;
        sB.BodyTransform = transC;

        if(sB.TypeEnum == (int)TypeEnum.Obstacle)
        {
            Console.WriteLine("Added Obstacle to collision list");
            Obstacles.Add(sB);
        }
        else if(sB.TypeEnum == (int)TypeEnum.Robot && Parent.EntityID != otherEntityID)
        {
            Console.WriteLine("Added robot to collision list");
            Robots.Add(sB);
        }
            
    }

    public void OnCollisionEnded(uint otherEntityID)
    {
        Entity entity = new Entity();
        entity.EntityID = otherEntityID;
        entity.SceneID = SceneID;

        TagComponent tC = new TagComponent();
        tC.Entity = entity;

        int typeEnum = tC.TypeEnum;

        if (typeEnum == (int)TypeEnum.Obstacle)
        {
            Console.WriteLine("Removed Obstacle from collision list");
            Obstacles.RemoveAll(s => s.EntityID == otherEntityID);
        }
        else if (typeEnum == (int)TypeEnum.Robot)
        {
            Console.WriteLine("Removed Robot from collision list");
            Robots.RemoveAll(s => s.EntityID == otherEntityID);
        }

    }

    public void OnUpdate(float ts)
    {

    }
}

