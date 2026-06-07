#pragma once

#include "demoapp.h"
#include "glslprogram.h"
#include "haircards.h"
#include "orbitcamera.h"
#include "plane.h"
#include "sphere.h"

#include <memory>

class Chapter14App : public DemoApp
{
  public:
    Chapter14App();

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
        HairCardsRecipe = 0,
        HairComparisonRecipe = 1,
        FurShellsRecipe = 2,
    };

    void drawGround();
    void drawHairClump( const glm::vec3& position, bool anisotropyEnabled );
    void drawFurShells();

  private:
    GLSLProgram m_program;
    std::unique_ptr<Plane> m_ground;
    std::unique_ptr<HairCards> m_hairCards;
    std::unique_ptr<Sphere> m_furSphere;
    OrbitCamera m_camera;

    GLuint m_environmentMap = 0;
    GLuint m_baseColorTexture = 0;
    GLuint m_normalTexture = 0;
    GLuint m_materialTexture = 0;
    GLuint m_opacityTexture = 0;
    GLuint m_flowTexture = 0;
    GLuint m_idTexture = 0;
    GLuint m_rootTipTexture = 0;
    GLuint m_noiseTexture = 0;

    Recipe m_recipe = HairCardsRecipe;
    float m_alphaCutoff = 0.35f;
    float m_flowInfluence = 0.65f;
    float m_anisotropyExponent = 48.0f;
    float m_furLength = 0.42f;
    int m_furShellCount = 12;
};
