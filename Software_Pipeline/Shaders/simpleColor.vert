
layout (location = 0) in vec3 aPos;

uniform mat4 model;//M����
uniform mat4 view;//V����
uniform mat4 projection;//P����

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
}