#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out VS_OUT{
    vec2 TexCoords;
    vec3 WorldPos;
    vec3 Normal;
}vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vert_out.TexCoords = aTexCoords;
    vert_out.WorldPos = vec3(model * vec4(aPos, 1.0));
    vert_out.Normal = mat3(model) * aNormal;   

    gl_Position =  projection * view * vec4(vert_out.WorldPos, 1.0);
}