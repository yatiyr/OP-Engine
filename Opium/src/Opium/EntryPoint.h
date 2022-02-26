#pragma once

// Entry points are currently the same for all platforms
// But this is going to be changed in the future
#ifdef OP_PLATFORM_WINDOWS

	extern Opium::Application* Opium::CreateApplication();

	int main(int argc, char** argv)
	{
		Opium::Logging::Init();
		OP_ENGINE_WARN("Initialized log for engine");
		OP_APP_INFO("Initialized app log!");

		OP_PROFILE_BEGIN_SESSION("Startup Session", "OpiumProfile_startup.json");
		auto app = Opium::CreateApplication();
		OP_PROFILE_END_SESSION();

		OP_PROFILE_BEGIN_SESSION("Runtime Session", "OpiumProfile_runtime.json");
		app->Run();
		OP_PROFILE_END_SESSION();

		OP_PROFILE_BEGIN_SESSION("Shutdown Session", "OpiumProfile_shutdown.json");
		delete app;
		OP_PROFILE_END_SESSION();
	}

#else

	extern Opium::Application* Opium::CreateApplication();

	int main(int argc, char** argv)
	{
		Opium::Logging::Init();

		auto app = Opium::CreateApplication();
		app->Run();
		delete app;
	}

#endif