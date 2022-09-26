using System;
using System.Threading.Tasks;
using System.Collections.Generic;
using OP;


internal class FreeFirstPersonCamera : Entity
{

    private TransformComponent Transform;
    private (float, float) LastMousePos = (0.0f, 0.0f);
    private (float, float) MouseDelta = (0.0f, 0.0f);


    public float YawSpeed = 0.001f;
    public float PitchSpeed = 0.001f;
    public float MoveSpeed = 1.0f;

    private Vec3 UpVec = new Vec3(0.0f, 1.0f, 0.0f);


    private bool MoveDisabled = false;

    public void OnCreate()
    {
        Transform = GetComponent<TransformComponent>();
        LastMousePos = Window.GetMousePos();
    }

    private void HandleKeyboardInputs(float ts)
    {
        if (Input.IsKeyPressed(KeyCode.O))
        {
            Window.HideCursor();
            MoveDisabled = false;
        }

        if (Input.IsKeyPressed(KeyCode.P))
        {
            Window.ShowCursor();
            MoveDisabled = true;
        }

        if(!MoveDisabled)
        {
            // Get Forward and Right
            Vec3 ForwardDir = Transform.GetDirection();
            Vec3 RightDir = Vec3.Cross(UpVec, ForwardDir);

            // Forward
            if (Input.IsKeyPressed(KeyCode.W))
            {
                Vec3 pos = Transform.Translation;
                pos += ForwardDir * MoveSpeed * ts;
                Transform.Translation = pos;
            }

            if (Input.IsKeyPressed(KeyCode.S))
            {
                Vec3 pos = Transform.Translation;
                pos -= ForwardDir * MoveSpeed * ts;
                Transform.Translation = pos;
            }

            if (Input.IsKeyPressed(KeyCode.A))
            {
                Vec3 pos = Transform.Translation;
                pos += RightDir * MoveSpeed * ts;
                Transform.Translation = pos;
            }

            if (Input.IsKeyPressed(KeyCode.D))
            {
                Vec3 pos = Transform.Translation;
                pos -= RightDir * MoveSpeed * ts;
                Transform.Translation = pos;
            }
        }
    }

    private void  HandleCameraOrPosInputs(float ts)
    {
        (float, float) newMousePos = Window.GetMousePos();
        MouseDelta.Item1 = LastMousePos.Item1 - newMousePos.Item1;
        MouseDelta.Item2 = LastMousePos.Item2 - newMousePos.Item2;

        // MouseDelta x yaw
        // MouseDelta y pitch
        Vec3 rotation = Transform.RotationEuler;

        rotation.x += MouseDelta.Item2 * YawSpeed * ts;
        rotation.y += MouseDelta.Item1 * PitchSpeed * ts;
        Transform.RotationEuler = rotation;

        LastMousePos = newMousePos;
    }

    public void OnUpdate(float ts)
    {

        HandleKeyboardInputs(ts);

        if(!MoveDisabled)
            HandleCameraOrPosInputs(ts);
    }
}

