#version 330 core

//in vec3 ourColour;
in vec2 TexCoord;

out vec4 colour;

uniform sampler2D ourTexture1;

void main()
{
	colour = texture (ourTexture1, TexCoord);
}