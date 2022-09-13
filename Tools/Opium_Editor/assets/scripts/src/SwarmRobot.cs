﻿using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;


internal class SwarmRobot : Entity
{
    public float speed = 1.0f;

    private TransformComponent tC;
    private Physics3DMaterial pC;


    Sensor sensor;


    public float separationDistance = 0.2f;
    public float obstacleDistance = 0.4f;
    public float cohesionFactor = 1.0f;

    private Vec3 targetVelocity;
    private Vec3 velocity;

    public float velChangeSpeed = 5.0f;

    public void OnCreate()
    {
        Console.WriteLine("Leader Robot Has been Created");
        tC = GetComponent<TransformComponent>();
        pC = GetComponent<Physics3DMaterial>();

        Entity child = GetChildEntity("Sensor");

        Console.WriteLine(child.GetType().Name);
        sensor = (Sensor)child;

        targetVelocity = new Vec3();
        velocity = new Vec3();

    }


    public void OnUpdate(float ts)
    {

        Vec3 currentPosition = tC.Translation;

        List<SceneBody> obstacles = sensor.GetObstacles();
        List<SceneBody> robots    = sensor.GetRobots();


        Vec3 cohesion   = new Vec3(0.0f);
        Vec3 separation = new Vec3(0.0f);
        Vec3 alignment  = new Vec3(0.0f);
        Vec3 averageRobotPos = new Vec3(0.0f);

        Vec3 obstacleAvoidance = new Vec3(0.0f);

        // Cohesion and Separation
        for(int i = 0; i < robots.Count; i++)
        {
            Vec3 robotPos = robots[i].BodyTransform.Translation;
            Vec3 robotLinearVelocity = robots[i].Body.LinearVelocity;

            averageRobotPos += robotPos;
            alignment += robotLinearVelocity;

            float dist = Vec3.Distance(robotPos, currentPosition);
            if (dist <= separationDistance)
            {
                float distFactor = 1.0f / (dist * dist + 0.001f);
                Vec3 dir = currentPosition - robotPos;
                dir.Normalize();
                separation += dir * distFactor;
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
            if (dist <= obstacleDistance)
            {
                float distFactor = 1 / (dist * dist + 0.001f);
                Vec3 dir = currentPosition - obstaclePos;
                dir.Normalize();
                obstacleAvoidance += dir * distFactor;
            }
        }

        Console.Write("RobotCount = {0}, ObstacleCount = {1}", robots.Count, obstacles.Count);
        if (robots.Count == 0 && obstacles.Count == 0)
        {
            targetVelocity.x = 0;
            targetVelocity.y = 0;
            targetVelocity.z = 0;
        }
        else
        {
            targetVelocity = (cohesion * cohesionFactor + alignment + separation + obstacleAvoidance);
            targetVelocity.Normalize();
        }

        velocity = Vec3.Mix(velocity, targetVelocity, ts * velChangeSpeed);
        velocity.Normalize();

        pC.LinearVelocity = velocity * speed;

    }
}

