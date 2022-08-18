#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
//layout (location = 3) in vec3 aTangent;
//layout (location = 4) in vec3 aBitangent;

out vec2 TexCoords;
out vec3 Normal;

uniform mat4 model;

layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};



void main()
{
    TexCoords = aTexCoords;    
    Normal=aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}