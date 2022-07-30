#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;//Mæÿ’Û
uniform mat4 view;//Væÿ’Û
uniform mat4 projection;//Pæÿ’Û

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}