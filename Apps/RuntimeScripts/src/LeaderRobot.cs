using System;
using System.Threading.Tasks;
using OP;


internal class LeaderRobot : Entity
{
    public float speed = 1.0f;
    public Vec3 direction = new Vec3();

    private TransformComponent tC;
    private Physics3DMaterial pC;

    public Vec3 waypoint1 = new Vec3(10.0f, 10.0f, 10.0f);
    public Vec3 waypoint2 = new Vec3(-10.0f, -10.0f, -10.0f);

    Script sensor;

    private Vec3 target;

    public void OnCreate()
    {
        Console.WriteLine("Leader Robot Has been Created");
        tC = GetComponent<TransformComponent>();
        pC = GetComponent<Physics3DMaterial>();
        target = waypoint1;

        Entity child = GetChildEntity("Sensor");

        Console.WriteLine(child.GetType().Name);
        sensor = (Script)child;
        sensor.ImScript();

    }


    public void OnUpdate(float ts)
    {

        Vec3 currentPosition = tC.Translation;

        float distanceToTarget = Vec3.Distance(currentPosition, target);
        if (distanceToTarget < 1.0)
        {
            if(target == waypoint1)
            {
                target = waypoint2;
            }
            else
            {
                target = waypoint1;
            }
        }

        Vec3 d = target - currentPosition;
        d.Normalize();
        direction = d;

        pC.LinearVelocity = direction * speed;

    }
}

