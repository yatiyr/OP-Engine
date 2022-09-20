using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;

using static OP.Constants;

internal class SwarmRobot : Entity
{

    // Needed components
    private TransformComponent tC;
    private Physics3DMaterial pC;

    // Parent entity
    private RobotManager robotManager;

    // Child Entity
    private Sensor sensor;

    // Internal variables
    private Vec3 targetVelocity = new Vec3();
    private Vec3 velocity       = new Vec3();

    
    // Determines original orientation of the robot is facing
    // upwards y direction
    private Vec3 originalAxis = new Vec3(0.0f, 1.0f, 0.0f);

    public void OnCreate()
    {

        // Get Components
        tC = GetComponent<TransformComponent>();
        pC = GetComponent<Physics3DMaterial>();

        // Get Parent
        robotManager = (RobotManager)GetParentEntity();

        // Get Child
        sensor       = (Sensor)GetChildEntity("Sensor");

    }

    public void OnUpdate(float ts)
    {

        // Current position info is taken from transform component
        Vec3 currentPosition = tC.Translation;

        // We get which robots and obstacles are around
        List<SceneBody> obstacles = sensor.GetObstacles();
        List<SceneBody> robots    = sensor.GetRobots();

        // We need these for target velocity calculation
        Vec3 cohesion   = new Vec3(0.0f);
        Vec3 separation = new Vec3(0.0f);
        Vec3 alignment  = new Vec3(0.0f);
        Vec3 obstacleAvoidance = new Vec3(0.0f);



        Vec3 averageRobotPos = new Vec3(0.0f);

        

        // Cohesion and Separation
        for(int i = 0; i < robots.Count; i++)
        {
            Vec3 robotPos = robots[i].BodyTransform.Translation;
            Vec3 robotToCur = robotPos - currentPosition;
            robotToCur.Normalize();
            Vec3 curVelocity = velocity;
            curVelocity.Normalize();

            float cosAngle = Vec3.Dot(curVelocity, robotToCur);
            float cosAngleThreshold = (float)Math.Cos(robotManager.SensorAngle * PI / 180.0f);

            if (cosAngle >= cosAngleThreshold)
            {
                Vec3 robotLinearVelocity = robots[i].Body.LinearVelocity;

                averageRobotPos += robotPos;
                alignment += robotLinearVelocity;

                float dist = Vec3.Distance(robotPos, currentPosition);
                if (dist <= robotManager.SeparationDistance)
                {
                    float distFactor = 1.0f / (dist * dist * dist + 0.001f);
                    Vec3 dir = currentPosition - robotPos;
                    dir.Normalize();
                    separation += dir * distFactor;
                }
            }
            
        }


        if (robots.Count > 0)
        {

            averageRobotPos = averageRobotPos / robots.Count;
            cohesion = averageRobotPos - currentPosition;
            cohesion.Normalize();
            alignment.Normalize();
        }

        // Obstacle avoidance
        for (int i=0; i < obstacles.Count; i++)
        {
            Vec3 obstaclePos = obstacles[i].BodyTransform.Translation;

            float dist = Vec3.Distance(obstaclePos, currentPosition);
            if (dist <= robotManager.ObstacleAvoidanceDistance)
            {
                float distFactor = 1 / (dist * dist * dist + 0.001f);
                Vec3 dir = currentPosition - obstaclePos;
                dir.Normalize();
                obstacleAvoidance += dir * distFactor;
            }
        }

        if (robots.Count == 0 && obstacles.Count == 0)
        {
            targetVelocity.x = 0;
            targetVelocity.y = 0;
            targetVelocity.z = 0;
        }
        else
        {
            targetVelocity = (cohesion * robotManager.Cohesion + alignment * robotManager.Alignment + separation * robotManager.Separation + obstacleAvoidance * robotManager.ObstacleAvoidance);
            targetVelocity.Normalize();
        }

        if (currentPosition.x < robotManager.Xmin)
            targetVelocity = new Vec3(5.5f, 0.0f, 0.0f);
        else if (currentPosition.x > robotManager.Xmax)
            targetVelocity = new Vec3(-5.5f, 0.0f, 0.0f);
        else if (currentPosition.y < robotManager.Ymin)
            targetVelocity = new Vec3(0.0f, 5.5f, 0.0f);
        else if (currentPosition.y > robotManager.Ymax)
            targetVelocity = new Vec3(0.0f, -5.5f, 0.0f);
        else if (currentPosition.z < robotManager.Zmin)
            targetVelocity = new Vec3(0.0f, 0.0f, 5.5f);
        else if (currentPosition.z > robotManager.Zmax)
            targetVelocity = new Vec3(0.0f, 0.0f, -5.5f);

        velocity = Vec3.Mix(velocity, targetVelocity, ts * robotManager.VelocityChangeSpeed);
        velocity.Normalize();

        tC.RotateFromTwoVectors(originalAxis, velocity);

        velocity = velocity * robotManager.Speed;


        pC.LinearVelocity = velocity;

        

    }
}

