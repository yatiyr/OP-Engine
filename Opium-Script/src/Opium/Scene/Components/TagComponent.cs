using System;
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
            get { return GetTag_Native(Entity.SceneID, Entity.EntityID); }
            set { SetTag_Native(value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetTag_Native(uint sceneID, uint entityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTag_Native(string tag);
    }
}
