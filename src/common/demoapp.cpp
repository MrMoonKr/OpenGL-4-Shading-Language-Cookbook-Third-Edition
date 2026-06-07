#include "demoapp.h"

#include "glutils.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <iostream>

namespace
{
std::filesystem::path resolvePathCandidate( const std::filesystem::path& path )
{
    if ( std::filesystem::exists( path ) )
    {
        return path;
    }
    return {};
}
} // namespace

DemoApp::DemoApp( std::string assetFolder ) : m_assetFolder( std::move( assetFolder ) )
{
}

std::filesystem::path DemoApp::resolveAssetPath( const std::string& relativePath ) const
{
    const std::filesystem::path directPath = resolvePathCandidate( relativePath );
    if ( !directPath.empty() )
    {
        return directPath;
    }

    std::filesystem::path searchBase = std::filesystem::current_path();
    for ( int depth = 0; depth < 8; ++depth )
    {
        const std::filesystem::path candidate = resolvePathCandidate( searchBase / relativePath );
        if ( !candidate.empty() )
        {
            return candidate;
        }

        if ( !searchBase.has_parent_path() )
        {
            break;
        }
        searchBase = searchBase.parent_path();
    }

    const std::filesystem::path localPath = resolvePathCandidate( std::filesystem::path( relativePath ) );
    if ( !localPath.empty() )
    {
        return localPath;
    }

    const std::filesystem::path sourceTreePath = std::filesystem::path( "src" ) / m_assetFolder / relativePath;
    const std::filesystem::path resolvedSource = resolvePathCandidate( sourceTreePath );
    if ( !resolvedSource.empty() )
    {
        return resolvedSource;
    }

    searchBase = std::filesystem::current_path();
    for ( int depth = 0; depth < 8; ++depth )
    {
        const std::filesystem::path sourceCandidate =
            resolvePathCandidate( searchBase / "src" / m_assetFolder / relativePath );
        if ( !sourceCandidate.empty() )
        {
            return sourceCandidate;
        }

        if ( !searchBase.has_parent_path() )
        {
            break;
        }
        searchBase = searchBase.parent_path();
    }

    return relativePath;
}

bool DemoApp::uiCapturesMouse() const
{
    return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse;
}

bool DemoApp::uiCapturesKeyboard() const
{
    return ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard;
}

void DemoApp::OnInit()
{
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_CULL_FACE );
    glEnable( GL_TEXTURE_CUBE_MAP_SEAMLESS );

#ifndef __APPLE__
    glEnable( GL_DEBUG_OUTPUT );
    glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
    glDebugMessageCallback( GLUtils::debugCallback, nullptr );
#endif

    GLUtils::dumpGLInfo();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL( GetNativeHandle(), false );
    ImGui_ImplOpenGL3_Init( "#version 460" );

    const std::filesystem::path fontPath = resolveAssetPath( "fonts/D2Coding-Ver1.3.2-20180524.ttf" );
    if ( std::filesystem::exists( fontPath ) )
    {
        ImGui::GetIO().Fonts->AddFontFromFileTTF( fontPath.string().c_str(), 18.0f, nullptr,
                                                  ImGui::GetIO().Fonts->GetGlyphRangesKorean() );
    }

    setupDemo();
}

void DemoApp::OnUpdate( float dt )
{
    m_elapsedSeconds += dt;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    updateDemo( dt );
}

void DemoApp::OnRender()
{
    renderDemo();

    if ( m_showUi )
    {
        renderDemoUi();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
}

void DemoApp::OnShutdown()
{
    shutdownDemo();

    if ( ImGui::GetCurrentContext() != nullptr )
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}

void DemoApp::OnFramebufferSize( int width, int height )
{
    m_framebufferSize = glm::ivec2( width, height );
    glViewport( 0, 0, width, height );
}

void DemoApp::OnWindowFocus( int focused )
{
    ImGui_ImplGlfw_WindowFocusCallback( GetNativeHandle(), focused );
}

void DemoApp::OnMouseButton( int button, int action, int mods )
{
    ImGui_ImplGlfw_MouseButtonCallback( GetNativeHandle(), button, action, mods );
    handleMouseButton( button, action, mods );
}

void DemoApp::OnCursorPos( double xpos, double ypos )
{
    ImGui_ImplGlfw_CursorPosCallback( GetNativeHandle(), xpos, ypos );
    handleCursorPos( xpos, ypos );
}

void DemoApp::OnScroll( double xoffset, double yoffset )
{
    ImGui_ImplGlfw_ScrollCallback( GetNativeHandle(), xoffset, yoffset );
    handleScroll( xoffset, yoffset );
}

void DemoApp::OnKey( int key, int scancode, int action, int mods )
{
    ImGui_ImplGlfw_KeyCallback( GetNativeHandle(), key, scancode, action, mods );

    if ( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
    {
        glfwSetWindowShouldClose( GetNativeHandle(), GLFW_TRUE );
        return;
    }

    if ( key == GLFW_KEY_TAB && action == GLFW_PRESS )
    {
        m_showUi = !m_showUi;
    }

    handleKey( key, scancode, action, mods );
}

void DemoApp::OnChar( unsigned int codepoint )
{
    ImGui_ImplGlfw_CharCallback( GetNativeHandle(), codepoint );
}

void DemoApp::OnCursorEnter( int entered )
{
    ImGui_ImplGlfw_CursorEnterCallback( GetNativeHandle(), entered );
}

void DemoApp::handleKey( int key, int scancode, int action, int mods )
{
    (void)key;
    (void)scancode;
    (void)action;
    (void)mods;
}

void DemoApp::handleMouseButton( int button, int action, int mods )
{
    (void)button;
    (void)action;
    (void)mods;
}

void DemoApp::handleCursorPos( double xpos, double ypos )
{
    (void)xpos;
    (void)ypos;
}

void DemoApp::handleScroll( double xoffset, double yoffset )
{
    (void)xoffset;
    (void)yoffset;
}
