#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoords;


out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    // 
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

    // ��������
    vert_out.FragPos = vec3(model * vec4(aPos, 1.0));
    // ȷ������ʼ�մ�ֱ�ڱ��棬ת����ת����
    vert_out.Normal = transpose(inverse(mat3(model))) * aNormal;

    vert_out.TexCoords = aTexcoords;
    // �ڹ�ԴͶӰ�ռ��е����꣨VP��Դ*M��
    vert_out.FragPosLightSpace = lightSpaceMatrix * vec4(vert_out.FragPos, 1.0);
}