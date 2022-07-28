#version 330 core

uniform sampler2D texture1;
uniform sampler2D texture2;

in vec2 TexCoord;
out vec4 FragColor;

void main()
{
	// ÎÆÀí²ÉÑù
	FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}