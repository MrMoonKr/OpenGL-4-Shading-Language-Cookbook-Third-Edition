#include "cookbookogl.h"
#include "scene.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_freetype.h>

#include "glutils.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <memory>

/**
 * @brief main() 함수에서 특정 씬을 실행하기 위해 사용되는 클래스.
 *
 * glfw를 초기화하고 OpenGL 컨텍스트를 생성하며, 씬을 실행하는 역할을 한다.
 */
class SceneRunner
{
private:
    GLFWwindow*     window;
    int             frameBufferWidth;
    int             frameBufferHeight;
    bool            debug; // Set true to enable debug messages

public:
    SceneRunner( const std::string& windowTitle, int width = WIN_WIDTH, int height = WIN_HEIGHT, int samples = 0 )
        : debug( true )
    {
        // Initialize GLFW
        if ( !glfwInit() )
        {
            exit( EXIT_FAILURE );
        }

#ifdef __APPLE__
        // Select OpenGL 4.1
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 1 );
#else
        // Select OpenGL 4.6
        glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
        glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
#endif

        glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
        glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
        glfwWindowHint( GLFW_RESIZABLE, GL_TRUE );

        glfwWindowHint( GLFW_SCALE_TO_MONITOR, GL_TRUE );

        if ( debug )
        {
            glfwWindowHint( GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE );
        }
        if ( samples > 0 )
        {
            glfwWindowHint( GLFW_SAMPLES, samples );
        }

        // Open the window
        window = glfwCreateWindow( width, height, windowTitle.c_str(), NULL, NULL );
        if ( !window )
        {
            std::cerr << "Unable to create OpenGL context." << std::endl;
            glfwTerminate();
            exit( EXIT_FAILURE );
        }
        glfwMakeContextCurrent( window );

        // Get framebuffer size
        glfwGetFramebufferSize( window, &frameBufferWidth, &frameBufferHeight );

        // Load the OpenGL functions.
        // if(!gladLoadGL()) { exit(-1); }
        if ( 0 == gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress ) )
        {
            std::cout << "failed to initialize GLAD " << std::endl;
            exit( -1 );
        }

        GLUtils::dumpGLInfo();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // ImGuiIO& io = ImGui::GetIO();
        // io.Fonts->AddFontDefault();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL( window, true );
        ImGui_ImplOpenGL3_Init( "#version 410" );
        
        // Initialization
        glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

#ifndef __APPLE__
        if ( debug )
        {
            glDebugMessageCallback( GLUtils::debugCallback, nullptr );
            glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE );
            glDebugMessageInsert( GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 0, GL_DEBUG_SEVERITY_NOTIFICATION,
                                  -1, "Start debugging" );
        }
#endif
    }

    int run( std::unique_ptr<Scene> scene )
    {
        glfwSetWindowUserPointer( window, scene.get() );
        glfwSetWindowSizeCallback( window, []( GLFWwindow* window, int width, int height ) {
            glViewport( 0, 0, width, height );
            Scene* scene = ( Scene* )glfwGetWindowUserPointer( window );
            scene->setDimensions( width, height );
            scene->resize( width, height );
        } );

        // Enter the main loop
        mainLoop( window, std::move( scene ) );

#ifndef __APPLE__
        if ( debug )
        {
            glDebugMessageInsert( GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, 1, GL_DEBUG_SEVERITY_NOTIFICATION,
                                  -1, "End debug" );
        }
#endif

        // Close window and terminate GLFW
        glfwTerminate();

        // Exit program
        return EXIT_SUCCESS;
    }

    static std::string parseCLArgs( int argc, char** argv, std::map<std::string, std::string>& sceneData )
    {
        if ( argc < 2 )
        {
            printHelpInfo( argv[ 0 ], sceneData );
            //exit( EXIT_FAILURE );
            printf( "No recipe name provided. So just using first : %s\n", sceneData.begin()->first.c_str() );
            return sceneData.begin()->first;
        }

        std::string recipeName = argv[ 1 ];
        auto it = sceneData.find( recipeName );
        if ( it == sceneData.end() )
        {
            printf( "Unknown recipe: %s\n\n", recipeName.c_str() );
            printHelpInfo( argv[ 0 ], sceneData );
            exit( EXIT_FAILURE );
        }

        return recipeName;
    }

  private:
    static void printHelpInfo( const char* exeFile, std::map<std::string, std::string>& sceneData )
    {
        printf( "Usage: %s recipe-name\n\n", exeFile );
        printf( "Recipe names: \n" );
        for ( auto it : sceneData )
        {
            printf( "  %s : %s\n", it.first.c_str(), it.second.c_str() );
        }
    }

    void mainLoop( GLFWwindow* window, std::unique_ptr<Scene> scene )
    {
        scene->setDimensions( frameBufferWidth, frameBufferHeight );
        scene->initScene();
        scene->resize( frameBufferWidth, frameBufferHeight );

        while ( !glfwWindowShouldClose( window ) && !glfwGetKey( window, GLFW_KEY_ESCAPE ) )
        {
            GLUtils::checkForOpenGLError( __FILE__, __LINE__ );

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            scene->update( float( glfwGetTime() ) );
            scene->render();

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

            glfwSwapBuffers( window );

            glfwPollEvents();
            int state = glfwGetKey( window, GLFW_KEY_SPACE );
            if ( state == GLFW_PRESS )
            {
                scene->animate( !scene->animating() );
            }
        }
    }
};
