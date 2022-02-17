#include <iostream>
#include <Opium.h>


class OpiumApp : public Opium::Application
{
public:
    OpiumApp()
    {

    }

    ~OpiumApp()
    {

    }
};

Opium::Application* Opium::CreateApplication()
{
    return new OpiumApp;
}