#include "glfwWindow.h"


CustomWindow::CustomWindow()
{
    _window = nullptr;
}

void CustomWindow::OpenWindow()
{
    glfwInit();

    _window = glfwCreateWindow(800, 600, "OpiumEngine", nullptr, nullptr);
}


bool CustomWindow::Update()
{
    glfwPollEvents();

    return glfwWindowShouldClose(_window);
}