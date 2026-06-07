#include "chapter14_app.h"

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

Chapter14App::Chapter14App() : DemoApp( "ch14" )
{
}

void Chapter14App::setupDemo()
{
    m_ground = std::make_unique<Plane>( 18.0f, 18.0f, 1, 1, 8.0f, 8.0f );
    m_hairCards = std::make_unique<HairCards>( 0.8f, 2.4f, 12 );
    m_furSphere = std::make_unique<Sphere>( 1.0f, 64, 32 );

    m_program.compileShader( resolveAssetPath( "shader/ch14.vert.glsl" ).string().c_str() );
    m_program.compileShader( resolveAssetPath( "shader/ch14.frag.glsl" ).string().c_str() );
    m_program.link();

    m_environmentMap = Procedural::createGradientCubemap( 128 );
    m_baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.26f, 0.19f, 0.11f ),
                                                               glm::vec3( 0.62f, 0.47f, 0.24f ) );
    m_normalTexture = Procedural::createWaveNormalTexture( 128, 0.14f );
    m_materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.48f, 0.0f );
    m_opacityTexture = Procedural::createOpacityTexture( 128 );
    m_flowTexture = Procedural::createFlowTexture( 128 );
    m_idTexture = Procedural::createIdTexture( 128 );
    m_rootTipTexture = Procedural::createRootTipTexture( 128 );
    m_noiseTexture = Procedural::createNoiseTexture( 128 );
}

void Chapter14App::updateDemo( float dt )
{
    (void)dt;
}

void Chapter14App::drawGround()
{
    const glm::mat4 view = m_camera.viewMatrix();
    const glm::mat4 projection =
        m_camera.projectionMatrix( static_cast<float>( framebufferSize().x ) / static_cast<float>( framebufferSize().y ) );

    m_program.use();
    m_program.setUniform( "uView", view );
    m_program.setUniform( "uProjection", projection );
    m_program.setUniform( "uModel", glm::mat4( 1.0f ) );
    m_program.setUniform( "uCameraPos", m_camera.position() );
    m_program.setUniform( "uLightDirection", glm::normalize( glm::vec3( -0.45f, -1.0f, -0.22f ) ) );
    m_program.setUniform( "uLightColor", glm::vec3( 9.0f, 8.6f, 7.9f ) );
    m_program.setUniform( "uRenderMode", 0 );
    m_program.setUniform( "uAnisotropyEnabled", false );
    m_program.setUniform( "uAlphaCutoff", m_alphaCutoff );
    m_program.setUniform( "uFlowInfluence", m_flowInfluence );
    m_program.setUniform( "uAnisotropyExponent", m_anisotropyExponent );
    m_program.setUniform( "uShellIndex", 0 );
    m_program.setUniform( "uShellCount", m_furShellCount );
    m_program.setUniform( "uFurLength", m_furLength );
    m_program.setUniform( "uEnvironmentMap", 7 );
    m_program.setUniform( "uBaseColorTexture", 0 );
    m_program.setUniform( "uNormalTexture", 1 );
    m_program.setUniform( "uMaterialTexture", 2 );
    m_program.setUniform( "uOpacityTexture", 3 );
    m_program.setUniform( "uFlowTexture", 4 );
    m_program.setUniform( "uIdTexture", 5 );
    m_program.setUniform( "uRootTipTexture", 6 );
    m_program.setUniform( "uNoiseTexture", 8 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_baseColorTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_normalTexture );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_materialTexture );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, m_opacityTexture );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, m_flowTexture );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, m_idTexture );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, m_rootTipTexture );
    glActiveTexture( GL_TEXTURE7 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_environmentMap );
    glActiveTexture( GL_TEXTURE8 );
    glBindTexture( GL_TEXTURE_2D, m_noiseTexture );

    m_ground->render();
}

void Chapter14App::drawHairClump( const glm::vec3& position, bool anisotropyEnabled )
{
    glm::mat4 model = glm::translate( glm::mat4( 1.0f ), position );
    m_program.setUniform( "uModel", model );
    m_program.setUniform( "uRenderMode", 1 );
    m_program.setUniform( "uAnisotropyEnabled", anisotropyEnabled );
    m_program.setUniform( "uShellIndex", 0 );
    m_hairCards->render();
}

void Chapter14App::drawFurShells()
{
    glDisable( GL_CULL_FACE );
    for ( int shellIndex = 0; shellIndex < m_furShellCount; ++shellIndex )
    {
        m_program.setUniform( "uModel", glm::translate( glm::mat4( 1.0f ), glm::vec3( 0.0f, 1.15f, 0.0f ) ) );
        m_program.setUniform( "uRenderMode", 2 );
        m_program.setUniform( "uAnisotropyEnabled", true );
        m_program.setUniform( "uShellIndex", shellIndex );
        m_furSphere->render();
    }
    glEnable( GL_CULL_FACE );
}

void Chapter14App::renderDemo()
{
    glClearColor( 0.035f, 0.041f, 0.055f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    drawGround();

    switch ( m_recipe )
    {
    case HairCardsRecipe:
        drawHairClump( glm::vec3( 0.0f, 0.15f, 0.0f ), true );
        break;
    case HairComparisonRecipe:
        drawHairClump( glm::vec3( -1.8f, 0.15f, 0.0f ), false );
        drawHairClump( glm::vec3( 1.8f, 0.15f, 0.0f ), true );
        break;
    case FurShellsRecipe:
        drawFurShells();
        break;
    }
}

void Chapter14App::renderDemoUi()
{
    ImGui::SetNextWindowPos( ImVec2( 24.0f, 24.0f ), ImGuiCond_Once );
    ImGui::SetNextWindowSize( ImVec2( 430.0f, 390.0f ), ImGuiCond_Once );
    if ( ImGui::Begin( "Chapter 14 - Fur and Hair" ) )
    {
        static const char* labels[] = {
            "Hair Cards",
            "Isotropic vs Anisotropic",
            "Fur Shell Layers",
        };
        int recipeIndex = static_cast<int>( m_recipe );
        if ( ImGui::Combo( "Recipe", &recipeIndex, labels, IM_ARRAYSIZE( labels ) ) )
        {
            m_recipe = static_cast<Recipe>( recipeIndex );
        }

        ImGui::SliderFloat( "Alpha Cutoff", &m_alphaCutoff, 0.05f, 0.85f );
        ImGui::SliderFloat( "Flow Influence", &m_flowInfluence, 0.0f, 1.0f );
        ImGui::SliderFloat( "Aniso Exponent", &m_anisotropyExponent, 8.0f, 96.0f );
        ImGui::SliderFloat( "Fur Length", &m_furLength, 0.05f, 0.75f );
        ImGui::SliderInt( "Fur Shell Count", &m_furShellCount, 4, 24 );
        ImGui::Separator();
        ImGui::TextWrapped( "Hair uses opacity, flow, ID, and root-to-tip textures. The comparison mode shows why anisotropic highlights are required for hair cards. Fur mode extrudes shell layers over a sphere for short-fur rendering." );
    }
    ImGui::End();
}

void Chapter14App::shutdownDemo()
{
    deleteTexture( m_environmentMap );
    deleteTexture( m_baseColorTexture );
    deleteTexture( m_normalTexture );
    deleteTexture( m_materialTexture );
    deleteTexture( m_opacityTexture );
    deleteTexture( m_flowTexture );
    deleteTexture( m_idTexture );
    deleteTexture( m_rootTipTexture );
    deleteTexture( m_noiseTexture );
}

void Chapter14App::handleKey( int key, int scancode, int action, int mods )
{
    (void)scancode;
    (void)mods;
    if ( action != GLFW_PRESS || uiCapturesKeyboard() )
    {
        return;
    }

    if ( key == GLFW_KEY_1 )
    {
        m_recipe = HairCardsRecipe;
    }
    else if ( key == GLFW_KEY_2 )
    {
        m_recipe = HairComparisonRecipe;
    }
    else if ( key == GLFW_KEY_3 )
    {
        m_recipe = FurShellsRecipe;
    }
}

void Chapter14App::handleMouseButton( int button, int action, int mods )
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

void Chapter14App::handleCursorPos( double xpos, double ypos )
{
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.cursorPos( xpos, ypos );
}

void Chapter14App::handleScroll( double xoffset, double yoffset )
{
    (void)xoffset;
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.scroll( yoffset );
}
