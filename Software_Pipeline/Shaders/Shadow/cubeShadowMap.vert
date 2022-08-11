#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;

void main()
{
    // 只变换到世界空间坐标
    gl_Position =  model * vec4(aPos, 1.0f);
}