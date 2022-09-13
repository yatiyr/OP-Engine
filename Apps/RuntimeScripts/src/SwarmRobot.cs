using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;


internal class SwarmRobot : Entity
{
    public float speed = 1.0f;
    public Vec3 direction = new Vec3();

    private TransformComponent tC;
    private Physics3DMaterial pC;


    Sensor sensor;


    public float separationDistance = 0.2f;
    public float obstacleDistance = 0.4f;

    public void OnCreate()
    {
        Console.WriteLine("Leader Robot Has been Created");
        tC = GetComponent<TransformComponent>();
        pC = GetComponent<Physics3DMaterial>();

        Entity child = GetChildEntity("Sensor");

        Console.WriteLine(child.GetType().Name);
        sensor = (Sensor)child;

    }


    public void OnUpdate(float ts)
    {

        Vec3 currentPosition = tC.Translation;

        List<SceneBody> obstacles = sensor.GetObstacles();
        List<SceneBody> robots    = sensor.GetRobots();

        Vec3 robotAvg = new Vec3(0.0f);

        Vec3 cohesion   = new Vec3(0.0f);
        Vec3 separation = new Vec3(0.0f);
        Vec3 alignment  = new Vec3(0.0f);

        Vec3 obstacleAvoidance = new Vec3(0.0f);

        // Cohesion and Separation
        for(int i = 0; i < robots.Count; i++)
        {
            Vec3 robotPos = robots[i].BodyTransform.Translation;
            Vec3 robotLinearVelocity = robots[i].Body.LinearVelocity;

            cohesion  += robotPos;
            alignment += robotLinearVelocity;

            float dist = Vec3.Distance(robotPos, currentPosition);
            if (dist <= separationDistance)
            {
                float distFactor = 1.0f / (dist * dist * dist + 0.001f);
                Vec3 dir = currentPosition - robotPos;
                dir.Normalize();
                separation += dir * distFactor;
            }
        }

        if(robots.Count > 0)
        {
            cohesion = cohesion / robots.Count;
            alignment.Normalize();
        }

        // Obstacle avoidance
        for(int i=0; i < obstacles.Count; i++)
        {
            Vec3 obstaclePos = obstacles[i].BodyTransform.Translation;

            float dist = Vec3.Distance(obstaclePos, currentPosition);
            if (dist <= obstacleDistance)
            {
                float distFactor = 1 / (dist * dist * dist + 0.001f);
                Vec3 dir = currentPosition - obstaclePos;
                dir.Normalize();
                obstacleAvoidance += dir * distFactor;
            }
        }

        Vec3 vel = (cohesion + separation + alignment + obstacleAvoidance) * speed;
        Console.WriteLine("Vel: {0} {1} {2}", vel.x, vel.y, vel.z);

        pC.LinearVelocity = vel;

    }
}

