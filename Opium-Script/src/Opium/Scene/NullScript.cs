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

    void OnCollision_Native(uint EntityID, uint SceneID, float x, float y, float z)
    {
        Entity oE = new Entity();
        oE.EntityID = EntityID;
        oE.SceneID = SceneID;

        Vec3 cP;
        cP.x = x;
        cP.y = y;
        cP.z = z;
        OnCollision(oE, cP);
    }

    void OnCollisionStarted_Native(uint EntityID, uint SceneID)
    {
        Entity oE = new Entity();
        oE.EntityID = EntityID;
        oE.SceneID = SceneID;

        OnCollisionStarted(oE);
    }

    void OnCollisionEnded_Native(uint EntityID, uint SceneID)
    {
        Entity oE = new Entity();
        oE.EntityID = EntityID;
        oE.SceneID = SceneID;
        OnCollisionEnded(oE);
    }

    void OnCollision(Entity otherEntity, Vec3 collisionPoint)
    {
        string res = String.Format("EntityID = {0}, SceneID = {1}, CollisionPoint=[{2},{3},{4}]",
            otherEntity.EntityID, otherEntity.SceneID,
            collisionPoint.x, collisionPoint.y, collisionPoint.z);
        Console.WriteLine(res);
    }

    void OnCollisionStarted(Entity otherEntity)
    {
        Console.WriteLine("OnCollisionStarted");
    }

    void OnCollisionEnded(Entity otherEntity)
    {
        Console.WriteLine("OnCollisionEnded");
    }
}

