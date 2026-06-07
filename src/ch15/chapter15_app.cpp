#include "chapter15_app.h"

#include "procedural.h"
#include "texture.h"

#include <glm/ext/matrix_transform.hpp>

#include <imgui.h>

#include <filesystem>
#include <stdexcept>

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

GLuint createTextureFromImage( const std::filesystem::path& filePath, bool srgb )
{
    int width = 0;
    int height = 0;
    unsigned char* pixels = Texture::loadPixels( filePath.string(), width, height, true );
    if ( pixels == nullptr )
    {
        throw std::runtime_error( "Failed to load texture: " + filePath.string() );
    }

    GLuint texture = 0;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    const GLint mipLevels = static_cast<GLint>( std::floor( std::log2( std::max( width, height ) ) ) ) + 1;
    glTexStorage2D( GL_TEXTURE_2D, mipLevels, srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8, width, height );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );
    glGenerateMipmap( GL_TEXTURE_2D );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    Texture::deletePixels( pixels );
    return texture;
}
} // namespace

Chapter15App::Chapter15App() : DemoApp( "ch15" )
{
    m_camera.setTarget( glm::vec3( 0.0f, 1.1f, 0.0f ) );
    m_camera.setDistance( 5.7f );
}

void Chapter15App::setupDemo()
{
    m_plane = std::make_unique<Plane>( 2.4f, 2.4f, 1, 1, 1.0f, 1.0f );

    m_program.compileShader( resolveAssetPath( "shader/ch15.vert.glsl" ).string().c_str() );
    m_program.compileShader( resolveAssetPath( "shader/ch15.frag.glsl" ).string().c_str() );
    m_program.link();

    loadTextures();
}

void Chapter15App::loadTextures()
{
    releaseTextures();

    const std::filesystem::path textureRoot = resolveAssetPath( "media/texture/mybrick" );
    m_baseColorTexture = createTextureFromImage( textureRoot / "mybrick-color.png", true );
    m_normalTexture = createTextureFromImage( textureRoot / "mybrick-normal.png", false );
    m_heightTexture = createTextureFromImage( textureRoot / "mybrick-height.png", false );
    m_materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.84f, 0.0f );
    m_maskTexture = Procedural::createMaskTexture( 4, 1.0f );
}

void Chapter15App::releaseTextures()
{
    deleteTexture( m_baseColorTexture );
    deleteTexture( m_normalTexture );
    deleteTexture( m_heightTexture );
    deleteTexture( m_materialTexture );
    deleteTexture( m_maskTexture );
}

void Chapter15App::updateDemo( float dt )
{
    (void)dt;
}

void Chapter15App::renderPlaneAt( const glm::vec3& position, float yawDegrees, Technique technique, float uvScale )
{
    glm::mat4 model = glm::translate( glm::mat4( 1.0f ), position ) *
                      glm::rotate( glm::mat4( 1.0f ), glm::radians( yawDegrees ), glm::vec3( 0.0f, 1.0f, 0.0f ) ) *
                      glm::rotate( glm::mat4( 1.0f ), glm::radians( -90.0f ), glm::vec3( 1.0f, 0.0f, 0.0f ) );
    glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( model ) ) );

    m_program.setUniform( "uModel", model );
    m_program.setUniform( "uNormalMatrix", normalMatrix );
    m_program.setUniform( "uTechnique", static_cast<int>( technique ) );
    m_program.setUniform( "uUvScale", uvScale );
    m_plane->render();
}

void Chapter15App::renderDemo()
{
    glClearColor( 0.036f, 0.041f, 0.052f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    const glm::mat4 view = m_camera.viewMatrix();
    const glm::mat4 projection =
        m_camera.projectionMatrix( static_cast<float>( framebufferSize().x ) / static_cast<float>( framebufferSize().y ) );
    const glm::vec3 lightDirection =
        glm::normalize( glm::vec3( std::cos( m_lightYaw ) * std::cos( m_lightElevation ), std::sin( m_lightElevation ),
                                   std::sin( m_lightYaw ) * std::cos( m_lightElevation ) ) );

    m_program.use();
    m_program.setUniform( "uView", view );
    m_program.setUniform( "uProjection", projection );
    m_program.setUniform( "uCameraPos", m_camera.position() );
    m_program.setUniform( "uLightDirection", lightDirection );
    m_program.setUniform( "uLightColor", m_lightColor );
    m_program.setUniform( "uHeightScale", m_heightScale );
    m_program.setUniform( "uMinLayers", m_minLayers );
    m_program.setUniform( "uMaxLayers", m_maxLayers );
    m_program.setUniform( "uShadowHeightScale", m_shadowHeightScale );
    m_program.setUniform( "_baseColorTexture", 0 );
    m_program.setUniform( "_normalTexture", 1 );
    m_program.setUniform( "_heightTexture", 2 );
    m_program.setUniform( "_materialTexture", 3 );
    m_program.setUniform( "_maskTexture", 4 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_baseColorTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, m_normalTexture );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, m_heightTexture );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, m_materialTexture );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, m_maskTexture );

    switch ( m_recipe )
    {
    case SingleSurfaceRecipe:
        renderPlaneAt( glm::vec3( 0.0f, 1.1f, 0.0f ), -22.0f, m_singleTechnique, m_uvScale );
        break;
    case TechniqueComparisonRecipe:
        renderPlaneAt( glm::vec3( -2.7f, 1.1f, 0.0f ), -18.0f, NormalOnly, m_uvScale );
        renderPlaneAt( glm::vec3( 0.0f, 1.1f, 0.0f ), -18.0f, ParallaxOffset, m_uvScale );
        renderPlaneAt( glm::vec3( 2.7f, 1.1f, 0.0f ), -18.0f, ParallaxOcclusion, m_uvScale );
        break;
    case SelfShadowRecipe:
        renderPlaneAt( glm::vec3( -1.6f, 1.1f, 0.0f ), -28.0f, ParallaxOcclusion, m_uvScale );
        renderPlaneAt( glm::vec3( 1.6f, 1.1f, 0.0f ), -28.0f, ParallaxOcclusionShadow, m_uvScale );
        break;
    }
}

void Chapter15App::renderDemoUi()
{
    ImGui::SetNextWindowPos( ImVec2( 24.0f, 24.0f ), ImGuiCond_Once );
    ImGui::SetNextWindowSize( ImVec2( 460.0f, 420.0f ), ImGuiCond_Once );
    if ( ImGui::Begin( "Chapter 15 - Parallax Occlusion Mapping" ) )
    {
        static const char* recipeLabels[] = {
            "Single Surface",
            "Technique Comparison",
            "POM Self Shadow",
        };
        int recipeIndex = static_cast<int>( m_recipe );
        if ( ImGui::Combo( "Recipe", &recipeIndex, recipeLabels, IM_ARRAYSIZE( recipeLabels ) ) )
        {
            m_recipe = static_cast<Recipe>( recipeIndex );
        }

        static const char* techniqueLabels[] = {
            "Normal Only",
            "Parallax Offset",
            "Parallax Occlusion",
            "POM + Self Shadow",
        };
        int techniqueIndex = static_cast<int>( m_singleTechnique );
        if ( ImGui::Combo( "Single Technique", &techniqueIndex, techniqueLabels, IM_ARRAYSIZE( techniqueLabels ) ) )
        {
            m_singleTechnique = static_cast<Technique>( techniqueIndex );
        }

        ImGui::SliderFloat( "Height Scale", &m_heightScale, 0.0f, 0.12f );
        ImGui::SliderInt( "Min Layers", &m_minLayers, 4, 32 );
        ImGui::SliderInt( "Max Layers", &m_maxLayers, 8, 96 );
        ImGui::SliderFloat( "Shadow Height Scale", &m_shadowHeightScale, 0.0f, 0.08f );
        ImGui::SliderFloat( "UV Scale", &m_uvScale, 1.0f, 6.0f );
        ImGui::SliderFloat( "Light Yaw", &m_lightYaw, -3.14f, 3.14f );
        ImGui::SliderFloat( "Light Elevation", &m_lightElevation, 0.10f, 1.45f );
        ImGui::ColorEdit3( "Light Color", &m_lightColor.x );
        ImGui::Separator();
        ImGui::TextWrapped( "Image set: mybrick-color / mybrick-normal / mybrick-height. POM uses view-angle-dependent layer counts and the self-shadow mode adds a second ray march along the light vector." );
    }
    ImGui::End();
}

void Chapter15App::shutdownDemo()
{
    releaseTextures();
}

void Chapter15App::handleKey( int key, int scancode, int action, int mods )
{
    (void)scancode;
    (void)mods;
    if ( action != GLFW_PRESS || uiCapturesKeyboard() )
    {
        return;
    }

    if ( key == GLFW_KEY_1 )
    {
        m_recipe = SingleSurfaceRecipe;
    }
    else if ( key == GLFW_KEY_2 )
    {
        m_recipe = TechniqueComparisonRecipe;
    }
    else if ( key == GLFW_KEY_3 )
    {
        m_recipe = SelfShadowRecipe;
    }
}

void Chapter15App::handleMouseButton( int button, int action, int mods )
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

void Chapter15App::handleCursorPos( double xpos, double ypos )
{
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.cursorPos( xpos, ypos );
}

void Chapter15App::handleScroll( double xoffset, double yoffset )
{
    (void)xoffset;
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.scroll( yoffset );
}
