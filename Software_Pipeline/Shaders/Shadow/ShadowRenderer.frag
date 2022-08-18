#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} frag_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;// �Ƿ������Ӱ

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ��Դ��ͶӰ�ռ�����--��z���һ��--��ƬԪ��ȣ����Խ�� ���ԴԽԶ
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // NDC����ת��
    projCoords = projCoords * 0.5 + 0.5;
    // ��ô��Ƚ�ƬԪ�����
    float currentDepth = projCoords.z;

    // ���������ͼ����ȡ���Դ������λ��
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // ��ȡbias
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //float bias=0.05;
    
    // PCF
    // ˼·�Ƕ�shadowMap��β���ȡƽ��
    float shadow = 0.0;
    // textureSize(shadowMap, 0) �� 0��mipmap��vec2���͵Ŀ��͸�
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            // projCoords.xy + vec2(x, y) * texelSizeʵ��texcoord����ƫ�ƣ�Ҳ�������Ĳ�������Χ��9�����ص�uv����
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            // �� ƬԪ��� ���� �����ͼ����˵��ƬԪ���Դ֮���������赲����ƬԪ������Ӱ��
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // �����Ұ���Ĭ��Ϊ����Ӱ
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    // ��ɫ/����
    vec3 color = texture(diffuseTexture, frag_in.TexCoords).rgb;
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightColor = vec3(0.4);
    // ������
    vec3 ambient = 0.2 * color;
    // ������
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // �߹ⷴ��(Blinn_Phong����ģ��)
    vec3 viewDir = normalize(viewPos - frag_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // ��Ӱ
    float shadow = shadows ? ShadowCalculation(frag_in.FragPosLightSpace) : 0.0;// �Ƿ�����Ӱ                  
    shadow = min(shadow, 0.75);

    // �����ⲻ����ӰӰ��
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    // Debug�� ��Ӱ��ֵ��
    // lighting=(1.0 - shadow)*vec3(1.0);

    FragColor = vec4(lighting, 1.0f);
}