#pragma once


// Entry point is being controlled by the engine

#ifdef OP_PLATFORM_WINDOWS

	extern OP::Application* OP::CreateApplication(AppCommandLineArguments args);

	int main(int argc, char** argv)
	{
		OP::Logging::Init();
		OP_ENGINE_WARN("Logging Service has been initialized!");

		OP::Application* Application = OP::CreateApplication({ argc, argv });
		Application->Run();


		delete Application;
	}

#else

	extern OP::Application* OP::CreateApplication();

	int main(int argc, char** argv)
	{
		OP::Logging::Init();

		auto app = Op::CreateApplication();
		app->Run();
		delete app;
	}

#endif