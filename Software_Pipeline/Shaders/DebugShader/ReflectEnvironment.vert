#version 330 core

// 布局修饰符
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 model;

// size=16*4 + 16*4 = 128
layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};



void main()
{
	 Normal =  mat3(transpose(inverse(model))) * aNormal;
	 Position=vec3(model*vec4(aPos,1.0));// 世界空间的位置
	 gl_Position = projection * view * model * vec4(aPos, 1.0);

}