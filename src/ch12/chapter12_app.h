#pragma once

#include "demoapp.h"
#include "glslprogram.h"
#include "orbitcamera.h"
#include "plane.h"
#include "sphere.h"

#include <glm/glm.hpp>

#include <memory>
#include <string>
#include <vector>

struct Chapter12Material
{
    std::string label;
    glm::vec4 baseColorFactor = glm::vec4( 1.0f );
    glm::vec3 emissionFactor = glm::vec3( 0.0f );
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.5f;
    float normalScale = 1.0f;
    float occlusionStrength = 1.0f;
    float clearcoatFactor = 0.0f;
    float clearcoatRoughness = 0.15f;
    float sheenFactor = 0.0f;
    float directScale = 1.0f;
    float iblScale = 1.0f;

    GLuint _baseColorTexture = 0;
    GLuint _normalTexture = 0;
    GLuint _materialTexture = 0;
    GLuint _maskTexture = 0;
    GLuint _emissiveTexture = 0;
    GLuint _clearcoatTexture = 0;
    GLuint _sheenTexture = 0;
};

class Chapter12App : public DemoApp
{
  public:
    Chapter12App();

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
    void buildMaterials();
    void deleteMaterials();
    void drawGround();
    void drawSphere( const glm::vec3& position, float scale, const Chapter12Material& material );

  private:
    enum Recipe
    {
        MaterialArray = 0,
        DirectVsIbl = 1,
        LayeredExtensions = 2,
    };

    GLSLProgram m_program;
    std::unique_ptr<Plane> m_ground;
    std::unique_ptr<Sphere> m_sphere;
    OrbitCamera m_camera;
    std::vector<Chapter12Material> m_materials;
    GLuint m_environmentMap = 0;
    GLuint m_brdfLut = 0;
    Recipe m_recipe = MaterialArray;
    float m_lightYaw = 0.65f;
    float m_lightElevation = 0.65f;
    glm::vec3 m_lightColor = glm::vec3( 12.0f, 11.0f, 9.5f );
};
