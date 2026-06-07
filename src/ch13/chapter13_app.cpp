#include "chapter13_app.h"

#include "procedural.h"

#include <glm/ext/matrix_transform.hpp>

#include <imgui.h>

namespace
{
void deleteTexture( GLuint& texture )
{
    if ( texture != 0 )
    {
        glDeleteTextures( 1, &texture );
        texture = 0;
    }
}
} // namespace

Chapter13App::Chapter13App() : DemoApp( "ch13" )
{
}

void Chapter13App::setupDemo()
{
    m_ground = std::make_unique<Plane>( 16.0f, 16.0f, 1, 1, 8.0f, 8.0f );
    m_cube = std::make_unique<Cube>( 1.8f );
    m_sphere = std::make_unique<Sphere>( 1.0f, 64, 32 );

    m_backgroundProgram.compileShader( resolveAssetPath( "shader/background.vert.glsl" ).string().c_str() );
    m_backgroundProgram.compileShader( resolveAssetPath( "shader/background.frag.glsl" ).string().c_str() );
    m_backgroundProgram.link();

    m_presentProgram.compileShader( resolveAssetPath( "shader/present.vert.glsl" ).string().c_str() );
    m_presentProgram.compileShader( resolveAssetPath( "shader/present.frag.glsl" ).string().c_str() );
    m_presentProgram.link();

    m_materialProgram.compileShader( resolveAssetPath( "shader/ch13.vert.glsl" ).string().c_str() );
    m_materialProgram.compileShader( resolveAssetPath( "shader/ch13.frag.glsl" ).string().c_str() );
    m_materialProgram.link();

    m_environmentMap = Procedural::createGradientCubemap( 128 );
    m_brdfLut = Procedural::createBrdfLutTexture( 128 );
    m_baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.77f, 0.84f, 0.88f ),
                                                               glm::vec3( 0.15f, 0.18f, 0.22f ) );
    m_normalTexture = Procedural::createWaveNormalTexture( 128, 0.14f );
    m_materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.22f, 0.0f );
    m_maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    m_thicknessTexture = Procedural::createThicknessTexture( 128 );
    m_irisTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.12f, 0.42f, 0.68f ),
                                                          glm::vec3( 0.04f, 0.10f, 0.15f ) );
    m_scleraTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.91f, 0.88f, 0.84f ),
                                                            glm::vec3( 0.74f, 0.70f, 0.66f ) );

    createFullscreenQuad();
    recreateOffscreenTargets();
}

void Chapter13App::recreateOffscreenTargets()
{
    if ( framebufferSize().x <= 0 || framebufferSize().y <= 0 )
    {
        return;
    }

    if ( m_sceneExtent == framebufferSize() && m_sceneFbo != 0 )
    {
        return;
    }

    releaseOffscreenTargets();
    m_sceneExtent = framebufferSize();

    glGenFramebuffers( 1, &m_sceneFbo );
    glBindFramebuffer( GL_FRAMEBUFFER, m_sceneFbo );

    glGenTextures( 1, &m_sceneColor );
    glBindTexture( GL_TEXTURE_2D, m_sceneColor );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_RGBA16F, m_sceneExtent.x, m_sceneExtent.y );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_sceneColor, 0 );

    glGenTextures( 1, &m_sceneDepth );
    glBindTexture( GL_TEXTURE_2D, m_sceneDepth );
    glTexStorage2D( GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, m_sceneExtent.x, m_sceneExtent.y );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_sceneDepth, 0 );

    const GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, drawBuffers );
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Chapter13App::releaseOffscreenTargets()
{
    deleteTexture( m_sceneColor );
    deleteTexture( m_sceneDepth );
    if ( m_sceneFbo != 0 )
    {
        glDeleteFramebuffers( 1, &m_sceneFbo );
        m_sceneFbo = 0;
    }
}

void Chapter13App::createFullscreenQuad()
{
    const float vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
    };

    glGenVertexArrays( 1, &m_quadVao );
    glGenBuffers( 1, &m_quadVbo );
    glBindVertexArray( m_quadVao );
    glBindBuffer( GL_ARRAY_BUFFER, m_quadVbo );
    glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_STATIC_DRAW );
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 4, reinterpret_cast<void*>( 0 ) );
    glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof( float ) * 4, reinterpret_cast<void*>( sizeof( float ) * 2 ) );
    glEnableVertexAttribArray( 0 );
    glEnableVertexAttribArray( 1 );
    glBindVertexArray( 0 );
}

void Chapter13App::releaseFullscreenQuad()
{
    if ( m_quadVbo != 0 )
    {
        glDeleteBuffers( 1, &m_quadVbo );
        m_quadVbo = 0;
    }
    if ( m_quadVao != 0 )
    {
        glDeleteVertexArrays( 1, &m_quadVao );
        m_quadVao = 0;
    }
}

void Chapter13App::updateDemo( float dt )
{
    (void)dt;
    recreateOffscreenTargets();
}

void Chapter13App::renderBackgroundPass( const glm::mat4& view, const glm::mat4& projection )
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_sceneFbo );
    glViewport( 0, 0, m_sceneExtent.x, m_sceneExtent.y );
    glClearColor( 0.03f, 0.05f, 0.07f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    m_backgroundProgram.use();
    m_backgroundProgram.setUniform( "uView", view );
    m_backgroundProgram.setUniform( "uProjection", projection );
    m_backgroundProgram.setUniform( "uLightDirection", glm::normalize( glm::vec3( -0.4f, -1.0f, -0.25f ) ) );

    glm::mat4 groundModel = glm::mat4( 1.0f );
    m_backgroundProgram.setUniform( "uModel", groundModel );
    m_backgroundProgram.setUniform( "uBaseColor", glm::vec3( 0.18f, 0.19f, 0.22f ) );
    m_ground->render();

    glm::mat4 cubeModel = glm::translate( glm::mat4( 1.0f ), glm::vec3( -2.2f, 0.9f, -0.8f ) );
    m_backgroundProgram.setUniform( "uModel", cubeModel );
    m_backgroundProgram.setUniform( "uBaseColor", glm::vec3( 0.88f, 0.52f, 0.12f ) );
    m_cube->render();

    cubeModel = glm::translate( glm::mat4( 1.0f ), glm::vec3( 2.4f, 0.75f, 0.85f ) ) *
                glm::scale( glm::mat4( 1.0f ), glm::vec3( 0.8f ) );
    m_backgroundProgram.setUniform( "uModel", cubeModel );
    m_backgroundProgram.setUniform( "uBaseColor", glm::vec3( 0.18f, 0.56f, 0.82f ) );
    m_cube->render();

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Chapter13App::presentBackground()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    glViewport( 0, 0, framebufferSize().x, framebufferSize().y );
    glDisable( GL_DEPTH_TEST );

    m_presentProgram.use();
    m_presentProgram.setUniform( "uColorTexture", 0 );
    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_sceneColor );
    glBindVertexArray( m_quadVao );
    glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
    glBindVertexArray( 0 );

    glEnable( GL_DEPTH_TEST );
}

void Chapter13App::renderForegroundObject( const glm::mat4& view, const glm::mat4& projection )
{
    m_materialProgram.use();
    m_materialProgram.setUniform( "uView", view );
    m_materialProgram.setUniform( "uProjection", projection );
    m_materialProgram.setUniform( "uModel", glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 1.05f, 0.0f ) ) );
    m_materialProgram.setUniform( "uCameraPos", m_camera.position() );
    m_materialProgram.setUniform( "uLightDirection", glm::normalize( glm::vec3( -0.35f, -0.95f, -0.15f ) ) );
    m_materialProgram.setUniform( "uLightColor", glm::vec3( 11.0f, 10.3f, 9.7f ) );
    m_materialProgram.setUniform( "uEnvironmentMap", 8 );
    m_materialProgram.setUniform( "uBrdfLut", 9 );
    m_materialProgram.setUniform( "uSceneColorTexture", 10 );
    m_materialProgram.setUniform( "uBaseColorTexture", 0 );
    m_materialProgram.setUniform( "uNormalTexture", 1 );
    m_materialProgram.setUniform( "uMaterialTexture", 2 );
    m_materialProgram.setUniform( "uMaskTexture", 3 );
    m_materialProgram.setUniform( "uThicknessTexture", 4 );
    m_materialProgram.setUniform( "uIrisTexture", 5 );
    m_materialProgram.setUniform( "uScleraTexture", 6 );
    m_materialProgram.setUniform( "uViewportSize", glm::vec2( framebufferSize() ) );
    m_materialProgram.setUniform( "uRefractionScale", m_refractionScale );
    m_materialProgram.setUniform( "uThicknessScale", m_thicknessScale );
    m_materialProgram.setUniform( "uSubsurfaceStrength", m_subsurfaceStrength );
    m_materialProgram.setUniform( "uSubsurfaceMix", m_subsurfaceMix );
    m_materialProgram.setUniform( "uIrisDepthScale", m_irisDepthScale );
    m_materialProgram.setUniform( "uAbsorptionCoeff", m_absorptionCoeff );
    m_materialProgram.setUniform( "uMaterialModel", static_cast<int>( m_materialModel ) );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_baseColorTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_normalTexture );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_materialTexture );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, m_maskTexture );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, m_thicknessTexture );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, m_irisTexture );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, m_scleraTexture );
    glActiveTexture( GL_TEXTURE8 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_environmentMap );
    glActiveTexture( GL_TEXTURE9 );
    glBindTexture( GL_TEXTURE_2D, m_brdfLut );
    glActiveTexture( GL_TEXTURE10 );
    glBindTexture( GL_TEXTURE_2D, m_sceneColor );

    m_sphere->render();
}

void Chapter13App::renderDemo()
{
    const glm::mat4 view = m_camera.viewMatrix();
    const glm::mat4 projection =
        m_camera.projectionMatrix( static_cast<float>( framebufferSize().x ) / static_cast<float>( framebufferSize().y ) );

    renderBackgroundPass( view, projection );

    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    presentBackground();
    renderForegroundObject( view, projection );
}

void Chapter13App::renderDemoUi()
{
    ImGui::SetNextWindowPos( ImVec2( 24.0f, 24.0f ), ImGuiCond_Once );
    ImGui::SetNextWindowSize( ImVec2( 440.0f, 420.0f ), ImGuiCond_Once );
    if ( ImGui::Begin( "Chapter 13 - Special Surface Models" ) )
    {
        static const char* labels[] = {
            "Transmission",
            "Skin",
            "Eye",
        };
        int model = static_cast<int>( m_materialModel );
        if ( ImGui::Combo( "Material Model", &model, labels, IM_ARRAYSIZE( labels ) ) )
        {
            m_materialModel = static_cast<MaterialModel>( model );
        }

        ImGui::SliderFloat( "Refraction Scale", &m_refractionScale, 0.0f, 0.20f );
        ImGui::SliderFloat( "Thickness Scale", &m_thicknessScale, 0.1f, 2.5f );
        ImGui::SliderFloat( "Subsurface Strength", &m_subsurfaceStrength, 0.0f, 1.5f );
        ImGui::SliderFloat( "Subsurface Mix", &m_subsurfaceMix, 0.0f, 1.0f );
        ImGui::SliderFloat( "Iris Depth Scale", &m_irisDepthScale, 0.0f, 0.12f );
        ImGui::ColorEdit3( "Absorption Coeff", &m_absorptionCoeff.x );
        ImGui::Separator();
        ImGui::TextWrapped( "Transmission uses screen-space refraction plus cubemap fallback. Skin adds wrap lighting and thickness-driven scatter. Eye splits cornea, iris, and sclera for a dedicated close-up shader." );
    }
    ImGui::End();
}

void Chapter13App::shutdownDemo()
{
    releaseOffscreenTargets();
    releaseFullscreenQuad();
    deleteTexture( m_environmentMap );
    deleteTexture( m_brdfLut );
    deleteTexture( m_baseColorTexture );
    deleteTexture( m_normalTexture );
    deleteTexture( m_materialTexture );
    deleteTexture( m_maskTexture );
    deleteTexture( m_thicknessTexture );
    deleteTexture( m_irisTexture );
    deleteTexture( m_scleraTexture );
}

void Chapter13App::handleKey( int key, int scancode, int action, int mods )
{
    (void)scancode;
    (void)mods;
    if ( action != GLFW_PRESS || uiCapturesKeyboard() )
    {
        return;
    }

    if ( key == GLFW_KEY_1 )
    {
        m_materialModel = Transmission;
    }
    else if ( key == GLFW_KEY_2 )
    {
        m_materialModel = Skin;
    }
    else if ( key == GLFW_KEY_3 )
    {
        m_materialModel = Eye;
    }
}

void Chapter13App::handleMouseButton( int button, int action, int mods )
{
    (void)mods;
    if ( uiCapturesMouse() )
    {
        return;
    }

    double x = 0.0;
    double y = 0.0;
    glfwGetCursorPos( GetNativeHandle(), &x, &y );

    if ( action == GLFW_PRESS )
    {
        if ( button == GLFW_MOUSE_BUTTON_LEFT )
        {
            m_camera.beginRotate( x, y );
        }
        else if ( button == GLFW_MOUSE_BUTTON_RIGHT || button == GLFW_MOUSE_BUTTON_MIDDLE )
        {
            m_camera.beginPan( x, y );
        }
    }
    else if ( action == GLFW_RELEASE )
    {
        m_camera.endInteraction();
    }
}

void Chapter13App::handleCursorPos( double xpos, double ypos )
{
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.cursorPos( xpos, ypos );
}

void Chapter13App::handleScroll( double xoffset, double yoffset )
{
    (void)xoffset;
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.scroll( yoffset );
}
