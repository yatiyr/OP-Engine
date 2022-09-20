using System;
using System.Threading.Tasks;
using OP;

internal class NullScript : Entity
{

    public void OnCreate()
    {

    }

    public void OnUpdate(float ts)
    {

    }

    void OnCollision(uint otherEntityID, float x, float y, float z)
    {

    }

    void OnCollisionStarted(uint otherEntityID)
    {
        Console.WriteLine("NullScript Class On Collision Started");
    }

    void OnCollisionEnded(uint otherEntityID)
    {
        Console.WriteLine("NullScript Class On Collision Ended");
    }
}

