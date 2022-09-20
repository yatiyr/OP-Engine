#pragma once

#include <Precomp.h>
#include <Op/Core.h>
#include <Input/MouseButtonCodes.h>
#include <Input/KeyCodes.h>

namespace OP
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode keycode);
		static bool IsMouseButtonPressed(MouseButtonCode button);

		static std::pair<float, float> GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}