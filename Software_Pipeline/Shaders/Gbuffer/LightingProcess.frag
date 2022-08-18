#version 430 core

out vec4 FragColor;
in vec2 TexCoords;

// ���ν׶��ǾͲ������ˣ�����ֱ�Ӵ�gAlbedoSpec(ColorAndSpecularCBO)�л�ȡ����
// uniform sampler2D texture_diffuse1;

uniform sampler2D gPositionDepth;// ��Ϊ����Ϊvec4 ���Ҫע������rgb��a
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gSSAO_blur;// ����ģ��������AO

uniform samplerCube depthMap;



uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;// �Ƿ������Ӱ

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // ƬԪ����Դ����
    vec3 fragToLight = fragPos - lightPos; 

    // ���Ƚϵ�ƬԪ�����
    float currentDepth = length(fragToLight);

    // PCF�Ż���ʹ��20��ƫ��������64�β���
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;

    float viewDistance = length(viewPos - fragPos);
    // ƫ�ư뾶
    // float diskRadius = 0.05;
    //���ݹ۲��ߵľ���������ƫ�ư뾶��
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane; 
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    
    return shadow;
}

// Debug�����ڲ鿴shadwmap��û������
vec4 DebugShowDepth(vec3 fragPos)
{
    // ƬԪ����Դ����
    vec3 fragToLight = fragPos - lightPos; 

    // ���Ƚϵ�ƬԪ�����
    float closestDepth = texture(depthMap, fragToLight).r; 
    float currentDepth = length(fragToLight);

    //vec3 color=texture(texture_diffuse1, frag_in.TexCoords).rgb;

    vec3 color=vec3(closestDepth);
    return  vec4(color, 1.0);
}

void main()
{           
    vec3 FragPos = texture(gPositionDepth, TexCoords).rgb;// ��������
    //float depth = texture(gPositionDepth, TexCoords).a;// ���ﲻ��Ҫ
    float AO = texture(gSSAO_blur, TexCoords).r;

    vec3 Normal = texture(gNormal, TexCoords).rgb;// 
    vec3 Color = texture(gAlbedoSpec, TexCoords).rgb;// ��ɫ/����
    
    vec3 lightColor = vec3(0.4);
    float Linear;
    float Quadratic;

    // ������
    vec3 ambient = vec3(0.3 * AO);
    // ambient= 0.2 * Color;
    

    // ������
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // �߹ⷴ��(Blinn_Phong����ģ��)
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(Normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor; 
    
    // ��Ӱ
    float shadow = shadows ? ShadowCalculation(FragPos) : 0.0;// �Ƿ�����Ӱ                  
    shadow = min(shadow, 0.75);
 
    
    // ���ǹ���˥��
    float dist = length(lightPos - FragPos);
    float attenuation = 1.0 / (1.0 + Linear * dist + Quadratic * dist * dist);

    //diffuse  *= attenuation;
    //specular *= attenuation;

    // ����
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * Color;
    //lighting=(1.0 - shadow)*vec3(1.0);
    FragColor = vec4(lighting, 1.0f);


    // Debug�� ��Ӱ��ֵ��
   // FragColor = DebugShowDepth(FragPos);
    //FragColor = vec4(Color,1.0);
   // FragColor = vec4(vec3(AO),1.0);
}