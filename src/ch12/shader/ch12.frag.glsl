#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;

struct MaterialGpuData
{
    vec4 baseColorFactor;
    vec3 emissionFactor;
    float metallicFactor;
    float roughnessFactor;
    float normalScale;
    float occlusionStrength;
    float clearcoatFactor;
    float clearcoatRoughness;
    float sheenFactor;
};

uniform MaterialGpuData uMaterial;

uniform vec3 uCameraPos;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uDirectScale;
uniform float uIblScale;

uniform sampler2D _baseColorTexture;
uniform sampler2D _normalTexture;
uniform sampler2D _materialTexture;
uniform sampler2D _maskTexture;
uniform sampler2D _emissiveTexture;
uniform sampler2D _clearcoatTexture;
uniform sampler2D _sheenTexture;
uniform samplerCube uEnvironmentMap;
uniform sampler2D uBrdfLut;

const float PI = 3.14159265359;

mat3 buildTbn( vec3 normal )
{
    vec3 tangentBasis = abs( normal.y ) < 0.999 ? vec3( 0.0, 1.0, 0.0 ) : vec3( 1.0, 0.0, 0.0 );
    vec3 tangent = normalize( cross( tangentBasis, normal ) );
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
    float NdotH2 = NdotH * NdotH;
    float denominator = ( NdotH2 * ( a2 - 1.0 ) + 1.0 );
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

void main()
{
    vec4 baseSample = texture( _baseColorTexture, vTexCoord ) * uMaterial.baseColorFactor;
    vec3 orm = texture( _materialTexture, vTexCoord ).rgb;
    float ao = orm.r * uMaterial.occlusionStrength;
    float roughness = clamp( orm.g * uMaterial.roughnessFactor, 0.045, 1.0 );
    float metallic = clamp( orm.b * uMaterial.metallicFactor, 0.0, 1.0 );

    mat3 tbn = buildTbn( normalize( vWorldNormal ) );
    vec3 normalMap = texture( _normalTexture, vTexCoord ).xyz * 2.0 - 1.0;
    normalMap.xy *= uMaterial.normalScale;
    vec3 N = normalize( tbn * normalMap );
    vec3 V = normalize( uCameraPos - vWorldPos );
    vec3 L = normalize( -uLightDirection );
    vec3 H = normalize( V + L );
    vec3 R = reflect( -V, N );

    vec3 F0 = mix( vec3( 0.04 ), baseSample.rgb, metallic );
    vec3 F = fresnelSchlick( max( dot( H, V ), 0.0 ), F0 );
    float D = distributionGGX( N, H, roughness );
    float G = geometrySmith( N, V, L, roughness );

    vec3 numerator = D * G * F;
    float denominator = 4.0 * max( dot( N, V ), 0.0 ) * max( dot( N, L ), 0.0 ) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = ( vec3( 1.0 ) - kS ) * ( 1.0 - metallic );
    vec3 diffuse = kD * baseSample.rgb / PI;
    float NdotL = max( dot( N, L ), 0.0 );
    vec3 directLighting = ( diffuse + specular ) * uLightColor * NdotL * uDirectScale;

    vec3 irradiance = texture( uEnvironmentMap, N ).rgb;
    vec3 diffuseIbl = irradiance * baseSample.rgb;

    float maxReflectionLod = 5.0;
    vec3 prefiltered = textureLod( uEnvironmentMap, R, roughness * maxReflectionLod ).rgb;
    vec2 brdf = texture( uBrdfLut, vec2( max( dot( N, V ), 0.0 ), roughness ) ).rg;
    vec3 specularIbl = prefiltered * ( F * brdf.x + brdf.y );
    vec3 ambient = ( kD * diffuseIbl + specularIbl ) * ao * uIblScale;

    float clearcoatMask = texture( _clearcoatTexture, vTexCoord ).r;
    float clearcoatRoughness = clamp( uMaterial.clearcoatRoughness, 0.04, 1.0 );
    float clearcoatD = distributionGGX( N, H, clearcoatRoughness );
    float clearcoatG = geometrySmith( N, V, L, clearcoatRoughness );
    vec3 clearcoatF = fresnelSchlick( max( dot( H, V ), 0.0 ), vec3( 0.04 ) );
    vec3 clearcoat = uMaterial.clearcoatFactor * clearcoatMask * ( clearcoatD * clearcoatG * clearcoatF ) / denominator * NdotL;

    float sheenMask = texture( _sheenTexture, vTexCoord ).r;
    float sheenTerm = pow( 1.0 - max( dot( N, V ), 0.0 ), 5.0 ) * uMaterial.sheenFactor * sheenMask;
    vec3 sheen = baseSample.rgb * sheenTerm;

    vec3 emission = texture( _emissiveTexture, vTexCoord ).rgb * uMaterial.emissionFactor;
    vec3 color = directLighting + ambient + clearcoat + sheen + emission;
    color = color / ( color + vec3( 1.0 ) );
    color = pow( color, vec3( 1.0 / 2.2 ) );

    fragColor = vec4( color, texture( _maskTexture, vTexCoord ).a );
}
