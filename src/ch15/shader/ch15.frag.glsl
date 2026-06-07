#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec3 vWorldPos;
in vec2 vTexCoord;
in vec3 vViewPosTangent;
in vec3 vFragPosTangent;
in vec3 vLightDirTangent;

uniform sampler2D _baseColorTexture;
uniform sampler2D _normalTexture;
uniform sampler2D _heightTexture;
uniform sampler2D _materialTexture;
uniform sampler2D _maskTexture;

uniform int uTechnique;
uniform float uHeightScale;
uniform int uMinLayers;
uniform int uMaxLayers;
uniform float uShadowHeightScale;
uniform vec3 uLightColor;

const int TECHNIQUE_NORMAL_ONLY = 0;
const int TECHNIQUE_PARALLAX_OFFSET = 1;
const int TECHNIQUE_POM = 2;
const int TECHNIQUE_POM_SELF_SHADOW = 3;

vec2 parallaxOffsetMapping( vec2 texCoord, vec3 viewDir )
{
    float height = texture( _heightTexture, texCoord ).r;
    vec2 offset = viewDir.xy / max( viewDir.z, 0.001 ) * ( height * uHeightScale );
    return texCoord - offset;
}

vec2 parallaxOcclusionMapping( vec2 texCoord, vec3 viewDir, out float layerDepth )
{
    float numLayers = mix( float( uMaxLayers ), float( uMinLayers ), abs( dot( vec3( 0.0, 0.0, 1.0 ), viewDir ) ) );
    float currentLayerDepth = 0.0;
    float depthStep = 1.0 / numLayers;
    vec2 p = viewDir.xy / max( viewDir.z, 0.001 ) * uHeightScale;
    vec2 deltaTexCoord = p / numLayers;
    vec2 currentTexCoord = texCoord;
    float currentDepthMapValue = texture( _heightTexture, currentTexCoord ).r;

    while ( currentLayerDepth < currentDepthMapValue )
    {
        currentTexCoord -= deltaTexCoord;
        currentDepthMapValue = texture( _heightTexture, currentTexCoord ).r;
        currentLayerDepth += depthStep;
    }

    vec2 previousTexCoord = currentTexCoord + deltaTexCoord;
    float afterDepth = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture( _heightTexture, previousTexCoord ).r - currentLayerDepth + depthStep;
    float weight = afterDepth / max( afterDepth - beforeDepth, 0.0001 );
    layerDepth = mix( currentLayerDepth, currentLayerDepth - depthStep, weight );
    return mix( currentTexCoord, previousTexCoord, clamp( weight, 0.0, 1.0 ) );
}

float parallaxSelfShadow( vec2 texCoord, vec3 lightDir, float initialDepth )
{
    if ( lightDir.z <= 0.0 )
    {
        return 1.0;
    }

    const int numShadowLayers = 24;
    float layerDepthStep = 1.0 / float( numShadowLayers );
    vec2 deltaTexCoord = lightDir.xy / max( lightDir.z, 0.001 ) * uShadowHeightScale / float( numShadowLayers );
    vec2 currentTexCoord = texCoord + deltaTexCoord;
    float currentDepth = initialDepth - layerDepthStep;

    float shadow = 1.0;
    for ( int layer = 0; layer < numShadowLayers; ++layer )
    {
        float sampledHeight = texture( _heightTexture, currentTexCoord ).r;
        if ( sampledHeight > currentDepth )
        {
            shadow = 0.35;
            break;
        }
        currentTexCoord += deltaTexCoord;
        currentDepth -= layerDepthStep;
    }
    return shadow;
}

void main()
{
    vec3 viewDir = normalize( vViewPosTangent - vFragPosTangent );
    vec3 lightDir = normalize( vLightDirTangent );

    vec2 texCoord = vTexCoord;
    float layerDepth = 0.0;

    if ( uTechnique == TECHNIQUE_PARALLAX_OFFSET )
    {
        texCoord = parallaxOffsetMapping( texCoord, viewDir );
    }
    else if ( uTechnique == TECHNIQUE_POM || uTechnique == TECHNIQUE_POM_SELF_SHADOW )
    {
        texCoord = parallaxOcclusionMapping( texCoord, viewDir, layerDepth );
    }

    if ( texCoord.x < 0.0 || texCoord.x > 1.0 || texCoord.y < 0.0 || texCoord.y > 1.0 )
    {
        discard;
    }

    vec3 baseColor = texture( _baseColorTexture, texCoord ).rgb;
    vec3 orm = texture( _materialTexture, texCoord ).rgb;
    vec3 normalMap = texture( _normalTexture, texCoord ).xyz * 2.0 - 1.0;
    vec3 N = normalize( normalMap );
    vec3 V = normalize( viewDir );
    vec3 L = normalize( lightDir );
    vec3 H = normalize( V + L );

    float roughness = clamp( orm.g, 0.04, 1.0 );
    float ao = orm.r;
    float NdotL = max( dot( N, L ), 0.0 );
    float NdotV = max( dot( N, V ), 0.0 );
    float NdotH = max( dot( N, H ), 0.0 );
    float VdotH = max( dot( V, H ), 0.0 );

    vec3 F0 = vec3( 0.04 );
    vec3 F = F0 + ( 1.0 - F0 ) * pow( 1.0 - VdotH, 5.0 );
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float denom = ( NdotH * NdotH ) * ( alpha2 - 1.0 ) + 1.0;
    float D = alpha2 / max( 3.14159265 * denom * denom, 0.0001 );
    float k = ( roughness + 1.0 ) * ( roughness + 1.0 ) / 8.0;
    float Gv = NdotV / mix( NdotV, 1.0, k );
    float Gl = NdotL / mix( NdotL, 1.0, k );
    vec3 specular = ( D * Gv * Gl * F ) / max( 4.0 * NdotV * NdotL, 0.0001 );

    float shadow = 1.0;
    if ( uTechnique == TECHNIQUE_POM_SELF_SHADOW )
    {
        shadow = parallaxSelfShadow( texCoord, L, layerDepth );
    }

    vec3 diffuse = baseColor / 3.14159265;
    vec3 color = ( diffuse + specular ) * uLightColor * NdotL * shadow;
    color += baseColor * 0.12 * ao;
    color = color / ( color + vec3( 1.0 ) );
    color = pow( color, vec3( 1.0 / 2.2 ) );
    fragColor = vec4( color, texture( _maskTexture, texCoord ).a );
}
