#version 460 core

layout( location = 0 ) in vec3 aPosition;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoord;
layout( location = 3 ) in vec4 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform int uRenderMode;
uniform int uShellIndex;
uniform int uShellCount;
uniform float uFurLength;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vTexCoord;
out vec3 vWorldTangent;
out float vShellFraction;

void main()
{
    float shellFraction = uShellCount > 1 ? float( uShellIndex ) / float( uShellCount - 1 ) : 0.0;
    vec3 position = aPosition;
    if ( uRenderMode == 2 )
    {
        position += aNormal * shellFraction * uFurLength;
    }

    vec4 worldPos = uModel * vec4( position, 1.0 );
    vWorldPos = worldPos.xyz;
    vWorldNormal = normalize( mat3( uModel ) * aNormal );
    vWorldTangent = normalize( mat3( uModel ) * aTangent.xyz );
    vTexCoord = aTexCoord;
    vShellFraction = shellFraction;
    gl_Position = uProjection * uView * worldPos;
}
