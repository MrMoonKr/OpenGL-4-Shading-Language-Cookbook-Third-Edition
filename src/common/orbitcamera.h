#pragma once

#include <glm/glm.hpp>

class OrbitCamera
{
  public:
    glm::mat4 viewMatrix() const;
    glm::mat4 projectionMatrix( float aspectRatio ) const;
    glm::vec3 position() const;

    void beginRotate( double x, double y );
    void beginPan( double x, double y );
    void endInteraction();
    void cursorPos( double x, double y );
    void scroll( double yoffset );

    void setTarget( const glm::vec3& target )
    {
        m_target = target;
    }
    void setDistance( float distance )
    {
        m_distance = distance;
    }

  private:
    glm::vec3 m_target = glm::vec3( 0.0f, 0.9f, 0.0f );
    float m_distance = 6.0f;
    float m_yaw = 0.45f;
    float m_pitch = 0.3f;
    bool m_rotating = false;
    bool m_panning = false;
    glm::dvec2 m_lastCursor = glm::dvec2( 0.0 );
};
