#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} frag_in;

uniform sampler2D texture_diffuse1;
uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;// �Ƿ�������?

vec3 sampleOffsetDirections[20] = vec3[](
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1));

float ShadowCalculation(vec3 fragPos)
{
    // ƬԪ����Դ����
    vec3 fragToLight = fragPos - lightPos; 

    // ���Ƚϵ�ƬԪ�����?
    float currentDepth = length(fragToLight);


    // PCF
    //float bias = 0.05; 
   // float shadow = 0.0;//��Ӱֵ
    // float samples = 4.0;
    //float offset = 0.1;

    // ����ά�ȵ����Ƕ�
    //for(float x = -offset; x < offset; x += offset / (samples * 0.5)){
        ///for(float y = -offset; y < offset; y += offset / (samples * 0.5)){
            //for(float z = -offset; z < offset; z += offset / (samples * 0.5)){
                // �÷������� ���� ������������?
                //float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; 
                // ���ڷ����ǹ�һ���ģ�����?ͳһ
                //closestDepth *= far_plane; 
                //if(currentDepth - bias > closestDepth)
                    //shadow += 1.0;
                //} } }
    //shadow /= (samples * samples * samples);

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

    // ���Ƚϵ�ƬԪ�����?
    float closestDepth = texture(depthMap, fragToLight).r; 
    float currentDepth = length(fragToLight);

    //vec3 color=texture(texture_diffuse1, frag_in.TexCoords).rgb;

    vec3 color=vec3(closestDepth);
    return  vec4(color, 1.0);
}

void main()
{           
    // ��ɫ/����
    //vec3 color  =vec3(0.4);
    vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;
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
    float shadow = shadows ? ShadowCalculation(frag_in.FragPos) : 0.0;// �Ƿ�����Ӱ                  
    shadow = min(shadow, 0.75);

    // �����ⲻ����ӰӰ��
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
    FragColor = vec4(lighting, 1.0f);

    // Debug�� ��Ӱ��ֵ��
    //lighting=(1.0 - shadow)*vec3(1.0);
    //FragColor = DebugShowDepth(frag_in.FragPos);

    
}