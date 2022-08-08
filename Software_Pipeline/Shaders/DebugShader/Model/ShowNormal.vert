#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform	mat4 view;


out v2g_Struct
{
    vec3 Normal;
}v2g_out;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    v2g_out.Normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}