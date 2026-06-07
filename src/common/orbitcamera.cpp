#include "orbitcamera.h"

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

glm::vec3 OrbitCamera::position() const
{
    const float cosPitch = std::cos( m_pitch );
    const glm::vec3 direction( std::cos( m_yaw ) * cosPitch, std::sin( m_pitch ), std::sin( m_yaw ) * cosPitch );
    return m_target + direction * m_distance;
}

glm::mat4 OrbitCamera::viewMatrix() const
{
    return glm::lookAt( position(), m_target, glm::vec3( 0.0f, 1.0f, 0.0f ) );
}

glm::mat4 OrbitCamera::projectionMatrix( float aspectRatio ) const
{
    return glm::perspective( glm::radians( 45.0f ), aspectRatio, 0.1f, 100.0f );
}

void OrbitCamera::beginRotate( double x, double y )
{
    m_rotating = true;
    m_panning = false;
    m_lastCursor = glm::dvec2( x, y );
}

void OrbitCamera::beginPan( double x, double y )
{
    m_panning = true;
    m_rotating = false;
    m_lastCursor = glm::dvec2( x, y );
}

void OrbitCamera::endInteraction()
{
    m_rotating = false;
    m_panning = false;
}

void OrbitCamera::cursorPos( double x, double y )
{
    const glm::dvec2 cursor( x, y );
    const glm::dvec2 delta = cursor - m_lastCursor;
    m_lastCursor = cursor;

    if ( m_rotating )
    {
        m_yaw -= static_cast<float>( delta.x ) * 0.008f;
        m_pitch -= static_cast<float>( delta.y ) * 0.008f;
        m_pitch = std::clamp( m_pitch, -1.4f, 1.4f );
    }

    if ( m_panning )
    {
        const glm::vec3 eye = position();
        const glm::vec3 forward = glm::normalize( m_target - eye );
        const glm::vec3 right = glm::normalize( glm::cross( forward, glm::vec3( 0.0f, 1.0f, 0.0f ) ) );
        const glm::vec3 up = glm::normalize( glm::cross( right, forward ) );
        m_target -= right * static_cast<float>( delta.x ) * 0.01f;
        m_target += up * static_cast<float>( delta.y ) * 0.01f;
    }
}

void OrbitCamera::scroll( double yoffset )
{
    m_distance = std::clamp( m_distance - static_cast<float>( yoffset ) * 0.35f, 1.25f, 18.0f );
}
