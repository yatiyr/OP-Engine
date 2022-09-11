﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public class Entity
    {

        private Buffer m_EntityStringBuffer;

        void OnCreate()
        {

        }

        void OnCollision(uint otherEntityID, float x, float y, float z)
        {
            Console.WriteLine("Entity Class On Collision");
        }

        void OnCollisionStarted(uint otherEntityID)
        {
            Console.WriteLine("Entity Class On Collision Started");
        }

        void OnCollisionEnded(uint otherEntityID)
        {
            Console.WriteLine("Entity Class On Collision Ended");
        }

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

        public Entity GetChildEntity(string tag)
        {
            m_EntityStringBuffer.Clear();
            byte[] values = Encoding.ASCII.GetBytes(tag);
            m_EntityStringBuffer.Store(values, 0);

            Object res;
            GetChildEntity_Native(SceneID, EntityID, ref m_EntityStringBuffer, out res);
            return (Entity)res;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void CreateComponent_Native(uint sceneID, uint entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool HasComponent_Native(uint sceneID, uint entityID, Type type);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetTransform_Native(uint sceneID, uint entityID, out Mat4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetTransform_Native(uint sceneID, uint entityID, ref Mat4 matrix);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetChildEntity_Native(uint sceneID, uint entityID, ref Buffer tag, out Object obj);

    }
}
