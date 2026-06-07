#version 460 core

layout( location = 0 ) out vec4 fragColor;

in vec2 vTexCoord;

uniform sampler2D uColorTexture;

void main()
{
    fragColor = texture( uColorTexture, vTexCoord );
}
