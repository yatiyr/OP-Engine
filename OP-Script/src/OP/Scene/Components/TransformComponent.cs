using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace OP
{
    public class TransformComponent : Component
    {
        public Mat4 Transform
        {
            get
            {
                Mat4 result;
                GetTransform_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }
            set
            {
                SetTransform_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 Translation
        {
            get
            {
                Vec3 result;
                GetTranslation_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetTranslation_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 RotationEuler
        {
            get
            {
                Vec3 result;
                GetRotationEuler_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetRotationEuler_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec4 RotationQuat
        {
            get
            {
                Vec4 result;
                GetRotationQuat_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetRotationQuat_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 Scale
        {
            get
            {
                Vec3 result;
                GetScale_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetScale_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public Vec3 GetDirection()
        {
            Vec3 result;
            GetDirection_Native(Entity.SceneID, Entity.EntityID, out result);
            return result;
        }

        public void RotateFromTwoVectors(Vec3 axis1, Vec3 axis2)
        {
            RotateFromTwoVectors_Native(Entity.SceneID, Entity.EntityID, ref axis1, ref axis2);
        }

        ////////////////////////////// GET-SET TRANSFORM ////////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTransform_Native(uint sceneID, uint entityID, out Mat4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransform_Native(uint sceneID, uint entityID, ref Mat4 result);
        ////////////////////////////////////// GET-SET TRANSLATION //////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTranslation_Native(uint sceneID, uint entityID, out Vec3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTranslation_Native(uint sceneID, uint entityID, ref Vec3 result);
        /////////////////////////////////// GET-SET ROTATION EULER //////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRotationEuler_Native(uint sceneID, uint entityID, out Vec3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRotationEuler_Native(uint sceneID, uint entityID, ref Vec3 result);
        ///////////////////////////////// GET-SET ROTATION QUAT //////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetRotationQuat_Native(uint sceneID, uint entityID, out Vec4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRotationQuat_Native(uint sceneID, uint entityID, ref Vec4 result);
        //////////////////////////////////// GET-SET SCALE ///////////////////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetScale_Native(uint sceneID, uint entityID, out Vec3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetScale_Native(uint sceneID, uint entityID, ref Vec3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RotateFromTwoVectors_Native(uint sceneID, uint entityID, ref Vec3 axis1, ref Vec3 axis2);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetDirection_Native(uint sceneID, uint entityID, out Vec3 result);

    }
}