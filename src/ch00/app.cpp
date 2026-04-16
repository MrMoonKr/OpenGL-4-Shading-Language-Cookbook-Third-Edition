#include "app.h"

#include <stdexcept>
#include <iostream>

AppWindow::~AppWindow()
{
    Shutdown();
}

void AppWindow::Init( const AppConfig& config )
{
    if ( m_window )
        throw std::runtime_error( "AppWindow::Init() already called" );

    glfwSetErrorCallback( ErrorCallback );

    if ( !glfwInit() )
        throw std::runtime_error( "glfwInit() failed" );

    m_glfwInitialized = true;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, config.glMajor );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, config.glMinor );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    glfwWindowHint( GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE );
    glfwWindowHint( GLFW_VISIBLE, config.visible ? GLFW_TRUE : GLFW_FALSE );
    glfwWindowHint( GLFW_DECORATED, config.decorated ? GLFW_TRUE : GLFW_FALSE );
    glfwWindowHint( GLFW_MAXIMIZED, config.maximized ? GLFW_TRUE : GLFW_FALSE );

#ifdef GLFW_SCALE_TO_MONITOR
    glfwWindowHint( GLFW_SCALE_TO_MONITOR, config.scaleToMonitor ? GLFW_TRUE : GLFW_FALSE );
#endif
#ifdef __APPLE__
#ifdef GLFW_COCOA_RETINA_FRAMEBUFFER
    glfwWindowHint( GLFW_COCOA_RETINA_FRAMEBUFFER, config.scaleToMonitor ? GLFW_TRUE : GLFW_FALSE );
#endif
#endif

    m_window = glfwCreateWindow( config.width, config.height, config.title, nullptr, nullptr );

    if ( !m_window )
    {
        Shutdown();
        throw std::runtime_error( "glfwCreateWindow() failed" );
    }

    glfwMakeContextCurrent( m_window );
    glfwSwapInterval( config.vsync ? 1 : 0 );

    glfwGetWindowSize( m_window, &m_width, &m_height );

    glfwSetWindowUserPointer( m_window, this );

    // ===== Window callbacks =====
    glfwSetWindowPosCallback( m_window, WindowPosCallback );
    glfwSetWindowSizeCallback( m_window, WindowSizeCallback );
    glfwSetWindowCloseCallback( m_window, WindowCloseCallback );
    glfwSetWindowRefreshCallback( m_window, WindowRefreshCallback );
    glfwSetWindowFocusCallback( m_window, WindowFocusCallback );
    glfwSetWindowIconifyCallback( m_window, WindowIconifyCallback );
    glfwSetWindowMaximizeCallback( m_window, WindowMaximizeCallback );
    glfwSetFramebufferSizeCallback( m_window, FramebufferSizeCallback );
    glfwSetWindowContentScaleCallback( m_window, WindowContentScaleCallback );

    // ===== Input callbacks =====
    glfwSetKeyCallback( m_window, KeyCallback );
    glfwSetCharCallback( m_window, CharCallback );
    glfwSetMouseButtonCallback( m_window, MouseButtonCallback );
    glfwSetCursorPosCallback( m_window, CursorPosCallback );
    glfwSetCursorEnterCallback( m_window, CursorEnterCallback );
    glfwSetScrollCallback( m_window, ScrollCallback );
    glfwSetDropCallback( m_window, DropCallback );

    // GLEW 초기화
    glewExperimental = GL_TRUE;
    if ( glewInit() != GLEW_OK )
    {
        throw std::runtime_error( "glewInit() failed" );
    }

    std::cout << "OpenGL Vendor : " << glGetString( GL_VENDOR ) << std::endl;
    std::cout << "OpenGL Version : " << glGetString( GL_VERSION ) << std::endl;
    std::cout << "OpenGL Renderer : " << glGetString( GL_RENDERER ) << std::endl;

    std::cout << "GLSL Version ( string ) : " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;
    GLint major , minor;
    glGetIntegerv( GL_MAJOR_VERSION, &major );
    glGetIntegerv( GL_MINOR_VERSION, &minor );
    std::cout << "GLSL Version ( integer ) : " << major << "."  << minor << std::endl;

    glfwGetWindowContentScale( m_window, &m_contentScaleX, &m_contentScaleY );
    OnWindowContentScale( m_contentScaleX, m_contentScaleY );

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize( m_window, &framebufferWidth, &framebufferHeight );
    OnFramebufferSize( framebufferWidth, framebufferHeight );

    glfwCenterWindow( m_window );

    m_lastTime = glfwGetTime();

    OnInit();
}

void AppWindow::Run()
{
    if ( !m_window )
        throw std::runtime_error( "AppWindow::Run() called before Init()" );

    while ( !glfwWindowShouldClose( m_window ) )
    {
        const double currentTime = glfwGetTime();
        const float dt = static_cast<float>( currentTime - m_lastTime );
        m_lastTime = currentTime;

        OnBeforePollEvents();
        glfwPollEvents();
        OnAfterPollEvents();

        OnUpdate( dt );
        OnRender();

        glfwSwapBuffers( m_window );
    }

    OnShutdown();
}

void AppWindow::Shutdown()
{
    if ( m_window )
    {
        glfwDestroyWindow( m_window );
        m_window = nullptr;
    }

    if ( m_glfwInitialized )
    {
        glfwTerminate();
        m_glfwInitialized = false;
    }
}

AppWindow* AppWindow::GetThis( GLFWwindow* window )
{
    return static_cast<AppWindow*>( glfwGetWindowUserPointer( window ) );
}

void AppWindow::glfwCenterWindow( GLFWwindow* window ) 
{
    int windowWidth, windowHeight;
    glfwGetWindowSize( window, &windowWidth, &windowHeight );

    const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );

    glfwSetWindowPos( window, ( mode->width - windowWidth ) / 2, ( mode->height - windowHeight ) / 2 );
}

void AppWindow::ErrorCallback( int error, const char* description )
{
    std::cerr << "[GLFW ERROR] (" << error << ") " << ( description ? description : "unknown" ) << std::endl;
}

void AppWindow::WindowPosCallback( GLFWwindow* window, int xpos, int ypos )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowPos( xpos, ypos );
}

void AppWindow::WindowSizeCallback( GLFWwindow* window, int width, int height )
{
    if ( auto* self = GetThis( window ) )
    {
        self->m_width = width;
        self->m_height = height;
        self->OnWindowSize( width, height );
    }
}

void AppWindow::WindowCloseCallback( GLFWwindow* window )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowClose();
}

void AppWindow::WindowRefreshCallback( GLFWwindow* window )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowRefresh();
}

void AppWindow::WindowFocusCallback( GLFWwindow* window, int focused )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowFocus( focused );
}

void AppWindow::WindowIconifyCallback( GLFWwindow* window, int iconified )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowIconify( iconified );
}

void AppWindow::WindowMaximizeCallback( GLFWwindow* window, int maximized )
{
    if ( auto* self = GetThis( window ) )
        self->OnWindowMaximize( maximized );
}

void AppWindow::FramebufferSizeCallback( GLFWwindow* window, int width, int height )
{
    if ( auto* self = GetThis( window ) )
        self->OnFramebufferSize( width, height );
}

void AppWindow::WindowContentScaleCallback( GLFWwindow* window, float xscale, float yscale )
{
    if ( auto* self = GetThis( window ) )
    {
        self->m_contentScaleX = xscale;
        self->m_contentScaleY = yscale;
        self->OnWindowContentScale( xscale, yscale );
    }
}

void AppWindow::KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
    if ( auto* self = GetThis( window ) )
        self->OnKey( key, scancode, action, mods );
}

void AppWindow::CharCallback( GLFWwindow* window, unsigned int codepoint )
{
    if ( auto* self = GetThis( window ) )
        self->OnChar( codepoint );
}

void AppWindow::MouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
    if ( auto* self = GetThis( window ) )
        self->OnMouseButton( button, action, mods );
}

void AppWindow::CursorPosCallback( GLFWwindow* window, double xpos, double ypos )
{
    if ( auto* self = GetThis( window ) )
        self->OnCursorPos( xpos, ypos );
}

void AppWindow::CursorEnterCallback( GLFWwindow* window, int entered )
{
    if ( auto* self = GetThis( window ) )
        self->OnCursorEnter( entered );
}

void AppWindow::ScrollCallback( GLFWwindow* window, double xoffset, double yoffset )
{
    if ( auto* self = GetThis( window ) )
        self->OnScroll( xoffset, yoffset );
}

void AppWindow::DropCallback( GLFWwindow* window, int pathCount, const char* paths[] )
{
    if ( auto* self = GetThis( window ) )
        self->OnDrop( pathCount, paths );
}


