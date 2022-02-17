#include <iostream>
#include <OpiumEngine/engine.h>
#include <glm/glm.hpp>
#include <OpiumEngine/serviceLocator.h>

int main(int argc, char** argv)
{
    std::cout << "Hello From App" << std::endl;
    OpiumEngine::Init();


    ServiceLocator::GetWindow()->OpenWindow();

    while(!ServiceLocator::GetWindow()->Update())
    {

    }
}