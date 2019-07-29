#version 330 core

layout (location = 0) in vec3 position;
//layout (location = 1) in vec3 colour;
layout (location = 2) in vec2 texCoord;

//out vec3 ourColour;
out vec2 TexCoord;		// the name must be matching with "in vec2 TexCoord" in the
						// fragment shader

uniform mat4 model;				// converts local coordinates to camera coordinates
uniform mat4 projection;		// converts camera coordinates to normalised coordinates ( between 0 and 1 )
uniform mat4 view;				// converts the normalised coordinates to window coordinates

void main()
{
	gl_Position = projection * view * model * vec4(position, 1.0f);
	//ourColour = colour;

	TexCoord = vec2 (texCoord.x, 1.0 - texCoord.y);
			// the y coord in opengl is inverted that's why
			// we are writing "1.0 - texCoord.y"
}