#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out v2g_Struct
{
    vec3 color;
}v2g_out;


void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0); 
    v2g_out.color=aColor;
}
