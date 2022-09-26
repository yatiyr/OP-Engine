using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;


internal class ObjectYRotating : Entity
{

    private TransformComponent Transform;
    public float RotationSpeed = 0.5f;

    public void OnCreate()
    {
        Transform = GetComponent<TransformComponent>();
    }

    public void OnUpdate(float ts)
    {
        Vec3 rot = Transform.RotationEuler;
        rot.y += RotationSpeed * ts;
        Transform.RotationEuler = rot;
    }
}