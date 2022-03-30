#pragma once

namespace OP
{

	typedef enum class KeyCode : uint16_t
	{
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

#define OP_KEY_SPACE           ::OP::Key::Space
#define OP_KEY_APOSTROPHE      ::OP::Key::Apostrophe    /* ' */
#define OP_KEY_COMMA           ::OP::Key::Comma         /* , */
#define OP_KEY_MINUS           ::OP::Key::Minus         /* - */
#define OP_KEY_PERIOD          ::OP::Key::Period        /* . */
#define OP_KEY_SLASH           ::OP::Key::Slash         /* / */
#define OP_KEY_0               ::OP::Key::D0
#define OP_KEY_1               ::OP::Key::D1
#define OP_KEY_2               ::OP::Key::D2
#define OP_KEY_3               ::OP::Key::D3
#define OP_KEY_4               ::OP::Key::D4
#define OP_KEY_5               ::OP::Key::D5
#define OP_KEY_6               ::OP::Key::D6
#define OP_KEY_7               ::OP::Key::D7
#define OP_KEY_8               ::OP::Key::D8
#define OP_KEY_9               ::OP::Key::D9
#define OP_KEY_SEMICOLON       ::OP::Key::Semicolon     /* ; */
#define OP_KEY_EQUAL           ::OP::Key::Equal         /* = */
#define OP_KEY_A               ::OP::Key::A
#define OP_KEY_B               ::OP::Key::B
#define OP_KEY_C               ::OP::Key::C
#define OP_KEY_D               ::OP::Key::D
#define OP_KEY_E               ::OP::Key::E
#define OP_KEY_F               ::OP::Key::F
#define OP_KEY_G               ::OP::Key::G
#define OP_KEY_H               ::OP::Key::H
#define OP_KEY_I               ::OP::Key::I
#define OP_KEY_J               ::OP::Key::J
#define OP_KEY_K               ::OP::Key::K
#define OP_KEY_L               ::OP::Key::L
#define OP_KEY_M               ::OP::Key::M
#define OP_KEY_N               ::OP::Key::N
#define OP_KEY_O               ::OP::Key::O
#define OP_KEY_P               ::OP::Key::P
#define OP_KEY_Q               ::OP::Key::Q
#define OP_KEY_R               ::OP::Key::R
#define OP_KEY_S               ::OP::Key::S
#define OP_KEY_T               ::OP::Key::T
#define OP_KEY_U               ::OP::Key::U
#define OP_KEY_V               ::OP::Key::V
#define OP_KEY_W               ::OP::Key::W
#define OP_KEY_X               ::OP::Key::X
#define OP_KEY_Y               ::OP::Key::Y
#define OP_KEY_Z               ::OP::Key::Z
#define OP_KEY_LEFT_BRACKET    ::OP::Key::LeftBracket   /* [ */
#define OP_KEY_BACKSLASH       ::OP::Key::Backslash     /* \ */
#define OP_KEY_RIGHT_BRACKET   ::OP::Key::RightBracket  /* ] */
#define OP_KEY_GRAVE_ACCENT    ::OP::Key::GraveAccent   /* ` */
#define OP_KEY_WORLD_1         ::OP::Key::World1        /* non-US #1 */
#define OP_KEY_WORLD_2         ::OP::Key::World2        /* non-US #2 */

/* Function keys */
#define OP_KEY_ESCAPE          ::OP::Key::Escape
#define OP_KEY_ENTER           ::OP::Key::Enter
#define OP_KEY_TAB             ::OP::Key::Tab
#define OP_KEY_BACKSPACE       ::OP::Key::Backspace
#define OP_KEY_INSERT          ::OP::Key::Insert
#define OP_KEY_DELETE          ::OP::Key::Delete
#define OP_KEY_RIGHT           ::OP::Key::Right
#define OP_KEY_LEFT            ::OP::Key::Left
#define OP_KEY_DOWN            ::OP::Key::Down
#define OP_KEY_UP              ::OP::Key::Up
#define OP_KEY_PAGE_UP         ::OP::Key::PageUp
#define OP_KEY_PAGE_DOWN       ::OP::Key::PageDown
#define OP_KEY_HOME            ::OP::Key::Home
#define OP_KEY_END             ::OP::Key::End
#define OP_KEY_CAPS_LOCK       ::OP::Key::CapsLock
#define OP_KEY_SCROLL_LOCK     ::OP::Key::ScrollLock
#define OP_KEY_NUM_LOCK        ::OP::Key::NumLock
#define OP_KEY_PRINT_SCREEN    ::OP::Key::PrintScreen
#define OP_KEY_PAUSE           ::OP::Key::Pause
#define OP_KEY_F1              ::OP::Key::F1
#define OP_KEY_F2              ::OP::Key::F2
#define OP_KEY_F3              ::OP::Key::F3
#define OP_KEY_F4              ::OP::Key::F4
#define OP_KEY_F5              ::OP::Key::F5
#define OP_KEY_F6              ::OP::Key::F6
#define OP_KEY_F7              ::OP::Key::F7
#define OP_KEY_F8              ::OP::Key::F8
#define OP_KEY_F9              ::OP::Key::F9
#define OP_KEY_F10             ::OP::Key::F10
#define OP_KEY_F11             ::OP::Key::F11
#define OP_KEY_F12             ::OP::Key::F12
#define OP_KEY_F13             ::OP::Key::F13
#define OP_KEY_F14             ::OP::Key::F14
#define OP_KEY_F15             ::OP::Key::F15
#define OP_KEY_F16             ::OP::Key::F16
#define OP_KEY_F17             ::OP::Key::F17
#define OP_KEY_F18             ::OP::Key::F18
#define OP_KEY_F19             ::OP::Key::F19
#define OP_KEY_F20             ::OP::Key::F20
#define OP_KEY_F21             ::OP::Key::F21
#define OP_KEY_F22             ::OP::Key::F22
#define OP_KEY_F23             ::OP::Key::F23
#define OP_KEY_F24             ::OP::Key::F24
#define OP_KEY_F25             ::OP::Key::F25

/* Keypad */
#define OP_KEY_KP_0            ::OP::Key::KP0
#define OP_KEY_KP_1            ::OP::Key::KP1
#define OP_KEY_KP_2            ::OP::Key::KP2
#define OP_KEY_KP_3            ::OP::Key::KP3
#define OP_KEY_KP_4            ::OP::Key::KP4
#define OP_KEY_KP_5            ::OP::Key::KP5
#define OP_KEY_KP_6            ::OP::Key::KP6
#define OP_KEY_KP_7            ::OP::Key::KP7
#define OP_KEY_KP_8            ::OP::Key::KP8
#define OP_KEY_KP_9            ::OP::Key::KP9
#define OP_KEY_KP_DECIMAL      ::OP::Key::KPDecimal
#define OP_KEY_KP_DIVIDE       ::OP::Key::KPDivide
#define OP_KEY_KP_MULTIPLY     ::OP::Key::KPMultiply
#define OP_KEY_KP_SUBTRACT     ::OP::Key::KPSubtract
#define OP_KEY_KP_ADD          ::OP::Key::KPAdd
#define OP_KEY_KP_ENTER        ::OP::Key::KPEnter
#define OP_KEY_KP_EQUAL        ::OP::Key::KPEqual

#define OP_KEY_LEFT_SHIFT      ::OP::Key::LeftShift
#define OP_KEY_LEFT_CONTROL    ::OP::Key::LeftControl
#define OP_KEY_LEFT_ALT        ::OP::Key::LeftAlt
#define OP_KEY_LEFT_SUPER      ::OP::Key::LeftSuper
#define OP_KEY_RIGHT_SHIFT     ::OP::Key::RightShift
#define OP_KEY_RIGHT_CONTROL   ::OP::Key::RightControl
#define OP_KEY_RIGHT_ALT       ::OP::Key::RightAlt
#define OP_KEY_RIGHT_SUPER     ::OP::Key::RightSuper
#define OP_KEY_MENU            ::OP::Key::Menu

#define OP_KEY_LAST               OP_KEY_MENU