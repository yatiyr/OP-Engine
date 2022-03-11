using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Opium
{
    public abstract class Component
    {
        public Entity Entity { get; set; }
    }

    public class TagComponent : Component
    {
        public string Tag
        {
            get { return GetTag_Native(Entity.SceneID, Entity.EntityID); }
            set { SetTag_Native(value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetTag_Native(uint sceneID, uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTag_Native(string tag);
    }

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

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTransform_Native(uint sceneID, uint entityID, out Mat4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTransform_Native(uint sceneID, uint entityID, ref Mat4 result);
    }

    public class CameraComponent : Component
    {
        // TODO: Implement this
    }

    public class ScriptComponent : Component
    {
        // TODO: Implement this
    }

    public class SpriteRendererComponent : Component
    {
        // TODO: Implement this
    }
}
