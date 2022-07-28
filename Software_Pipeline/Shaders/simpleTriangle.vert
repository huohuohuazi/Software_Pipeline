#version 330 core
layout(location = 0) in vec3 aPos;//顶点坐标
layout(location = 1) in vec3 aColor;//顶点颜色
layout (location = 2) in vec2 aTexCoord;//纹理坐标

out vec3 Color;
out vec2 TexCoord;


void main() {
	gl_Position = vec4(aPos, 1.0);
	Color = aColor;
	TexCoord=aTexCoord;
}