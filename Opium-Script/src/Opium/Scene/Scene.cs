using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace OP
{
    public unsafe struct Buffer
    {
        const int MAX_LENGTH = 256;
        fixed byte buffer[MAX_LENGTH];

        public void Clear()
        {
            fixed(byte* ptr=buffer)
            {
                for(int i=0; i<MAX_LENGTH; i++)
                {
                    ptr[i] = 0;
                }
            }
        }

        public void Store(byte[] array, int index)
        {
            fixed(byte* ptr=array)
            {
                int remainingMax = Math.Min(index + array.Length, MAX_LENGTH) - index;
                for (int i=0; i<remainingMax; i++)
                {
                    ptr[index + i] = array[i];
                }
            }
        }

        public byte[] ToArray()
        {
            byte[] array = new byte[MAX_LENGTH];
            fixed(byte* ptr=buffer)
            {
                for(int i=0; i<MAX_LENGTH; i++)
                {
                    array[i] = ptr[i];
                }
            }

            return array;
        }
    }

    public class Scene
    {

        static private Buffer m_StringBuffer;

        static public string Skybox
        {
            get
            {
                m_StringBuffer.Clear();
                GetSkybox_Native(out m_StringBuffer);
                byte[] stringBytes = m_StringBuffer.ToArray();
                string result = Encoding.ASCII.GetString(stringBytes);
                return result;
            }
            set
            {
                m_StringBuffer.Clear();
                byte[] values = Encoding.ASCII.GetBytes(value);
                m_StringBuffer.Store(values, 0);
                SetSkybox_Native(ref m_StringBuffer);
            }
        }

        static public bool ToneMap
        {
            get
            {
                bool ToneMap;
                GetToneMap_Native(out ToneMap);
                return ToneMap;
            }

            set
            {
                SetToneMap_Native(value);
            }
        }

        static public float Exposure
        {
            get
            {
                float Exposure;
                GetExposure_Native(out Exposure);
                return Exposure;
            }

            set
            {
                SetExposure_Native(value);
            }
        }


        static public string SceneHandler
        {
            get
            {
                m_StringBuffer.Clear();
                GetScene_Native(out m_StringBuffer);
                byte[] stringBytes = m_StringBuffer.ToArray();
                return Encoding.ASCII.GetString(stringBytes);
            }
            set
            {
                m_StringBuffer.Clear();
                byte[] values = Encoding.ASCII.GetBytes(value);
                m_StringBuffer.Store(values, 0);
                SetScene_Native(ref m_StringBuffer);
            }
        }

        /////////////////////////////// GET-SET SKYBOX ////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetSkybox_Native(out Buffer result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetSkybox_Native(ref Buffer result);
        /////////////////////////////// GET-SET TONEMAP ////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetToneMap_Native(out bool result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetToneMap_Native(bool result);
        /////////////////////////////// GET-SET EXPOSURE ///////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetExposure_Native(out float result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetExposure_Native(float result);
        ////////////////////////////// GET-SET SCENE ////////////////////////////////////
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetScene_Native(out Buffer result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void SetScene_Native(ref Buffer result);

    }
}
