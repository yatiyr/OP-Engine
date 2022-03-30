#pragma once

namespace OP
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
#define OP_MOUSE_BUTTON_0      ::OP::Mouse::Button0
#define OP_MOUSE_BUTTON_1      ::OP::Mouse::Button1
#define OP_MOUSE_BUTTON_2      ::OP::Mouse::Button2
#define OP_MOUSE_BUTTON_3      ::OP::Mouse::Button3
#define OP_MOUSE_BUTTON_4      ::OP::Mouse::Button4
#define OP_MOUSE_BUTTON_5      ::OP::Mouse::Button5
#define OP_MOUSE_BUTTON_6      ::OP::Mouse::Button6
#define OP_MOUSE_BUTTON_7      ::OP::Mouse::Button7
#define OP_MOUSE_BUTTON_LAST   ::OP::Mouse::ButtonLast
#define OP_MOUSE_BUTTON_LEFT   ::OP::Mouse::ButtonLeft
#define OP_MOUSE_BUTTON_RIGHT  ::OP::Mouse::ButtonRight
#define OP_MOUSE_BUTTON_MIDDLE ::OP::Mouse::ButtonMiddle