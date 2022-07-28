#version 330 core
layout(location = 0) in vec3 aPos;//顶点坐标
layout (location = 1) in vec2 aTexCoord;//纹理坐标

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;

void main() {

	//乘法从右往左开始
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}