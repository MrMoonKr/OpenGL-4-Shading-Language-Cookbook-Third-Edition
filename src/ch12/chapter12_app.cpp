#include "chapter12_app.h"

#include "procedural.h"

#include <glm/ext/matrix_clip_space.hpp>
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
} // namespace

Chapter12App::Chapter12App() : DemoApp( "ch12" )
{
}

void Chapter12App::setupDemo()
{
    m_ground = std::make_unique<Plane>( 14.0f, 14.0f, 1, 1, 6.0f, 6.0f );
    m_sphere = std::make_unique<Sphere>( 1.0f, 64, 32 );

    m_program.compileShader( resolveAssetPath( "shader/ch12.vert.glsl" ).string().c_str() );
    m_program.compileShader( resolveAssetPath( "shader/ch12.frag.glsl" ).string().c_str() );
    m_program.link();

    m_environmentMap = Procedural::createGradientCubemap( 128 );
    m_brdfLut = Procedural::createBrdfLutTexture( 128 );
    buildMaterials();
}

void Chapter12App::buildMaterials()
{
    deleteMaterials();

    Chapter12Material metal;
    metal.label = "Brushed Metal";
    metal.baseColorFactor = glm::vec4( 0.82f, 0.84f, 0.88f, 1.0f );
    metal.metallicFactor = 1.0f;
    metal.roughnessFactor = 0.17f;
    metal._baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.79f, 0.82f, 0.88f ),
                                                                    glm::vec3( 0.63f, 0.67f, 0.74f ) );
    metal._normalTexture = Procedural::createWaveNormalTexture( 128, 0.18f );
    metal._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.18f, 1.0f );
    metal._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    metal._emissiveTexture = Procedural::createMaskTexture( 4, 0.0f );
    metal._clearcoatTexture = Procedural::createMaskTexture( 4, 0.0f );
    metal._sheenTexture = Procedural::createMaskTexture( 4, 0.0f );
    m_materials.push_back( metal );

    Chapter12Material paint;
    paint.label = "Painted Coat";
    paint.baseColorFactor = glm::vec4( 0.69f, 0.09f, 0.08f, 1.0f );
    paint.metallicFactor = 0.0f;
    paint.roughnessFactor = 0.33f;
    paint.clearcoatFactor = 0.95f;
    paint.clearcoatRoughness = 0.06f;
    paint._baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.81f, 0.14f, 0.08f ),
                                                                    glm::vec3( 0.62f, 0.06f, 0.08f ) );
    paint._normalTexture = Procedural::createWaveNormalTexture( 128, 0.09f );
    paint._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.35f, 0.0f );
    paint._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    paint._emissiveTexture = Procedural::createMaskTexture( 4, 0.0f );
    paint._clearcoatTexture = Procedural::createMaskTexture( 4, 1.0f );
    paint._sheenTexture = Procedural::createMaskTexture( 4, 0.0f );
    m_materials.push_back( paint );

    Chapter12Material plastic;
    plastic.label = "Plastic";
    plastic.baseColorFactor = glm::vec4( 0.10f, 0.32f, 0.71f, 1.0f );
    plastic.metallicFactor = 0.0f;
    plastic.roughnessFactor = 0.52f;
    plastic._baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.11f, 0.35f, 0.74f ),
                                                                      glm::vec3( 0.09f, 0.25f, 0.57f ) );
    plastic._normalTexture = Procedural::createWaveNormalTexture( 128, 0.12f );
    plastic._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.52f, 0.0f );
    plastic._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    plastic._emissiveTexture = Procedural::createMaskTexture( 4, 0.0f );
    plastic._clearcoatTexture = Procedural::createMaskTexture( 4, 0.0f );
    plastic._sheenTexture = Procedural::createMaskTexture( 4, 0.0f );
    m_materials.push_back( plastic );

    Chapter12Material fabric;
    fabric.label = "Fabric";
    fabric.baseColorFactor = glm::vec4( 0.58f, 0.53f, 0.46f, 1.0f );
    fabric.metallicFactor = 0.0f;
    fabric.roughnessFactor = 0.83f;
    fabric.sheenFactor = 0.9f;
    fabric._baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.62f, 0.56f, 0.46f ),
                                                                     glm::vec3( 0.52f, 0.46f, 0.38f ) );
    fabric._normalTexture = Procedural::createWaveNormalTexture( 128, 0.28f );
    fabric._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.85f, 0.0f );
    fabric._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    fabric._emissiveTexture = Procedural::createMaskTexture( 4, 0.0f );
    fabric._clearcoatTexture = Procedural::createMaskTexture( 4, 0.0f );
    fabric._sheenTexture = Procedural::createMaskTexture( 4, 1.0f );
    m_materials.push_back( fabric );

    Chapter12Material emissive = paint;
    emissive.label = "Emissive Panel";
    emissive.baseColorFactor = glm::vec4( 0.16f, 0.18f, 0.21f, 1.0f );
    emissive.roughnessFactor = 0.42f;
    emissive.clearcoatFactor = 0.65f;
    emissive.sheenFactor = 0.0f;
    emissive.emissionFactor = glm::vec3( 3.0f, 1.6f, 0.55f );
    emissive._baseColorTexture = Procedural::createCheckerSrgbTexture( 128, glm::vec3( 0.18f, 0.20f, 0.24f ),
                                                                       glm::vec3( 0.09f, 0.10f, 0.12f ) );
    emissive._normalTexture = Procedural::createWaveNormalTexture( 128, 0.08f );
    emissive._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.42f, 0.0f );
    emissive._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    emissive._emissiveTexture = Procedural::createEmissionTexture( 128, glm::vec3( 1.0f, 0.48f, 0.15f ) );
    emissive._clearcoatTexture = Procedural::createMaskTexture( 4, 1.0f );
    emissive._sheenTexture = Procedural::createMaskTexture( 4, 0.0f );
    m_materials.push_back( emissive );
}

void Chapter12App::deleteMaterials()
{
    for ( Chapter12Material& material : m_materials )
    {
        deleteTexture( material._baseColorTexture );
        deleteTexture( material._normalTexture );
        deleteTexture( material._materialTexture );
        deleteTexture( material._maskTexture );
        deleteTexture( material._emissiveTexture );
        deleteTexture( material._clearcoatTexture );
        deleteTexture( material._sheenTexture );
    }
    m_materials.clear();
}

void Chapter12App::updateDemo( float dt )
{
    (void)dt;
}

void Chapter12App::renderDemo()
{
    glClearColor( 0.038f, 0.043f, 0.061f, 1.0f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    const glm::vec2 framebuffer = glm::vec2( framebufferSize() );
    const glm::mat4 view = m_camera.viewMatrix();
    const glm::mat4 projection = m_camera.projectionMatrix( framebuffer.x / framebuffer.y );
    const glm::vec3 cameraPosition = m_camera.position();
    const glm::vec3 lightDirection =
        glm::normalize( glm::vec3( std::cos( m_lightYaw ) * std::cos( m_lightElevation ), std::sin( m_lightElevation ),
                                   std::sin( m_lightYaw ) * std::cos( m_lightElevation ) ) );

    m_program.use();
    m_program.setUniform( "uView", view );
    m_program.setUniform( "uProjection", projection );
    m_program.setUniform( "uCameraPos", cameraPosition );
    m_program.setUniform( "uLightDirection", lightDirection );
    m_program.setUniform( "uLightColor", m_lightColor );
    m_program.setUniform( "uEnvironmentMap", 8 );
    m_program.setUniform( "uBrdfLut", 9 );

    glActiveTexture( GL_TEXTURE8 );
    glBindTexture( GL_TEXTURE_CUBE_MAP, m_environmentMap );
    glActiveTexture( GL_TEXTURE9 );
    glBindTexture( GL_TEXTURE_2D, m_brdfLut );

    drawGround();

    switch ( m_recipe )
    {
    case MaterialArray:
        drawSphere( glm::vec3( -4.2f, 1.05f, 0.0f ), 1.0f, m_materials[ 0 ] );
        drawSphere( glm::vec3( -1.4f, 1.05f, 0.0f ), 1.0f, m_materials[ 1 ] );
        drawSphere( glm::vec3( 1.4f, 1.05f, 0.0f ), 1.0f, m_materials[ 2 ] );
        drawSphere( glm::vec3( 4.2f, 1.05f, 0.0f ), 1.0f, m_materials[ 3 ] );
        break;
    case DirectVsIbl:
    {
        Chapter12Material directOnly = m_materials[ 0 ];
        directOnly.iblScale = 0.0f;
        Chapter12Material balanced = m_materials[ 0 ];
        Chapter12Material iblOnly = m_materials[ 0 ];
        iblOnly.directScale = 0.0f;
        drawSphere( glm::vec3( -3.6f, 1.05f, 0.0f ), 1.0f, directOnly );
        drawSphere( glm::vec3( 0.0f, 1.05f, 0.0f ), 1.0f, balanced );
        drawSphere( glm::vec3( 3.6f, 1.05f, 0.0f ), 1.0f, iblOnly );
        break;
    }
    case LayeredExtensions:
        drawSphere( glm::vec3( -3.0f, 1.05f, 0.0f ), 1.0f, m_materials[ 1 ] );
        drawSphere( glm::vec3( 0.0f, 1.05f, 0.0f ), 1.0f, m_materials[ 3 ] );
        drawSphere( glm::vec3( 3.0f, 1.05f, 0.0f ), 1.0f, m_materials[ 4 ] );
        break;
    }
}

void Chapter12App::drawGround()
{
    Chapter12Material groundMaterial;
    groundMaterial.baseColorFactor = glm::vec4( 0.20f, 0.21f, 0.24f, 1.0f );
    groundMaterial.roughnessFactor = 0.95f;
    groundMaterial._baseColorTexture =
        Procedural::createCheckerSrgbTexture( 64, glm::vec3( 0.19f, 0.19f, 0.20f ), glm::vec3( 0.14f, 0.14f, 0.15f ) );
    groundMaterial._normalTexture = Procedural::createFlatNormalTexture( 4 );
    groundMaterial._materialTexture = Procedural::createOrmTexture( 4, 1.0f, 0.95f, 0.0f );
    groundMaterial._maskTexture = Procedural::createMaskTexture( 4, 1.0f );
    groundMaterial._emissiveTexture = Procedural::createMaskTexture( 4, 0.0f );
    groundMaterial._clearcoatTexture = Procedural::createMaskTexture( 4, 0.0f );
    groundMaterial._sheenTexture = Procedural::createMaskTexture( 4, 0.0f );

    const glm::mat4 model = glm::mat4( 1.0f );
    const glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( model ) ) );
    m_program.setUniform( "uModel", model );
    m_program.setUniform( "uNormalMatrix", normalMatrix );

    m_program.setUniform( "uMaterial.baseColorFactor", groundMaterial.baseColorFactor );
    m_program.setUniform( "uMaterial.emissionFactor", groundMaterial.emissionFactor );
    m_program.setUniform( "uMaterial.metallicFactor", groundMaterial.metallicFactor );
    m_program.setUniform( "uMaterial.roughnessFactor", groundMaterial.roughnessFactor );
    m_program.setUniform( "uMaterial.normalScale", groundMaterial.normalScale );
    m_program.setUniform( "uMaterial.occlusionStrength", groundMaterial.occlusionStrength );
    m_program.setUniform( "uMaterial.clearcoatFactor", groundMaterial.clearcoatFactor );
    m_program.setUniform( "uMaterial.clearcoatRoughness", groundMaterial.clearcoatRoughness );
    m_program.setUniform( "uMaterial.sheenFactor", groundMaterial.sheenFactor );
    m_program.setUniform( "uDirectScale", 1.0f );
    m_program.setUniform( "uIblScale", 1.0f );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._baseColorTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._normalTexture );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._materialTexture );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._maskTexture );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._emissiveTexture );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._clearcoatTexture );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, groundMaterial._sheenTexture );

    m_program.setUniform( "_baseColorTexture", 0 );
    m_program.setUniform( "_normalTexture", 1 );
    m_program.setUniform( "_materialTexture", 2 );
    m_program.setUniform( "_maskTexture", 3 );
    m_program.setUniform( "_emissiveTexture", 4 );
    m_program.setUniform( "_clearcoatTexture", 5 );
    m_program.setUniform( "_sheenTexture", 6 );

    m_ground->render();

    deleteTexture( groundMaterial._baseColorTexture );
    deleteTexture( groundMaterial._normalTexture );
    deleteTexture( groundMaterial._materialTexture );
    deleteTexture( groundMaterial._maskTexture );
    deleteTexture( groundMaterial._emissiveTexture );
    deleteTexture( groundMaterial._clearcoatTexture );
    deleteTexture( groundMaterial._sheenTexture );
}

void Chapter12App::drawSphere( const glm::vec3& position, float scale, const Chapter12Material& material )
{
    if ( scale <= 0.0f )
    {
        return;
    }

    const glm::mat4 model =
        glm::translate( glm::mat4( 1.0f ), position ) * glm::scale( glm::mat4( 1.0f ), glm::vec3( scale ) );
    const glm::mat3 normalMatrix = glm::transpose( glm::inverse( glm::mat3( model ) ) );

    m_program.setUniform( "uModel", model );
    m_program.setUniform( "uNormalMatrix", normalMatrix );

    m_program.setUniform( "uMaterial.baseColorFactor", material.baseColorFactor );
    m_program.setUniform( "uMaterial.emissionFactor", material.emissionFactor );
    m_program.setUniform( "uMaterial.metallicFactor", material.metallicFactor );
    m_program.setUniform( "uMaterial.roughnessFactor", material.roughnessFactor );
    m_program.setUniform( "uMaterial.normalScale", material.normalScale );
    m_program.setUniform( "uMaterial.occlusionStrength", material.occlusionStrength );
    m_program.setUniform( "uMaterial.clearcoatFactor", material.clearcoatFactor );
    m_program.setUniform( "uMaterial.clearcoatRoughness", material.clearcoatRoughness );
    m_program.setUniform( "uMaterial.sheenFactor", material.sheenFactor );
    m_program.setUniform( "uDirectScale", material.directScale );
    m_program.setUniform( "uIblScale", material.iblScale );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, material._baseColorTexture );
    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_2D, material._normalTexture );
    glActiveTexture( GL_TEXTURE2 );
    glBindTexture( GL_TEXTURE_2D, material._materialTexture );
    glActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, material._maskTexture );
    glActiveTexture( GL_TEXTURE4 );
    glBindTexture( GL_TEXTURE_2D, material._emissiveTexture );
    glActiveTexture( GL_TEXTURE5 );
    glBindTexture( GL_TEXTURE_2D, material._clearcoatTexture );
    glActiveTexture( GL_TEXTURE6 );
    glBindTexture( GL_TEXTURE_2D, material._sheenTexture );

    m_program.setUniform( "_baseColorTexture", 0 );
    m_program.setUniform( "_normalTexture", 1 );
    m_program.setUniform( "_materialTexture", 2 );
    m_program.setUniform( "_maskTexture", 3 );
    m_program.setUniform( "_emissiveTexture", 4 );
    m_program.setUniform( "_clearcoatTexture", 5 );
    m_program.setUniform( "_sheenTexture", 6 );

    m_sphere->render();
}

void Chapter12App::renderDemoUi()
{
    ImGui::SetNextWindowPos( ImVec2( 24.0f, 24.0f ), ImGuiCond_Once );
    ImGui::SetNextWindowSize( ImVec2( 420.0f, 360.0f ), ImGuiCond_Once );
    if ( ImGui::Begin( "Chapter 12 - Principled BSDF" ) )
    {
        static const char* recipeLabels[] = {
            "Material Ball Array",
            "Direct vs IBL",
            "Clearcoat / Sheen / Emission",
        };
        int recipeIndex = static_cast<int>( m_recipe );
        if ( ImGui::Combo( "Recipe", &recipeIndex, recipeLabels, IM_ARRAYSIZE( recipeLabels ) ) )
        {
            m_recipe = static_cast<Recipe>( recipeIndex );
        }

        ImGui::SliderFloat( "Light Yaw", &m_lightYaw, -3.14f, 3.14f );
        ImGui::SliderFloat( "Light Elevation", &m_lightElevation, 0.15f, 1.35f );
        ImGui::ColorEdit3( "Light Color", &m_lightColor.x );
        ImGui::Separator();
        ImGui::TextWrapped( "Tab: toggle UI, Left mouse: orbit, Right mouse: pan, Wheel: zoom." );

        if ( m_recipe == MaterialArray )
        {
            ImGui::BulletText( "Metal / clearcoat paint / plastic / fabric comparison." );
        }
        else if ( m_recipe == DirectVsIbl )
        {
            ImGui::BulletText( "Left: direct only, Center: direct + IBL, Right: IBL only." );
        }
        else
        {
            ImGui::BulletText( "Left: clearcoat paint, Center: sheen fabric, Right: emissive layer." );
        }
    }
    ImGui::End();
}

void Chapter12App::shutdownDemo()
{
    deleteMaterials();
    deleteTexture( m_environmentMap );
    deleteTexture( m_brdfLut );
}

void Chapter12App::handleKey( int key, int scancode, int action, int mods )
{
    (void)scancode;
    (void)mods;
    if ( action != GLFW_PRESS || uiCapturesKeyboard() )
    {
        return;
    }

    if ( key == GLFW_KEY_1 )
    {
        m_recipe = MaterialArray;
    }
    else if ( key == GLFW_KEY_2 )
    {
        m_recipe = DirectVsIbl;
    }
    else if ( key == GLFW_KEY_3 )
    {
        m_recipe = LayeredExtensions;
    }
}

void Chapter12App::handleMouseButton( int button, int action, int mods )
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

void Chapter12App::handleCursorPos( double xpos, double ypos )
{
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.cursorPos( xpos, ypos );
}

void Chapter12App::handleScroll( double xoffset, double yoffset )
{
    (void)xoffset;
    if ( uiCapturesMouse() )
    {
        return;
    }
    m_camera.scroll( yoffset );
}
