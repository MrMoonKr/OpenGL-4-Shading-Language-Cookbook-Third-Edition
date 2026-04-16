#pragma once

#include <GL/glew.h> // always should be before glfw
#include <GLFW/glfw3.h>

struct AppConfig
{
    int width = 1280;
    int height = 720;
    const char* title = "App";

    int glMajor = 4;
    int glMinor = 6;

    bool resizable = true;
    bool visible = true;
    bool decorated = true;
    bool maximized = false;
    bool vsync = true;
    bool scaleToMonitor = true;
};

class AppWindow
{
  public:
    AppWindow() = default;
    virtual ~AppWindow();

    void Init( const AppConfig& config );
    void Run();
    void Shutdown();

    GLFWwindow* GetNativeHandle() const
    {
        return m_window;
    }
    int GetWidth() const
    {
        return m_width;
    }
    int GetHeight() const
    {
        return m_height;
    }
    float GetContentScaleX() const
    {
        return m_contentScaleX;
    }
    float GetContentScaleY() const
    {
        return m_contentScaleY;
    }

  protected:

    virtual void OnInit()
    {
    }
    virtual void OnUpdate( float dt )
    {
    }
    virtual void OnRender()
    {
    }
    virtual void OnShutdown()
    {
    }

    // ===== Window callbacks =====
    virtual void OnWindowPos( int xpos, int ypos )
    {
    }
    virtual void OnWindowSize( int width, int height )
    {
    }
    virtual void OnWindowClose()
    {
    }
    virtual void OnWindowRefresh()
    {
    }
    virtual void OnWindowFocus( int focused )
    {
    }
    virtual void OnWindowIconify( int iconified )
    {
    }
    virtual void OnWindowMaximize( int maximized )
    {
    }
    virtual void OnFramebufferSize( int width, int height )
    {
    }
    virtual void OnWindowContentScale( float xscale, float yscale )
    {
    }

    // ===== Input callbacks =====
    virtual void OnKey( int key, int scancode, int action, int mods )
    {
    }
    virtual void OnChar( unsigned int codepoint )
    {
    }
    virtual void OnMouseButton( int button, int action, int mods )
    {
    }
    virtual void OnCursorPos( double xpos, double ypos )
    {
    }
    virtual void OnCursorEnter( int entered )
    {
    }
    virtual void OnScroll( double xoffset, double yoffset )
    {
    }
    virtual void OnDrop( int pathCount, const char* const* paths )
    {
    }

    // ===== Optional per-frame hook =====
    virtual void OnBeforePollEvents()
    {
    }
    virtual void OnAfterPollEvents()
    {
    }

  protected:
    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;
    float m_contentScaleX = 1.0f;
    float m_contentScaleY = 1.0f;

  private:
    double m_lastTime = 0.0;
    bool m_glfwInitialized = false;

  private:
    static void ErrorCallback( int error, const char* description );

    static void WindowPosCallback( GLFWwindow* window, int xpos, int ypos );
    static void WindowSizeCallback( GLFWwindow* window, int width, int height );
    static void WindowCloseCallback( GLFWwindow* window );
    static void WindowRefreshCallback( GLFWwindow* window );
    static void WindowFocusCallback( GLFWwindow* window, int focused );
    static void WindowIconifyCallback( GLFWwindow* window, int iconified );
    static void WindowMaximizeCallback( GLFWwindow* window, int maximized );
    static void FramebufferSizeCallback( GLFWwindow* window, int width, int height );
    static void WindowContentScaleCallback( GLFWwindow* window, float xscale, float yscale );

    static void KeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods );
    static void CharCallback( GLFWwindow* window, unsigned int codepoint );

    static void MouseButtonCallback( GLFWwindow* window, int button, int action, int mods );
    static void CursorPosCallback( GLFWwindow* window, double xpos, double ypos );
    static void CursorEnterCallback( GLFWwindow* window, int entered );
    static void ScrollCallback( GLFWwindow* window, double xoffset, double yoffset );
    
    static void DropCallback( GLFWwindow* window, int pathCount, const char* paths[] );

  private:
    static AppWindow* GetThis( GLFWwindow* window );
    static void glfwCenterWindow( GLFWwindow* window );
};
