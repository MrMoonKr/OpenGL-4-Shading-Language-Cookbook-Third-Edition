#include "chapter12_app.h"

#include <iostream>

int main()
{
    try
    {
        AppConfig config;
        config.width = 1600;
        config.height = 900;
        config.title = "Chapter 12 - Principled BSDF Core";
        config.glMajor = 4;
        config.glMinor = 6;
        config.vsync = true;

        Chapter12App app;
        app.Init( config );
        app.Run();
    }
    catch ( const std::exception& exception )
    {
        std::cerr << "[FATAL] " << exception.what() << '\n';
        return -1;
    }

    return 0;
}
