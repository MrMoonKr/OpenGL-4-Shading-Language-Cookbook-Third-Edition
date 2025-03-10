#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

void error_callback(int error, const char* description) {
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
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    glfwWindowHint( GLFW_SCALE_TO_MONITOR, GL_TRUE );

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLEW with FetchContent", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent( window );
    glfwCenterWindow( window );
    
    // GLEW 초기화
    if ( glewInit() != GLEW_OK )
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString( GL_VERSION ) << std::endl ;

    glClearColor( 0.3f, 0.3f, 0.3f, 1.0f );

    while ( !glfwWindowShouldClose( window ) ) 
    {
        glClear( GL_COLOR_BUFFER_BIT );
        glfwSwapBuffers( window );
        glfwPollEvents();
    }

    glfwDestroyWindow( window );
    glfwTerminate();
    return 0;
}
