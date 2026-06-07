#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec2 vTexCoord;
in vec3 vWorldTangent;
in float vShellFraction;

uniform int uRenderMode;
uniform bool uAnisotropyEnabled;
uniform vec3 uCameraPos;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uAlphaCutoff;
uniform float uFlowInfluence;
uniform float uAnisotropyExponent;
uniform int uShellCount;
uniform float uFurLength;

uniform sampler2D uBaseColorTexture;
uniform sampler2D uNormalTexture;
uniform sampler2D uMaterialTexture;
uniform sampler2D uOpacityTexture;
uniform sampler2D uFlowTexture;
uniform sampler2D uIdTexture;
uniform sampler2D uRootTipTexture;
uniform samplerCube uEnvironmentMap;
uniform sampler2D uNoiseTexture;

vec3 buildTangentNormal( vec3 normal )
{
    vec3 sampleNormal = texture( uNormalTexture, vTexCoord ).xyz * 2.0 - 1.0;
    vec3 up = abs( normal.y ) < 0.999 ? vec3( 0.0, 1.0, 0.0 ) : vec3( 1.0, 0.0, 0.0 );
    vec3 tangent = normalize( cross( up, normal ) );
    vec3 bitangent = cross( normal, tangent );
    return normalize( mat3( tangent, bitangent, normal ) * sampleNormal );
}

float hairSpecular( vec3 tangentDir, vec3 L, vec3 V, float exponent )
{
    vec3 H = normalize( L + V );
    float TdotH = dot( normalize( tangentDir ), H );
    float sinTH = sqrt( max( 1.0 - TdotH * TdotH, 0.0 ) );
    return pow( sinTH, exponent );
}

void main()
{
    vec3 N = buildTangentNormal( normalize( vWorldNormal ) );
    vec3 V = normalize( uCameraPos - vWorldPos );
    vec3 L = normalize( -uLightDirection );
    vec3 H = normalize( L + V );
    vec3 baseColor = texture( uBaseColorTexture, vTexCoord ).rgb;
    vec3 orm = texture( uMaterialTexture, vTexCoord ).rgb;

    if ( uRenderMode == 0 )
    {
        float diffuse = max( dot( N, L ), 0.0 );
        vec3 color = baseColor * ( 0.18 + diffuse * 0.82 );
        color = color / ( color + vec3( 1.0 ) );
        color = pow( color, vec3( 1.0 / 2.2 ) );
        fragColor = vec4( color, 1.0 );
        return;
    }

    float alpha = texture( uOpacityTexture, vTexCoord ).r;
    float rootTip = texture( uRootTipTexture, vTexCoord ).r;
    float strandId = texture( uIdTexture, vTexCoord ).r;
    vec2 flow = texture( uFlowTexture, vTexCoord ).rg * 2.0 - 1.0;

    if ( uRenderMode == 1 )
    {
        if ( alpha < uAlphaCutoff )
        {
            discard;
        }

        vec3 geometricTangent = normalize( vWorldTangent );
        vec3 flowTangent = normalize( mix( geometricTangent, normalize( vec3( flow.x, 1.0, flow.y ) ), uFlowInfluence ) );

        float specular = uAnisotropyEnabled ? hairSpecular( flowTangent, L, V, mix( 24.0, uAnisotropyExponent, strandId ) )
                                            : pow( max( dot( N, H ), 0.0 ), 24.0 );
        float diffuse = max( dot( N, L ), 0.0 );
        vec3 tipTint = mix( vec3( 0.18, 0.11, 0.06 ), vec3( 0.86, 0.71, 0.42 ), rootTip );
        vec3 ibl = texture( uEnvironmentMap, reflect( -V, N ) ).rgb * 0.18;
        vec3 color = baseColor * tipTint * ( 0.14 + diffuse * 0.86 ) + ibl + vec3( 1.0 ) * specular * 0.55;
        color = color / ( color + vec3( 1.0 ) );
        color = pow( color, vec3( 1.0 / 2.2 ) );
        fragColor = vec4( color, alpha );
        return;
    }

    float density = texture( uNoiseTexture, vTexCoord * 4.0 + vec2( vShellFraction * 0.17 ) ).r;
    float shellMask = smoothstep( 0.15, 1.0, density - vShellFraction * 0.28 );
    float shellAlpha = shellMask * ( 1.0 - vShellFraction );
    if ( shellAlpha < 0.04 )
    {
        discard;
    }

    float rim = pow( 1.0 - max( dot( N, V ), 0.0 ), 2.5 );
    vec3 rootColor = vec3( 0.24, 0.18, 0.08 );
    vec3 tipColor = vec3( 0.82, 0.74, 0.54 );
    vec3 furColor = mix( rootColor, tipColor, vShellFraction ) * mix( 0.7, 1.05, density );
    vec3 color = furColor * ( 0.25 + max( dot( N, L ), 0.0 ) * 0.7 ) + vec3( 1.0, 0.92, 0.75 ) * rim * 0.3;
    color = color / ( color + vec3( 1.0 ) );
    color = pow( color, vec3( 1.0 / 2.2 ) );
    fragColor = vec4( color, shellAlpha );
}
