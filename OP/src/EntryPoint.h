#pragma once



extern OP::Application* OP::CreateApplication(AppCommandLineArguments args);

int main(int argc, char** argv)
{

	// Initialize the spdlog
	OP::Logging::Init();

	OP_ENGINE_TRACE("Logging Init!");

	// Create the application and run it
	OP::Application* Application = OP::CreateApplication({ argc, argv });
	Application->Run();

	// Delete the application after it is complete or there has been an error
	delete Application;

	return 0;
}
