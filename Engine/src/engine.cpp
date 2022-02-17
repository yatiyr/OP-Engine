#include <OpiumEngine/engine.h>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <OpiumEngine/serviceLocator.h>

#include "Platform/glfwWindow.h"

void OpiumEngine::Init()
{
    std::cout << "Initializing Window" << std::endl;

    ServiceLocator::Provide(new CustomWindow());
}
