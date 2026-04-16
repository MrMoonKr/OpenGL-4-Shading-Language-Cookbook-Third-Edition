#include "scene.h"
#include "scenerunner.h"
#include "scenebasic.h"
#include "scenemenu.h"

#include <filesystem>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#endif

static std::map<std::string, std::string> sceneData = { 
    { "basic", "Basic scene." } 
};

int main( int argc, char* argv[] )
{
#ifdef _WIN32
    SetConsoleCP( CP_UTF8 );
    SetConsoleOutputCP( CP_UTF8 );
#endif

    if ( argc > 0 )
    {
        const std::filesystem::path executablePath = std::filesystem::absolute( argv[ 0 ] );
        std::filesystem::current_path( executablePath.parent_path() );
    }

    std::string recipe = SceneRunner::parseCLArgs(argc, argv, sceneData);
    // std::string recipe = "basic";

    SceneRunner runner( "Chapter 1 - " + recipe, 1280, 720 );

    std::unique_ptr<Scene> scene;
    if ( recipe == "basic" )
    {
        //scene = std::unique_ptr<Scene>( new SceneBasic() );
        scene = std::unique_ptr<Scene>( new SceneMenu() );
    }

    return runner.run( std::move( scene ) );
}
