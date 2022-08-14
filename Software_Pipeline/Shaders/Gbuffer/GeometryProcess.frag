#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;// ע������Ҫ��CBO��RGB/RGBAͳһ

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} frag_in;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;

void main()
{    
    // �����ָ������ɫ������
    gPosition = frag_in.FragPos;// ����������ռ�����
    gNormal = normalize(frag_in.Normal);
    gAlbedoSpec = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    // �洢����ǿ�ȵ�Aͨ����
    // gAlbedoSpec.a = texture(texture_specular1, frag_in. TexCoords).r;
}  