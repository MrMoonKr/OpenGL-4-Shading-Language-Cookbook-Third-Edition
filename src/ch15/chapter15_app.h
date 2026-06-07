#pragma once

#include "demoapp.h"
#include "glslprogram.h"
#include "orbitcamera.h"
#include "plane.h"

#include <memory>

class Chapter15App : public DemoApp
{
  public:
    Chapter15App();

  protected:
    void setupDemo() override;
    void updateDemo( float dt ) override;
    void renderDemo() override;
    void renderDemoUi() override;
    void shutdownDemo() override;
    void handleKey( int key, int scancode, int action, int mods ) override;
    void handleMouseButton( int button, int action, int mods ) override;
    void handleCursorPos( double xpos, double ypos ) override;
    void handleScroll( double xoffset, double yoffset ) override;

  private:
    enum Recipe
    {
        SingleSurfaceRecipe = 0,
        TechniqueComparisonRecipe = 1,
        SelfShadowRecipe = 2,
    };

    enum Technique
    {
        NormalOnly = 0,
        ParallaxOffset = 1,
        ParallaxOcclusion = 2,
        ParallaxOcclusionShadow = 3,
    };

    void loadTextures();
    void releaseTextures();
    void renderPlaneAt( const glm::vec3& position, float yawDegrees, Technique technique, float uvScale );

  private:
    GLSLProgram m_program;
    std::unique_ptr<Plane> m_plane;
    OrbitCamera m_camera;

    GLuint m_baseColorTexture = 0;
    GLuint m_normalTexture = 0;
    GLuint m_heightTexture = 0;
    GLuint m_materialTexture = 0;
    GLuint m_maskTexture = 0;

    Recipe m_recipe = SingleSurfaceRecipe;
    Technique m_singleTechnique = ParallaxOcclusion;
    float m_heightScale = 0.055f;
    int m_minLayers = 12;
    int m_maxLayers = 40;
    float m_shadowHeightScale = 0.028f;
    float m_uvScale = 3.0f;
    float m_lightYaw = -0.72f;
    float m_lightElevation = 0.88f;
    glm::vec3 m_lightColor = glm::vec3( 10.5f, 9.9f, 9.4f );
};
