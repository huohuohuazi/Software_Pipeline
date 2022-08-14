#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vert_out.FragPos = (model * vec4(position, 1.0)).xyz;// 这里是世界空间坐标
    vert_out.Normal = transpose(inverse(mat3(model))) * normal;
    vert_out.TexCoords = texCoords;

}