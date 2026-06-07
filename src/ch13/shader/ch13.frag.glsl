#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;

uniform int uMaterialModel;
uniform vec3 uCameraPos;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform vec3 uAbsorptionCoeff;
uniform vec2 uViewportSize;
uniform float uRefractionScale;
uniform float uThicknessScale;
uniform float uSubsurfaceStrength;
uniform float uSubsurfaceMix;
uniform float uIrisDepthScale;

uniform sampler2D uBaseColorTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uMaterialTexture;
uniform sampler2D uMaskTexture;
uniform sampler2D uThicknessTexture;
uniform sampler2D uIrisTexture;
uniform sampler2D uScleraTexture;
uniform samplerCube uEnvironmentMap;
uniform sampler2D uBrdfLut;
uniform sampler2D uSceneColorTexture;

const float PI = 3.14159265359;

mat3 buildTbn( vec3 normal )
{
    vec3 helper = abs( normal.y ) < 0.999 ? vec3( 0.0, 1.0, 0.0 ) : vec3( 1.0, 0.0, 0.0 );
    vec3 tangent = normalize( cross( helper, normal ) );
    vec3 bitangent = cross( normal, tangent );
    return mat3( tangent, bitangent, normal );
}

vec3 fresnelSchlick( float cosTheta, vec3 F0 )
{
    return F0 + ( 1.0 - F0 ) * pow( clamp( 1.0 - cosTheta, 0.0, 1.0 ), 5.0 );
}

float distributionGGX( vec3 N, vec3 H, float roughness )
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max( dot( N, H ), 0.0 );
    float denominator = NdotH * NdotH * ( a2 - 1.0 ) + 1.0;
    return a2 / max( PI * denominator * denominator, 0.0001 );
}

float geometrySchlickGGX( float NdotV, float roughness )
{
    float r = roughness + 1.0;
    float k = ( r * r ) / 8.0;
    return NdotV / mix( NdotV, 1.0, k );
}

float geometrySmith( vec3 N, vec3 V, vec3 L, float roughness )
{
    float ggxV = geometrySchlickGGX( max( dot( N, V ), 0.0 ), roughness );
    float ggxL = geometrySchlickGGX( max( dot( N, L ), 0.0 ), roughness );
    return ggxV * ggxL;
}

vec3 evaluatePrincipledCore( vec3 baseColor, vec3 N, vec3 V, float roughness, float metallic, float ao )
{
    vec3 L = normalize( -uLightDirection );
    vec3 H = normalize( V + L );
    vec3 F0 = mix( vec3( 0.04 ), baseColor, metallic );
    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );
    float D = distributionGGX( N, H, roughness );
    float G = geometrySmith( N, V, L, roughness );
    vec3 specular = ( D * G * F ) / ( 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 ) + 0.0001 );
    vec3 kD = ( vec3( 1.0 ) - F ) * ( 1.0 - metallic );
    vec3 direct = ( kD * baseColor / PI + specular ) * uLightColor * max( dot( N, L ), 0.0 );

    vec3 irradiance = texture( uEnvironmentMap, N ).rgb;
    vec3 diffuseIbl = irradiance * baseColor;
    vec3 reflected = reflect( -V, N );
    vec3 prefiltered = textureLod( uEnvironmentMap, reflected, roughness * 5.0 ).rgb;
    vec2 brdf = texture( uBrdfLut, vec2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
    vec3 specularIbl = prefiltered * ( F * brdf.x + brdf.y );
    vec3 ambient = ( kD * diffuseIbl + specularIbl ) * ao;
    return direct + ambient;
}

vec3 applyAbsorption( vec3 transmittanceColor, float thickness )
{
    return exp( -transmittanceColor * thickness );
}

void main()
{
    vec3 baseColor = texture( uBaseColorTexture, vTexCoord ).rgb;
    vec3 normalMap = texture( uNormalTexture, vTexCoord ).xyz * 2.0 - 1.0;
    vec3 orm = texture( uMaterialTexture, vTexCoord ).rgb;
    float ao = orm.r;
    float roughness = clamp( orm.g, 0.05, 1.0 );
    float metallic = clamp( orm.b, 0.0, 1.0 );
    float thickness = texture( uThicknessTexture, vTexCoord ).r * uThicknessScale;

    vec3 N = normalize( buildTbn( normalize( vWorldNormal ) ) * normalMap );
    vec3 V = normalize( uCameraPos - vWorldPos );
    vec3 color = evaluatePrincipledCore( baseColor, N, V, roughness, metallic, ao );

    if ( uMaterialModel == 0 )
    {
        vec3 refracted = refract( -V, N, 1.0 / 1.45 );
        vec2 screenUv = gl_FragCoord.xy / uViewportSize;
        vec2 refractUv = screenUv + refracted.xy * uRefractionScale;
        float validHit = step( 0.0, refractUv.x ) * step( 0.0, refractUv.y ) * step( refractUv.x, 1.0 ) * step( refractUv.y, 1.0 );
        vec3 sceneColor = texture( uSceneColorTexture, clamp( refractUv, 0.0, 1.0 ) ).rgb;
        vec3 fallback = texture( uEnvironmentMap, refracted ).rgb;
        vec3 transmitted = mix( fallback, sceneColor, validHit );
        vec3 absorption = applyAbsorption( uAbsorptionCoeff, thickness );
        color = mix( color, transmitted * absorption, 0.82 );
    }
    else if ( uMaterialModel == 1 )
    {
        vec3 L = normalize( -uLightDirection );
        float wrap = 0.35;
        float wrappedNdotL = max( ( dot( N, L ) + wrap ) / ( 1.0 + wrap ), 0.0 );
        float backScatter = max( dot( -N, L ), 0.0 );
        vec3 subsurfaceColor = vec3( 0.98, 0.45, 0.33 ) * wrappedNdotL * uSubsurfaceStrength;
        vec3 transmissionTint = vec3( 1.0, 0.36, 0.28 ) * backScatter * thickness * 0.35;
        vec3 layered = color + subsurfaceColor + transmissionTint;
        color = mix( color, layered, uSubsurfaceMix );
    }
    else if ( uMaterialModel == 2 )
    {
        vec2 tangentView = V.xy;
        vec2 irisUv = vTexCoord + tangentView * uIrisDepthScale;
        vec3 irisColor = texture( uIrisTexture, irisUv ).rgb;
        vec3 scleraColor = texture( uScleraTexture, vTexCoord ).rgb;
        float irisMask = smoothstep( 0.42, 0.10, distance( vTexCoord, vec2( 0.5 ) ) );
        vec3 eyeBase = mix( scleraColor, irisColor, irisMask );
        vec3 H = normalize( normalize( -uLightDirection ) + V );
        float corneaSpec = pow( max( dot( N, H ), 0.0 ), 128.0 );
        color = eyeBase * 0.16 + irisColor * 0.12 + vec3( 1.8 ) * corneaSpec;
    }

    color = color / ( color + vec3( 1.0 ) );
    color = pow( color, vec3( 1.0 / 2.2 ) );
    fragColor = vec4( color, texture( uMaskTexture, vTexCoord ).a );
}
