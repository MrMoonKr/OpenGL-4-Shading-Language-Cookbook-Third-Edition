#version 460 core

layout( location = 0 ) in vec3 aPosition;
layout( location = 1 ) in vec3 aNormal;
layout( location = 2 ) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat3 uNormalMatrix;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec2 vTexCoord;

void main()
{
    vec4 worldPosition = uModel * vec4( aPosition, 1.0 );
    vWorldPos = worldPosition.xyz;
    vWorldNormal = normalize( uNormalMatrix * aNormal );
    vTexCoord = aTexCoord;
    gl_Position = uProjection * uView * worldPosition;
}
