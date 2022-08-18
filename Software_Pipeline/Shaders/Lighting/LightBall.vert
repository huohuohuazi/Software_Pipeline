#version 430 core

uniform vec3 lightPos;
uniform mat4 model;

layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};


void main()
{
    // �ڹ۲�ռ��λ��
    gl_Position = projection * view * model * vec4(lightPos, 1.0);
}