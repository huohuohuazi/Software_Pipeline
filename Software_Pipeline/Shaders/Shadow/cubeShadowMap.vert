#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    // ֻ�任������ռ�����
    gl_Position =  model * vec4(aPos, 1.0f);
}