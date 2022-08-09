#version 330 core

uniform vec3 lightPos;
uniform mat4 model;

layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};


void main()
{
    // 于观察空间的位置
    gl_Position = projection * view * model * vec4(lightPos, 1.0);
}