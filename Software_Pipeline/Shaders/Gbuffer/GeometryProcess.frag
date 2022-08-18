#version 430 core
layout (location = 0) out vec3 gPositionDepth;// ����λPos+depth
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;// ע������Ҫ��CBO��RGB/RGBAͳһ
layout (location = 3) out vec3 viewPosition;
layout (location = 4) out vec3 viewNormal;

in VS_OUT {
    vec3 worldFragPos;
    vec3 worldNormal;
    vec2 TexCoords;

    vec3 viewFragPos;
    vec3 viewNormal;
} frag_in;

uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;


const float NEAR = 0.1; // ͶӰ����Ľ�ƽ��
const float FAR = 100.0f; // ͶӰ�����Զƽ��
// ��ȡ���ֵ
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // �ص�NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}


void main()
{    
    // �����ָ������ɫ������
    gPositionDepth.rgb = frag_in.worldFragPos;// ����������ռ�����
    // gPositionDepth.a = LinearizeDepth(gl_FragCoord.z); 
    //gPositionDepth.a = 0.0;

    gNormal = normalize(frag_in.worldNormal);

    viewPosition=frag_in.viewFragPos;
    viewNormal= normalize(frag_in.viewNormal);
   

    gAlbedoSpec = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    // �洢����ǿ�ȵ�Aͨ����
    // gAlbedoSpec.a = texture(texture_specular1, frag_in. TexCoords).r;
}  