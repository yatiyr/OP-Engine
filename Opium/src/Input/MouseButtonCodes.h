#pragma once

namespace Opium
{
	typedef enum class MouseButtonCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} MouseButton;

	inline std::ostream& operator<<(std::ostream& os, MouseButtonCode mouseButtonCode)
	{
		os << static_cast<int32_t>(mouseButtonCode);
		return os;
	}
}

/* Mouse button code taken from glfw */
#define OP_MOUSE_BUTTON_0      ::Opium::Mouse::Button0
#define OP_MOUSE_BUTTON_1      ::Opium::Mouse::Button1
#define OP_MOUSE_BUTTON_2      ::Opium::Mouse::Button2
#define OP_MOUSE_BUTTON_3      ::Opium::Mouse::Button3
#define OP_MOUSE_BUTTON_4      ::Opium::Mouse::Button4
#define OP_MOUSE_BUTTON_5      ::Opium::Mouse::Button5
#define OP_MOUSE_BUTTON_6      ::Opium::Mouse::Button6
#define OP_MOUSE_BUTTON_7      ::Opium::Mouse::Button7
#define OP_MOUSE_BUTTON_LAST   ::Opium::Mouse::ButtonLast
#define OP_MOUSE_BUTTON_LEFT   ::Opium::Mouse::ButtonLeft
#define OP_MOUSE_BUTTON_RIGHT  ::Opium::Mouse::ButtonRight
#define OP_MOUSE_BUTTON_MIDDLE ::Opium::Mouse::ButtonMiddle