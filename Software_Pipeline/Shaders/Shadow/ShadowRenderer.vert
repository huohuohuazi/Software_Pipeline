#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    // 
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    // 世界坐标
    vert_out.FragPos = vec3(model * vec4(aPos, 1.0));
    // 确保法线始终垂直于表面，转置逆转矩阵
    vert_out.Normal = transpose(inverse(mat3(model))) * aNormal;

    vert_out.TexCoords = aTexcoords;
    // 于光源投影空间中的坐标（VP光源*M）
    vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
}