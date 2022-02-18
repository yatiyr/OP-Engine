#include <Opium/Application.h>
#include <iostream>

namespace Opium
{
	Application::Application()
	{

	}

	Application::~Application()
	{

	}

	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			OP_ENGINE_TRACE(e);
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			OP_ENGINE_TRACE(e);
		}
		while (true);
	}
}