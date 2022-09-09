using System.Runtime.CompilerServices;

namespace OP
{
    public class Window
    {
        public static void HideCursor()
        {
            HideCursor_Native();
        }

        public static void ShowCursor()
        {
            ShowCursor_Native();
        }

        public static (float, float) GetMousePos()
        {
            float x, y;
            GetMousePos_Native(out x, out y);
            return (x, y);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void HideCursor_Native();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void ShowCursor_Native();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void GetMousePos_Native(out float x, out float y);
    }
}
