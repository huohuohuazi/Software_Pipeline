#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

// û��M����
uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0);

    // ��Ϊ��պеĴ�Сֻ��1*1*1�����´�������嶼�������棬�����Ҫ�ü��������ֵ�����1
    // ͸�ӳ���ʵ��Ƭ����ɫ����ִ�еģ�(x,y,z /= w)����z���ó�w������z������Ϊ1
    gl_Position = pos.xyww;
}