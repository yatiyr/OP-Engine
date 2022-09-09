using System.Runtime.CompilerServices;

namespace OP
{
    public class Input
    {
        public static bool IsKeyPressed(KeyCode keycode)
        {
            return IsKeyPressed_Native(keycode);
        }

        public static bool IsMouseButtonPressed(MouseButtonCode mouseButtonCode)
        {
            return IsMouseButtonPressed_Native(mouseButtonCode);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsKeyPressed_Native(KeyCode keycode);


        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool IsMouseButtonPressed_Native(MouseButtonCode mouseButtonCode);
    }
}
