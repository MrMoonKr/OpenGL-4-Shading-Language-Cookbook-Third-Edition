#pragma once

#include "cube.h"
#include "demoapp.h"
#include "glslprogram.h"
#include "orbitcamera.h"
#include "plane.h"
#include "sphere.h"

#include <memory>

class Chapter13App : public DemoApp
{
  public:
    Chapter13App();

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
    enum MaterialModel
    {
        Transmission = 0,
        Skin = 1,
        Eye = 2,
    };

    void recreateOffscreenTargets();
    void releaseOffscreenTargets();
    void createFullscreenQuad();
    void releaseFullscreenQuad();
    void renderBackgroundPass( const glm::mat4& view, const glm::mat4& projection );
    void presentBackground();
    void renderForegroundObject( const glm::mat4& view, const glm::mat4& projection );

  private:
    GLSLProgram m_backgroundProgram;
    GLSLProgram m_presentProgram;
    GLSLProgram m_materialProgram;
    std::unique_ptr<Plane> m_ground;
    std::unique_ptr<Cube> m_cube;
    std::unique_ptr<Sphere> m_sphere;
    OrbitCamera m_camera;

    GLuint m_sceneFbo = 0;
    GLuint m_sceneColor = 0;
    GLuint m_sceneDepth = 0;
    glm::ivec2 m_sceneExtent = glm::ivec2( 0 );

    GLuint m_environmentMap = 0;
    GLuint m_brdfLut = 0;
    GLuint m_baseColorTexture = 0;
    GLuint m_normalTexture = 0;
    GLuint m_materialTexture = 0;
    GLuint m_maskTexture = 0;
    GLuint m_thicknessTexture = 0;
    GLuint m_irisTexture = 0;
    GLuint m_scleraTexture = 0;

    GLuint m_quadVao = 0;
    GLuint m_quadVbo = 0;

    MaterialModel m_materialModel = Transmission;
    float m_refractionScale = 0.08f;
    float m_thicknessScale = 1.2f;
    float m_subsurfaceStrength = 0.55f;
    float m_subsurfaceMix = 0.6f;
    float m_irisDepthScale = 0.035f;
    glm::vec3 m_absorptionCoeff = glm::vec3( 0.75f, 0.18f, 0.08f );
};
