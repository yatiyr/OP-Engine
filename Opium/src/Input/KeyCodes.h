#pragma once

namespace Opium
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

#define OP_KEY_SPACE           ::Opium::Key::Space
#define OP_KEY_APOSTROPHE      ::Opium::Key::Apostrophe    /* ' */
#define OP_KEY_COMMA           ::Opium::Key::Comma         /* , */
#define OP_KEY_MINUS           ::Opium::Key::Minus         /* - */
#define OP_KEY_PERIOD          ::Opium::Key::Period        /* . */
#define OP_KEY_SLASH           ::Opium::Key::Slash         /* / */
#define OP_KEY_0               ::Opium::Key::D0
#define OP_KEY_1               ::Opium::Key::D1
#define OP_KEY_2               ::Opium::Key::D2
#define OP_KEY_3               ::Opium::Key::D3
#define OP_KEY_4               ::Opium::Key::D4
#define OP_KEY_5               ::Opium::Key::D5
#define OP_KEY_6               ::Opium::Key::D6
#define OP_KEY_7               ::Opium::Key::D7
#define OP_KEY_8               ::Opium::Key::D8
#define OP_KEY_9               ::Opium::Key::D9
#define OP_KEY_SEMICOLON       ::Opium::Key::Semicolon     /* ; */
#define OP_KEY_EQUAL           ::Opium::Key::Equal         /* = */
#define OP_KEY_A               ::Opium::Key::A
#define OP_KEY_B               ::Opium::Key::B
#define OP_KEY_C               ::Opium::Key::C
#define OP_KEY_D               ::Opium::Key::D
#define OP_KEY_E               ::Opium::Key::E
#define OP_KEY_F               ::Opium::Key::F
#define OP_KEY_G               ::Opium::Key::G
#define OP_KEY_H               ::Opium::Key::H
#define OP_KEY_I               ::Opium::Key::I
#define OP_KEY_J               ::Opium::Key::J
#define OP_KEY_K               ::Opium::Key::K
#define OP_KEY_L               ::Opium::Key::L
#define OP_KEY_M               ::Opium::Key::M
#define OP_KEY_N               ::Opium::Key::N
#define OP_KEY_O               ::Opium::Key::O
#define OP_KEY_P               ::Opium::Key::P
#define OP_KEY_Q               ::Opium::Key::Q
#define OP_KEY_R               ::Opium::Key::R
#define OP_KEY_S               ::Opium::Key::S
#define OP_KEY_T               ::Opium::Key::T
#define OP_KEY_U               ::Opium::Key::U
#define OP_KEY_V               ::Opium::Key::V
#define OP_KEY_W               ::Opium::Key::W
#define OP_KEY_X               ::Opium::Key::X
#define OP_KEY_Y               ::Opium::Key::Y
#define OP_KEY_Z               ::Opium::Key::Z
#define OP_KEY_LEFT_BRACKET    ::Opium::Key::LeftBracket   /* [ */
#define OP_KEY_BACKSLASH       ::Opium::Key::Backslash     /* \ */
#define OP_KEY_RIGHT_BRACKET   ::Opium::Key::RightBracket  /* ] */
#define OP_KEY_GRAVE_ACCENT    ::Opium::Key::GraveAccent   /* ` */
#define OP_KEY_WORLD_1         ::Opium::Key::World1        /* non-US #1 */
#define OP_KEY_WORLD_2         ::Opium::Key::World2        /* non-US #2 */

/* Function keys */
#define OP_KEY_ESCAPE          ::Opium::Key::Escape
#define OP_KEY_ENTER           ::Opium::Key::Enter
#define OP_KEY_TAB             ::Opium::Key::Tab
#define OP_KEY_BACKSPACE       ::Opium::Key::Backspace
#define OP_KEY_INSERT          ::Opium::Key::Insert
#define OP_KEY_DELETE          ::Opium::Key::Delete
#define OP_KEY_RIGHT           ::Opium::Key::Right
#define OP_KEY_LEFT            ::Opium::Key::Left
#define OP_KEY_DOWN            ::Opium::Key::Down
#define OP_KEY_UP              ::Opium::Key::Up
#define OP_KEY_PAGE_UP         ::Opium::Key::PageUp
#define OP_KEY_PAGE_DOWN       ::Opium::Key::PageDown
#define OP_KEY_HOME            ::Opium::Key::Home
#define OP_KEY_END             ::Opium::Key::End
#define OP_KEY_CAPS_LOCK       ::Opium::Key::CapsLock
#define OP_KEY_SCROLL_LOCK     ::Opium::Key::ScrollLock
#define OP_KEY_NUM_LOCK        ::Opium::Key::NumLock
#define OP_KEY_PRINT_SCREEN    ::Opium::Key::PrintScreen
#define OP_KEY_PAUSE           ::Opium::Key::Pause
#define OP_KEY_F1              ::Opium::Key::F1
#define OP_KEY_F2              ::Opium::Key::F2
#define OP_KEY_F3              ::Opium::Key::F3
#define OP_KEY_F4              ::Opium::Key::F4
#define OP_KEY_F5              ::Opium::Key::F5
#define OP_KEY_F6              ::Opium::Key::F6
#define OP_KEY_F7              ::Opium::Key::F7
#define OP_KEY_F8              ::Opium::Key::F8
#define OP_KEY_F9              ::Opium::Key::F9
#define OP_KEY_F10             ::Opium::Key::F10
#define OP_KEY_F11             ::Opium::Key::F11
#define OP_KEY_F12             ::Opium::Key::F12
#define OP_KEY_F13             ::Opium::Key::F13
#define OP_KEY_F14             ::Opium::Key::F14
#define OP_KEY_F15             ::Opium::Key::F15
#define OP_KEY_F16             ::Opium::Key::F16
#define OP_KEY_F17             ::Opium::Key::F17
#define OP_KEY_F18             ::Opium::Key::F18
#define OP_KEY_F19             ::Opium::Key::F19
#define OP_KEY_F20             ::Opium::Key::F20
#define OP_KEY_F21             ::Opium::Key::F21
#define OP_KEY_F22             ::Opium::Key::F22
#define OP_KEY_F23             ::Opium::Key::F23
#define OP_KEY_F24             ::Opium::Key::F24
#define OP_KEY_F25             ::Opium::Key::F25

/* Keypad */
#define OP_KEY_KP_0            ::Opium::Key::KP0
#define OP_KEY_KP_1            ::Opium::Key::KP1
#define OP_KEY_KP_2            ::Opium::Key::KP2
#define OP_KEY_KP_3            ::Opium::Key::KP3
#define OP_KEY_KP_4            ::Opium::Key::KP4
#define OP_KEY_KP_5            ::Opium::Key::KP5
#define OP_KEY_KP_6            ::Opium::Key::KP6
#define OP_KEY_KP_7            ::Opium::Key::KP7
#define OP_KEY_KP_8            ::Opium::Key::KP8
#define OP_KEY_KP_9            ::Opium::Key::KP9
#define OP_KEY_KP_DECIMAL      ::Opium::Key::KPDecimal
#define OP_KEY_KP_DIVIDE       ::Opium::Key::KPDivide
#define OP_KEY_KP_MULTIPLY     ::Opium::Key::KPMultiply
#define OP_KEY_KP_SUBTRACT     ::Opium::Key::KPSubtract
#define OP_KEY_KP_ADD          ::Opium::Key::KPAdd
#define OP_KEY_KP_ENTER        ::Opium::Key::KPEnter
#define OP_KEY_KP_EQUAL        ::Opium::Key::KPEqual

#define OP_KEY_LEFT_SHIFT      ::Opium::Key::LeftShift
#define OP_KEY_LEFT_CONTROL    ::Opium::Key::LeftControl
#define OP_KEY_LEFT_ALT        ::Opium::Key::LeftAlt
#define OP_KEY_LEFT_SUPER      ::Opium::Key::LeftSuper
#define OP_KEY_RIGHT_SHIFT     ::Opium::Key::RightShift
#define OP_KEY_RIGHT_CONTROL   ::Opium::Key::RightControl
#define OP_KEY_RIGHT_ALT       ::Opium::Key::RightAlt
#define OP_KEY_RIGHT_SUPER     ::Opium::Key::RightSuper
#define OP_KEY_MENU            ::Opium::Key::Menu

#define OP_KEY_LAST               OP_KEY_MENU