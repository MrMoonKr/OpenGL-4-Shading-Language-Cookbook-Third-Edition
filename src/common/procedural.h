#pragma once

#include "cookbookogl.h"

#include <glm/glm.hpp>

namespace Procedural
{
GLuint createCheckerSrgbTexture( int size, const glm::vec3& colorA, const glm::vec3& colorB );
GLuint createFlatNormalTexture( int size );
GLuint createWaveNormalTexture( int size, float amplitude );
GLuint createOrmTexture( int size, float ao, float roughness, float metallic );
GLuint createEmissionTexture( int size, const glm::vec3& emissionColor );
GLuint createMaskTexture( int size, float alpha );
GLuint createThicknessTexture( int size );
GLuint createOpacityTexture( int size );
GLuint createFlowTexture( int size );
GLuint createIdTexture( int size );
GLuint createRootTipTexture( int size );
GLuint createNoiseTexture( int size );
GLuint createGradientCubemap( int size );
GLuint createBrdfLutTexture( int size );
} // namespace Procedural
