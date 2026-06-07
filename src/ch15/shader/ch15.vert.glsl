#version 460 core

layout( location = 0 ) in vec3 aPosition;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoord;
layout( location = 3 ) in vec4 aTangent;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;
uniform float uUvScale;

out vec3 vWorldPos;
out vec2 vTexCoord;
out vec3 vViewPosTangent;
out vec3 vFragPosTangent;
out vec3 vLightDirTangent;

uniform vec3 uCameraPos;
uniform vec3 uLightDirection;

void main()
{
    vec4 worldPos = uModel * vec4( aPosition, 1.0 );
    vec3 N = normalize( uNormalMatrix * aNormal );
    vec3 T = normalize( uNormalMatrix * aTangent.xyz );
    vec3 B = normalize( cross( N, T ) * aTangent.w );
    mat3 tbn = transpose( mat3( T, B, N ) );

    vWorldPos = worldPos.xyz;
    vTexCoord = aTexCoord * uUvScale;
    vFragPosTangent = tbn * worldPos.xyz;
    vViewPosTangent = tbn * uCameraPos;
    vLightDirTangent = tbn * normalize( -uLightDirection );

    gl_Position = uProjection * uView * worldPos;
}
