#version 330 core
layout (location = 0) in vec3 aPos;//��������
layout (location = 1) in vec2 aTexCoord;//��������

// size=16*4 + 16*4 = 128
layout (std140,binding = 1) uniform VPmatrix
{
	mat4 view;
	mat4 projection;
};

uniform mat4 model;


out vec2 TexCoords;

void main() {

	//�˷���������ʼ
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoords = aTexCoord;
}