#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 Position;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    Position = aPos;  
    gl_Position =  projection * view * vec4(Position, 1.0);
}