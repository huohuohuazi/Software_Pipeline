#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


out vert_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;// 片段于灯光的透视空间坐标
} vs_out;

uniform mat4 model;
// size=16*4 + 16*4 = 128
layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};
uniform mat4 lightSpaceMatrix;// 相机空间VP矩阵

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    vs_out.FragPos =  vec3(model * vec4(aPos, 1.0));// 世界空间坐标
    vs_out.Normal =  aNormal; 
    vs_out.TexCoords=aTexCoords;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);// 灯光的相机空间坐标
    
}