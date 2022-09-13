﻿using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public class TagComponent : Component
    {
        public string Tag
        {
            get 
            {
                string result;
                GetTag_Native(Entity.EntityID, Entity.SceneID, out result);
                return result;

            }
            set
            {
                SetTag_Native(Entity.EntityID, Entity.SceneID, ref value);
            }
        }

        public int TypeEnum
        {
            get
            {
                int result;
                GetTypeEnum_Native(Entity.EntityID, Entity.SceneID, out result);
                return result;
            }

            set
            {
                SetTypeEnum_Native(Entity.EntityID, Entity.SceneID, ref value);
            }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTag_Native(uint entityID, uint sceneID, out string tag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTag_Native(uint entityID, uint sceneID, ref string tag);


        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTypeEnum_Native(uint entityID, uint sceneID, out int typeEnum);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTypeEnum_Native(uint entityID, uint sceneID, ref int typeEnum);
    }
}
