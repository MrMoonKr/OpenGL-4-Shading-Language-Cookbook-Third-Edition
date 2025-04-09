#include "scene.h"
#include "scenerunner.h"
#include "scenebasic.h"

#include <memory>

static std::map<std::string, std::string> sceneData = { 
    { "basic", "Basic scene." } 
};

int main( int argc, char* argv[] )
{
    std::string recipe = SceneRunner::parseCLArgs(argc, argv, sceneData);
    // std::string recipe = "basic";

    SceneRunner runner( "Chapter 1 - " + recipe, 800, 800 );

    std::unique_ptr<Scene> scene;
    if ( recipe == "basic" )
    {
        scene = std::unique_ptr<Scene>( new SceneBasic() );
    }

    return runner.run( std::move( scene ) );
}
