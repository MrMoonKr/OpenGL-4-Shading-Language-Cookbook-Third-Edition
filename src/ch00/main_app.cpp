#include "app.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

class GLApp : public AppWindow
{
private:
    float m_elapsedTime = 0.0f;

protected:
    void OnInit() override
    {
        std::cout << "OnInit\n";
        glClearColor( 0.16f, 0.22f, 0.30f, 1.0f );
    }

    void OnUpdate( float dt ) override
    {
        m_elapsedTime += dt;

        const float red = 0.5f + 0.5f * std::sin( m_elapsedTime * 0.55f );
        const float green = 0.5f + 0.5f * std::sin( m_elapsedTime * 0.55f + 2.094f );
        const float blue = 0.5f + 0.5f * std::sin( m_elapsedTime * 0.55f + 4.188f );

        glClearColor( red, green, blue, 1.0f );
    }

    void OnRender() override
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OnKey(int key, int scancode, int action, int mods) override
    {
        (void)scancode;
        (void)mods;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(GetNativeHandle(), GLFW_TRUE);
    }

    void OnFramebufferSize(int width, int height) override
    {
        glViewport(0, 0, width, height);
        std::cout << "FramebufferSize: " << width << ", " << height << "\n";
    }

    void OnDrop(int pathCount, const char* const* paths) override
    {
        for (int i = 0; i < pathCount; ++i)
            std::cout << "Dropped: " << paths[i] << "\n";
    }
};


int main()
{
    try
    {
        AppConfig config;
        config.width = 1280;
        config.height = 720;
        config.title = "GLFW AppWindow";
        config.glMajor = 4;
        config.glMinor = 6;
        config.vsync = true;

        GLApp app;
        app.Init(config);
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "[FATAL] " << e.what() << '\n';
        return -1;
    }

    return 0;
}
