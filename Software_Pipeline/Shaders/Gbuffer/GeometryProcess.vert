#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;


out VS_OUT {
    vec3 worldFragPos;
    vec3 worldNormal;
    vec2 TexCoords;

    vec3 viewFragPos;
    vec3 viewNormal;


} vert_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vert_out.worldFragPos = (model * vec4(position, 1.0)).xyz;// ����������ռ�����
    vert_out.worldNormal = transpose(inverse(mat3(model))) * normal;

    // ��ΪSSAOʹ���ӽǿռ䣬��˵�������
    vert_out.viewFragPos = (view*model * vec4(position, 1.0)).xyz;// �������ӽǿռ�����
    vert_out.viewNormal = transpose(inverse(mat3(view*model))) * normal;

    vert_out.TexCoords = texCoords;

}