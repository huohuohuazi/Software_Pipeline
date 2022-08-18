#version 430 core
// ���һ���Ҷ�ֵ��gbuffer��ssaoCBO��
// out float FragColor;
out float ssaoCBO;

in vec2 TexCoords;

uniform sampler2D viewPosition;// Vec3
uniform sampler2D viewNormal;
uniform sampler2D aTexNoise;

// sample[64]��64���������
uniform vec3 samples[64];
uniform mat4 projection;


int kernelSize = 64;// ������
float radius = 0.5;// ����뾶
float bias = 0.025;

// tile noise texture
const vec2 noiseScale = vec2(1280.0/4.0, 720.0/4.0); 

void main()
{
    // ����SSAO����Ҫ�Ĳ���
    vec3 fragPos = texture(viewPosition, TexCoords).xyz;
    // fragPos=(view*vec4(fragPos,1.0)).xyz;// ת��������ռ�����

    vec3 normal = normalize(texture(viewNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(aTexNoise, TexCoords * noiseScale).xyz);

    // TBN���� : ���߿ռ�-->�ӽǿռ�
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    float occlusion = 0.0;
    // �����Ǽ�������ص㱻��ΧƬ���ڵ������
    for(int i = 0; i < kernelSize; ++i)
    {
        // �����������в���
        vec3 samplePos = TBN * samples[i];// ���߿ռ䷽��-->�ӽǿռ䷽��
        samplePos = fragPos + samplePos * radius; // ����
        
        // 
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // matrixP * matricxM * Pos==> �ü��ռ�
        offset.xyz /= offset.w; // ͸�ӻ���
        offset.xyz = offset.xyz * 0.5 + 0.5; // NDC��Ļ�ռ䣬������FBO�Ͷ�����
        
        // ȡ��samplePos�����Ӧ��������� ֮ǰ�Ѿ��洢��Aͨ������
        float sampleDepth = texture(viewPosition, offset.xy).z;
        
        // ��Χ���ԣ���ֻ֤���������ֵ��ȡ���뾶��ʱӰ���ڱ�����
        // ��radius / abs(fragPos.z - sampleDepth)������Χ�����޶���0-1֮��
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        // ��������ĵ�ǰ���ֵ sampleDepth �Ƿ���� ���Ƚϵ�(����) samplePos �����ֵ
        // ����ǣ������ӵ����յĹ���������
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    ssaoCBO = occlusion;// �����CBO��
    // ssaoCBO=1.0;
}
