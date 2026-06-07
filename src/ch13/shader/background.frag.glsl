#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec3 vWorldPos;
in vec3 vWorldNormal;

uniform vec3 uBaseColor;
uniform vec3 uLightDirection;

void main()
{
    vec3 normal = normalize( vWorldNormal );
    float diffuse = max( dot( normal, -uLightDirection ), 0.0 );
    vec3 color = uBaseColor * ( 0.22 + diffuse * 0.78 );
    color = color / ( color + vec3( 1.0 ) );
    color = pow( color, vec3( 1.0 / 2.2 ) );
    fragColor = vec4( color, 1.0 );
}
