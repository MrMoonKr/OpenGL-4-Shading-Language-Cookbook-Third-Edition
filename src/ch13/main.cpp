#include "chapter13_app.h"

#include <iostream>

int main()
{
    try
    {
        AppConfig config;
        config.width = 1600;
        config.height = 900;
        config.title = "Chapter 13 - Transmission / Subsurface / Eye";

        Chapter13App app;
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
