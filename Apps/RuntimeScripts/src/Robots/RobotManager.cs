using System;
using System.Threading.Tasks;
using OP;


internal class RobotManager : Entity
{

    // These variables are going to be used by
    // child robots and all of them are going to be
    // between 0.0 - 1.0f
    public float Cohesion   = 1.0f;
    public float Alignment  = 1.0f;
    public float Separation = 1.0f;

    public float ObstacleAvoidance = 1.0f;

    public float SensorAngle = 280.0f;

    public float SeparationDistance = 0.5f;
    public float ObstacleAvoidanceDistance = 0.5f;

    public float Speed = 1.0f;
    public float VelocityChangeSpeed = 5.0f;

    public float Xmax = 10.0f;
    public float Xmin = -10.0f;

    public float Ymax = 10.0f;
    public float Ymin = -10.0f;

    public float Zmax = 10.0f;
    public float Zmin = -10.0f;
}

