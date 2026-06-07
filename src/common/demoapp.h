#pragma once

#include "appwindow.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <string>

class DemoApp : public AppWindow
{
  public:
    explicit DemoApp( std::string assetFolder );
    virtual ~DemoApp() = default;

    std::filesystem::path resolveAssetPath( const std::string& relativePath ) const;
    glm::ivec2 framebufferSize() const
    {
        return m_framebufferSize;
    }
    float elapsedSeconds() const
    {
        return m_elapsedSeconds;
    }
    bool uiCapturesMouse() const;
    bool uiCapturesKeyboard() const;
    bool showUi() const
    {
        return m_showUi;
    }

  protected:
    virtual void setupDemo() = 0;
    virtual void updateDemo( float dt ) = 0;
    virtual void renderDemo() = 0;
    virtual void renderDemoUi() = 0;
    virtual void shutdownDemo()
    {
    }

    virtual void handleKey( int key, int scancode, int action, int mods );
    virtual void handleMouseButton( int button, int action, int mods );
    virtual void handleCursorPos( double xpos, double ypos );
    virtual void handleScroll( double xoffset, double yoffset );

  private:
    void OnInit() override;
    void OnUpdate( float dt ) override;
    void OnRender() override;
    void OnShutdown() override;
    void OnFramebufferSize( int width, int height ) override;
    void OnWindowFocus( int focused ) override;
    void OnMouseButton( int button, int action, int mods ) override;
    void OnCursorPos( double xpos, double ypos ) override;
    void OnScroll( double xoffset, double yoffset ) override;
    void OnKey( int key, int scancode, int action, int mods ) override;
    void OnChar( unsigned int codepoint ) override;
    void OnCursorEnter( int entered ) override;

  private:
    std::string m_assetFolder;
    glm::ivec2 m_framebufferSize = glm::ivec2( 1280, 720 );
    float m_elapsedSeconds = 0.0f;
    bool m_showUi = true;
};
