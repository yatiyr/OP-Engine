using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public class Physics3DMaterial : Component
    {

        public float Mass
        {
            get 
            {
                float result;
                GetMass_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            { 
                SetMass_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float Friction
        {
            get
            {
                float result;
                GetFriction_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            {
                SetFriction_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float RollingFriction
        {
            get
            {
                float result;
                GetRollingFriction_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            {
                SetRollingFriction_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float SpinningFriction
        {
            get
            {
                float result;
                GetSpinningFriction_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            {
                SetSpinningFriction_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float Restitution
        {
            get
            {
                float result;
                GetRestitution_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            {
                SetRestitution_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 AngularVelocity
        {
            get
            {
                Vec3 result;
                GetAngularVelocity_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetAngularVelocity_Native (Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 LinearVelocity
        {
            get
            {
                Vec3 result;
                GetLinearVelocity_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetLinearVelocity_Native (Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public void ApplyCentralForce(Vec3 force)
        {
            ApplyCentralForce_Native(Entity.SceneID, Entity.EntityID, force.x, force.y, force.z);
        }

        public void ApplyCentralImpulse(Vec3 impulse)
        {
            ApplyCentralImpulse_Native(Entity.SceneID, Entity.EntityID, impulse.x, impulse.y, impulse.z);
        }

        public void ApplyForce(Vec3 force, Vec3 point)
        {
            ApplyForce_Native(Entity.SceneID, Entity.EntityID, force.x, force.y, force.z, point.x, point.y, point.z);
        }

        public void ApplyGravity()
        {
            ApplyGravity_Native(Entity.SceneID, Entity.EntityID);
        }

        public void ApplyImpulse(Vec3 impulse, Vec3 point)
        {
            ApplyImpulse_Native(Entity.SceneID, Entity.EntityID, impulse.x, impulse.y, impulse.z, point.x, point.y, point.z);
        }

        public void ApplyTorque(Vec3 torque)
        {
            ApplyTorque_Native(Entity.SceneID, Entity.EntityID, torque.x, torque.y, torque.z);
        }

        public void ApplyTorqueImpulse(Vec3 torqueImpulse)
        {
            ApplyTorqueImpulse_Native(Entity.SceneID, Entity.EntityID, torqueImpulse.x, torqueImpulse.y, torqueImpulse.z);
        }

        public void ClearForces()
        {
            ClearForces_Native(Entity.SceneID, Entity.EntityID);
        }

        /////////////////////////////// RIGID BODY FUNCTIONS //////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyCentralForce_Native(uint sceneID, uint entityID, float fX, float fY, float fZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyCentralImpulse_Native(uint sceneID, uint entityID, float fX, float fY, float fZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyForce_Native(uint sceneID, uint entityID, float fX, float fY, float fZ,
                                                                                 float pX, float pY, float pZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyGravity_Native(uint sceneID, uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyImpulse_Native(uint sceneID, uint entityID, float fX, float fY, float fZ,
                                                                                   float pX, float pY, float pZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyTorque_Native(uint sceneID, uint entityID, float tX, float tY, float tZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ApplyTorqueImpulse_Native(uint sceneID, uint entityID, float tX, float tY, float tZ);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ClearForces_Native(uint sceneID, uint entityID);



        ///////////////////////////////////// GET-SET ANGULAR VELOCITY ////////////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAngularVelocity_Native(uint sceneID, uint entityID, out Vec3 aVelocity);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetAngularVelocity_Native(uint sceneID, uint entityID, ref Vec3 aVelocity);
        //////////////////////////////////// GET-SET LINEAR VELOCITY //////////////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetLinearVelocity_Native(uint sceneID, uint entityID, out Vec3 lVelovicty);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetLinearVelocity_Native(uint sceneID, uint entityID, ref Vec3 lVelocity);
        //////////////////////////// GET-SET MASS /////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMass_Native(uint sceneID, uint entityID, out float mass);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetMass_Native(uint sceneID, uint entityID, ref float mass);

        ////////////////////////////// GET-SET FRICTION ///////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetFriction_Native(uint sceneID, uint entityID, out float friction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFriction_Native(uint sceneID, uint entityID, ref float friction);
        //////////////////////////// GET-SET R_FRICTION ///////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRollingFriction_Native(uint sceneID, uint entityID, out float rollingFriction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRollingFriction_Native(uint sceneID, uint entityID, ref float rollingFriction);
        //////////////////////////// GET-SET R_FRICTION ///////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetSpinningFriction_Native(uint sceneID, uint entityID, out float spinningFriction);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSpinningFriction_Native(uint sceneID, uint entityID, ref float spinningFriction);
        //////////////////////////// GET-SET R_FRICTION ///////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRestitution_Native(uint sceneID, uint entityID, out float restitution);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRestitution_Native(uint sceneID, uint entityID, ref float restitution);

    }
}