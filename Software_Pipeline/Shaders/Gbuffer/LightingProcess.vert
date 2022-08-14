#version 330 core

// 本质上变成后处理了，因此输入的是图像(缓冲)信息
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
// 因此也不需要MVP信息

out vec2 TexCoords;


void main()
{
    gl_Position =  vec4(aPos.x, aPos.y, 0.0, 1.0); 
    TexCoords = aTexCoords;
}