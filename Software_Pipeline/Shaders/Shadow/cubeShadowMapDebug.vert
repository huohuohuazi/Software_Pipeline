#version 430 core
layout (location = 0) in vec3 aPos;//��������
layout (location = 1) in vec2 aTexCoord;//��������

uniform mat4 model;

layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};


out vec2 TexCoords;
out vec3 FragPos;


void main() {

	//�˷���������ʼ
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	//gl_Position = vec4(aPos, 1.0);
	TexCoords = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
}