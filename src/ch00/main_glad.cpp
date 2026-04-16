#include <iostream>

#include <glad/gl.h>        // Library for handling the loading of OpenGL functions, must be included before GLFW
#include <GLFW/glfw3.h>     // Library for handling window and user input


void error_callback( int error, const char* description ) 
{
    std::cerr << "Error: " << description << std::endl;
}

void glfwCenterWindow( GLFWwindow* window ) 
{
    int windowWidth, windowHeight;
    glfwGetWindowSize( window, &windowWidth, &windowHeight );

    const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );

    glfwSetWindowPos( window, ( mode->width - windowWidth ) / 2, ( mode->height - windowHeight ) / 2 );
}

int main() 
{
    if ( !glfwInit() ) 
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwSetErrorCallback( error_callback );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 4 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 6 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GL_TRUE );

    GLFWwindow* window = glfwCreateWindow( 1280, 720, "GLFW with GLAD using cmake", NULL, NULL );
    if ( !window )
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent( window );
    glfwCenterWindow( window );

    // GLAD 초기화
    const int version = gladLoadGL( glfwGetProcAddress );
    if ( 0 == version )
    {
        std::cout << "[에러] failed to initialize GLAD " << std::endl;
        return -1;
    }
    std::cout << "Loaded OpenGL " << GLAD_VERSION_MAJOR( version ) << "." << GLAD_VERSION_MINOR( version ) << std::endl;
    
    std::cout << "OpenGL Vendor : " << glGetString( GL_VENDOR ) << std::endl ;
    std::cout << "OpenGL Version : " << glGetString( GL_VERSION ) << std::endl ;

    glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

    while ( !glfwWindowShouldClose( window ) ) 
    {
        glfwPollEvents();

        glClear( GL_COLOR_BUFFER_BIT );

        // update and draw coding heare

        glfwSwapBuffers( window );
    }

    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}
