#include "procedural.h"

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

namespace
{
GLuint uploadTexture2D( int width, int height, GLenum internalFormat, GLenum format, GLenum type, const void* data,
                        bool generateMipmaps, bool clampToEdge )
{
    GLuint texture = 0;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    const GLint levels = generateMipmaps ? static_cast<GLint>( std::floor( std::log2( std::max( width, height ) ) ) ) + 1 : 1;
    glTexStorage2D( GL_TEXTURE_2D, levels, internalFormat, width, height );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, format, type, data );

    if ( generateMipmaps )
    {
        glGenerateMipmap( GL_TEXTURE_2D );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    }
    else
    {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    }

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToEdge ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToEdge ? GL_CLAMP_TO_EDGE : GL_REPEAT );
    return texture;
}

glm::vec3 faceDirection( int face, float u, float v )
{
    switch ( face )
    {
    case 0:
        return glm::normalize( glm::vec3( 1.0f, v, -u ) );
    case 1:
        return glm::normalize( glm::vec3( -1.0f, v, u ) );
    case 2:
        return glm::normalize( glm::vec3( u, 1.0f, -v ) );
    case 3:
        return glm::normalize( glm::vec3( u, -1.0f, v ) );
    case 4:
        return glm::normalize( glm::vec3( u, v, 1.0f ) );
    default:
        return glm::normalize( glm::vec3( -u, v, -1.0f ) );
    }
}

float radicalInverseVdc( uint32_t bits )
{
    bits = ( bits << 16u ) | ( bits >> 16u );
    bits = ( ( bits & 0x55555555u ) << 1u ) | ( ( bits & 0xAAAAAAAAu ) >> 1u );
    bits = ( ( bits & 0x33333333u ) << 2u ) | ( ( bits & 0xCCCCCCCCu ) >> 2u );
    bits = ( ( bits & 0x0F0F0F0Fu ) << 4u ) | ( ( bits & 0xF0F0F0F0u ) >> 4u );
    bits = ( ( bits & 0x00FF00FFu ) << 8u ) | ( ( bits & 0xFF00FF00u ) >> 8u );
    return static_cast<float>( bits ) * 2.3283064365386963e-10f;
}

glm::vec2 hammersley( uint32_t index, uint32_t count )
{
    return glm::vec2( static_cast<float>( index ) / static_cast<float>( count ), radicalInverseVdc( index ) );
}

glm::vec3 importanceSampleGgx( const glm::vec2& xi, const glm::vec3& normal, float roughness )
{
    const float a = roughness * roughness;
    const float phi = 2.0f * glm::pi<float>() * xi.x;
    const float cosTheta = std::sqrt( ( 1.0f - xi.y ) / ( 1.0f + ( a * a - 1.0f ) * xi.y ) );
    const float sinTheta = std::sqrt( std::max( 1.0f - cosTheta * cosTheta, 0.0f ) );

    const glm::vec3 halfVector( std::cos( phi ) * sinTheta, std::sin( phi ) * sinTheta, cosTheta );

    const glm::vec3 up = std::abs( normal.z ) < 0.999f ? glm::vec3( 0.0f, 0.0f, 1.0f ) : glm::vec3( 1.0f, 0.0f, 0.0f );
    const glm::vec3 tangent = glm::normalize( glm::cross( up, normal ) );
    const glm::vec3 bitangent = glm::cross( normal, tangent );

    const glm::vec3 sampleVector = tangent * halfVector.x + bitangent * halfVector.y + normal * halfVector.z;
    return glm::normalize( sampleVector );
}

float geometrySchlickGgxIbl( float nDotV, float roughness )
{
    const float a = roughness;
    const float k = ( a * a ) * 0.5f;
    return nDotV / ( nDotV * ( 1.0f - k ) + k );
}

glm::vec2 integrateBrdf( float nDotV, float roughness )
{
    glm::vec3 view( std::sqrt( std::max( 1.0f - nDotV * nDotV, 0.0f ) ), 0.0f, nDotV );
    glm::vec3 normal( 0.0f, 0.0f, 1.0f );

    float accumulatorA = 0.0f;
    float accumulatorB = 0.0f;
    constexpr uint32_t sampleCount = 128;

    for ( uint32_t sampleIndex = 0; sampleIndex < sampleCount; ++sampleIndex )
    {
        const glm::vec2 xi = hammersley( sampleIndex, sampleCount );
        const glm::vec3 halfVector = importanceSampleGgx( xi, normal, roughness );
        const glm::vec3 light = glm::normalize( 2.0f * glm::dot( view, halfVector ) * halfVector - view );

        const float nDotL = std::max( light.z, 0.0f );
        const float nDotH = std::max( halfVector.z, 0.0f );
        const float vDotH = std::max( glm::dot( view, halfVector ), 0.0f );

        if ( nDotL > 0.0f )
        {
            const float g = geometrySchlickGgxIbl( nDotV, roughness ) * geometrySchlickGgxIbl( nDotL, roughness );
            const float gVis = ( g * vDotH ) / std::max( nDotH * nDotV, 0.0001f );
            const float fresnel = std::pow( 1.0f - vDotH, 5.0f );

            accumulatorA += ( 1.0f - fresnel ) * gVis;
            accumulatorB += fresnel * gVis;
        }
    }

    return glm::vec2( accumulatorA, accumulatorB ) / static_cast<float>( sampleCount );
}
} // namespace

namespace Procedural
{
GLuint createCheckerSrgbTexture( int size, const glm::vec3& colorA, const glm::vec3& colorB )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const bool checker = ( ( x / 16 ) + ( y / 16 ) ) % 2 == 0;
            const glm::vec3 color = checker ? colorA : colorB;
            pixels[ y * size + x ] = glm::u8vec4( glm::clamp( color, 0.0f, 1.0f ) * 255.0f, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createFlatNormalTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size, glm::u8vec4( 128, 128, 255, 255 ) );
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createWaveNormalTexture( int size, float amplitude )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float fx = static_cast<float>( x ) / static_cast<float>( size );
            const float fy = static_cast<float>( y ) / static_cast<float>( size );
            glm::vec3 normal(
                std::sin( fx * glm::two_pi<float>() * 4.0f ) * amplitude,
                std::cos( fy * glm::two_pi<float>() * 4.0f ) * amplitude,
                1.0f );
            normal = glm::normalize( normal );
            pixels[ y * size + x ] = glm::u8vec4( ( normal * 0.5f + 0.5f ) * 255.0f, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createOrmTexture( int size, float ao, float roughness, float metallic )
{
    std::vector<glm::u8vec4> pixels( size * size );
    const glm::u8vec4 texel( static_cast<uint8_t>( ao * 255.0f ), static_cast<uint8_t>( roughness * 255.0f ),
                             static_cast<uint8_t>( metallic * 255.0f ), 255 );
    std::fill( pixels.begin(), pixels.end(), texel );
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), false, true );
}

GLuint createEmissionTexture( int size, const glm::vec3& emissionColor )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float fx = static_cast<float>( x ) / static_cast<float>( size - 1 );
            const float pulse = 0.4f + 0.6f * std::sin( fx * glm::two_pi<float>() * 6.0f );
            const glm::vec3 color = emissionColor * pulse;
            pixels[ y * size + x ] = glm::u8vec4( glm::clamp( color, 0.0f, 1.0f ) * 255.0f, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createMaskTexture( int size, float alpha )
{
    std::vector<glm::u8vec4> pixels( size * size, glm::u8vec4( 255, 255, 255, static_cast<uint8_t>( alpha * 255.0f ) ) );
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), false, true );
}

GLuint createThicknessTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const glm::vec2 uv = glm::vec2( x, y ) / static_cast<float>( size - 1 ) * 2.0f - 1.0f;
            const float radius = glm::length( uv );
            const float thickness = std::clamp( 1.0f - radius, 0.0f, 1.0f );
            pixels[ y * size + x ] = glm::u8vec4( static_cast<uint8_t>( thickness * 255.0f ), 0, 0, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, true );
}

GLuint createOpacityTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float fx = static_cast<float>( x ) / static_cast<float>( size - 1 );
            const float fy = static_cast<float>( y ) / static_cast<float>( size - 1 );
            const float strand = 0.5f + 0.5f * std::sin( fx * glm::two_pi<float>() * 14.0f + fy * 8.0f );
            const float edgeFade = std::pow( std::sin( fy * glm::pi<float>() ), 0.35f );
            const float alpha = std::clamp( strand * edgeFade, 0.0f, 1.0f );
            const uint8_t alphaByte = static_cast<uint8_t>( alpha * 255.0f );
            pixels[ y * size + x ] = glm::u8vec4( alphaByte, alphaByte, alphaByte, alphaByte );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createFlowTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float fy = static_cast<float>( y ) / static_cast<float>( size - 1 );
            const float angle = ( fy - 0.5f ) * 0.9f;
            const glm::vec2 flow = glm::normalize( glm::vec2( std::sin( angle ), 1.0f ) );
            pixels[ y * size + x ] =
                glm::u8vec4( ( flow.x * 0.5f + 0.5f ) * 255.0f, ( flow.y * 0.5f + 0.5f ) * 255.0f, 128, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createIdTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float fx = static_cast<float>( x ) / static_cast<float>( size - 1 );
            const float idValue = 0.5f + 0.5f * std::sin( fx * glm::two_pi<float>() * 7.0f );
            const uint8_t encoded = static_cast<uint8_t>( idValue * 255.0f );
            pixels[ y * size + x ] = glm::u8vec4( encoded, encoded, encoded, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createRootTipTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        const uint8_t encoded = static_cast<uint8_t>( static_cast<float>( y ) / static_cast<float>( size - 1 ) * 255.0f );
        for ( int x = 0; x < size; ++x )
        {
            pixels[ y * size + x ] = glm::u8vec4( encoded, encoded, encoded, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createNoiseTexture( int size )
{
    std::vector<glm::u8vec4> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float noise = 0.5f + 0.5f * std::sin( x * 12.9898f + y * 78.233f );
            const uint8_t encoded = static_cast<uint8_t>( std::clamp( noise, 0.0f, 1.0f ) * 255.0f );
            pixels[ y * size + x ] = glm::u8vec4( encoded, encoded, encoded, 255 );
        }
    }
    return uploadTexture2D( size, size, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data(), true, false );
}

GLuint createGradientCubemap( int size )
{
    GLuint cubemap = 0;
    glGenTextures( 1, &cubemap );
    glBindTexture( GL_TEXTURE_CUBE_MAP, cubemap );

    const GLint levels = static_cast<GLint>( std::floor( std::log2( size ) ) ) + 1;
    glTexStorage2D( GL_TEXTURE_CUBE_MAP, levels, GL_RGB16F, size, size );

    std::vector<glm::vec3> pixels( size * size );
    for ( int face = 0; face < 6; ++face )
    {
        for ( int y = 0; y < size; ++y )
        {
            for ( int x = 0; x < size; ++x )
            {
                const float u = ( ( static_cast<float>( x ) + 0.5f ) / static_cast<float>( size ) ) * 2.0f - 1.0f;
                const float v = ( ( static_cast<float>( y ) + 0.5f ) / static_cast<float>( size ) ) * 2.0f - 1.0f;
                const glm::vec3 direction = faceDirection( face, u, v );
                const float skyBlend = std::clamp( direction.y * 0.5f + 0.5f, 0.0f, 1.0f );
                const glm::vec3 skyColor = glm::mix( glm::vec3( 0.15f, 0.12f, 0.10f ), glm::vec3( 0.24f, 0.45f, 0.82f ), skyBlend );
                const float sun = std::pow( std::max( glm::dot( direction, glm::normalize( glm::vec3( 0.3f, 0.9f, 0.18f ) ) ), 0.0f ), 96.0f );
                pixels[ y * size + x ] = skyColor + glm::vec3( 1.2f, 0.95f, 0.7f ) * sun;
            }
        }

        glTexSubImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, 0, 0, size, size, GL_RGB, GL_FLOAT, pixels.data() );
    }

    glGenerateMipmap( GL_TEXTURE_CUBE_MAP );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
    return cubemap;
}

GLuint createBrdfLutTexture( int size )
{
    std::vector<glm::vec2> pixels( size * size );
    for ( int y = 0; y < size; ++y )
    {
        for ( int x = 0; x < size; ++x )
        {
            const float nDotV = ( static_cast<float>( x ) + 0.5f ) / static_cast<float>( size );
            const float roughness = ( static_cast<float>( y ) + 0.5f ) / static_cast<float>( size );
            pixels[ y * size + x ] = integrateBrdf( nDotV, roughness );
        }
    }
    return uploadTexture2D( size, size, GL_RG16F, GL_RG, GL_FLOAT, pixels.data(), false, true );
}
} // namespace Procedural
