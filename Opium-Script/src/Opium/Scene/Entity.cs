using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public class Entity
    {
        

        void OnCreate()
        {

        }
        void OnCollision_Native(uint EntityID, uint SceneID, float x, float y, float z)
        {
            Entity oE = new Entity();
            oE.EntityID = EntityID;
            oE.SceneID = SceneID;

            Vec3 cP;
            cP.x = x;
            cP.y = y;
            cP.z = z;
            OnCollision(oE, cP);
        }

        void OnCollisionStarted_Native(uint EntityID, uint SceneID)
        {
            Entity oE = new Entity();
            oE.EntityID = EntityID;
            oE.SceneID = SceneID;

            OnCollisionStarted(oE);
        }

        void OnCollisionEnded_Native(uint EntityID, uint SceneID)
        {
            Entity oE = new Entity();
            oE.EntityID = EntityID;
            oE.SceneID = SceneID;
            OnCollisionEnded(oE);
        }

        void OnCollision(Entity otherEntity, Vec3 collisionPoint)
        {
            string res = String.Format("EntityID = {0}, SceneID = {1}, CollisionPoint=[{2},{3},{4}]",
                otherEntity.EntityID, otherEntity.SceneID,
                collisionPoint.x, collisionPoint.y, collisionPoint.z);
            Console.WriteLine(res);
        }

        void OnCollisionStarted(Entity otherEntity)
        {
            Console.WriteLine("OnCollisionStarted");
        }

        void OnCollisionEnded(Entity otherEntity)
        {
            Console.WriteLine("OnCollisionEnded");
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void CreateComponent_Native(uint sceneID, uint entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent_Native(uint sceneID, uint entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetTransform_Native(uint sceneID, uint entityID, out Mat4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTransform_Native(uint sceneID, uint entityID, ref Mat4 matrix);


        public uint SceneID { get; set; }
        public uint EntityID { get; set; }

        ~Entity()
        {
            Console.WriteLine("Destroyed Entity {0}:{1}", SceneID, EntityID);
        }

        public T CreateComponent<T>() where T : Component, new()
        {
            CreateComponent_Native(SceneID, EntityID, typeof(T));
            T component = new T();
            component.Entity = this;
            return component;
        }

        public bool HasComponent<T>() where T : Component, new()
        {
            return HasComponent_Native(SceneID, EntityID, typeof(T));
        }

        public T GetComponent<T>() where T : Component, new()
        {
            if (HasComponent<T>())
            {
                T component = new T();
                component.Entity = this;
                return component;
            }
            return null;
        }

        public Mat4 GetTransform()
        {
            Mat4 mat4Instance;
            GetTransform_Native(SceneID, EntityID, out mat4Instance);
            return mat4Instance;
        }

        public void SetTransform(Mat4 transform)
        {
            SetTransform_Native(SceneID, EntityID, ref transform);
        }

    }
}
