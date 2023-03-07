using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public class CameraComponent : Component
    {
        // Perspective = 0, Orthographic = 1
        public int ProjectionType
        {
            get
            {
                int result;
                GetProjectionType_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetProjectionType_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public bool Primary
        {
            get
            {
                bool result;
                GetPrimary_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetPrimary_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public bool FixedAspectRatio
        {
            get
            {
                bool result;
                GetFixedAspectRatio_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetFixedAspectRatio_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float PerspectiveVerticalFOV
        {
            get
            {
                float result;
                GetPerspectiveVerticalFOV_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetPerspectiveVerticalFOV_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float PerspectiveNearClip
        {
            get
            {
                float result;
                GetPerspectiveNearClip_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetPerspectiveNearClip_Native(Entity.SceneID, Entity.EntityID, ref value);
            }


        }

        public float PerspectiveFarClip
        {
            get
            {
                float result;
                GetPerspectiveFarClip_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetPerspectiveFarClip_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }


        public float OrthographicSize
        {
            get
            {
                float result;
                GetOrthographicSize_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetOrthographicSize_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float OrthographicNearClip
        {
            get
            {
                float result;
                GetOrthographicNearClip_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetOrthographicNearClip_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }

        public float OrthographicFarClip
        {
            get
            {
                float result;
                GetOrthographicFarClip_Native(Entity.SceneID, Entity.EntityID, out result);
                return result;
            }

            set
            {
                SetOrthographicFarClip_Native(Entity.SceneID, Entity.EntityID, ref value);
            }
        }


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetProjectionType_Native(uint entityID, uint sceneID, out int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetProjectionType_Native(uint entityID, uint sceneID, ref int value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPrimary_Native(uint entityID, uint sceneID, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPrimary_Native(uint entityID, uint sceneID, ref bool value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetFixedAspectRatio_Native(uint entityID, uint sceneID, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFixedAspectRatio_Native(uint entityID, uint sceneID, ref bool value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPerspectiveVerticalFOV_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPerspectiveVerticalFOV_Native(uint entityID, uint sceneID, ref float value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPerspectiveNearClip_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPerspectiveNearClip_Native(uint entityID, uint sceneID, ref float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPerspectiveFarClip_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPerspectiveFarClip_Native(uint entityID, uint sceneID, ref float value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetOrthographicSize_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOrthographicSize_Native(uint entityID, uint sceneID, ref float value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetOrthographicNearClip_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOrthographicNearClip_Native(uint entityID, uint sceneID, ref float value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetOrthographicFarClip_Native(uint entityID, uint sceneID, out float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOrthographicFarClip_Native(uint entityID, uint sceneID, ref float value);


    }
}
