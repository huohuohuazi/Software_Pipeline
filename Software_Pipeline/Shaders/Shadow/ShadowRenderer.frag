#version 330 core


out vec4 FragColor;

in vert_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} frag_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
//uniform vec3 lightDir;

 // ������Ӱ
 float ShadowCalculation(vec4 fragPosLightSpace)
{
    // ִ��͸�ӳ���
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // ��һ����NDC���꣬Ҳ���ǻ�ȡƬԪ�ڹ�Դ��Ļ�ռ������
    projCoords = projCoords * 0.5 + 0.5;
    // ��ShadowMap���� 
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // ���浱ǰƬԪ����ȣ�����Ļ�ռ���ͬ�����ShadowMap�Ƚϡ������ȴ���Shadowmap����Ϊ��Ӱ
    // ����ڹ�Դ��Զ��Ҳ����˵��Դ�͸�ƬԪ�м��ж�������
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    // ������
    return shadow;
}


void main()
{
    // ����ɫ
    vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    vec3 lightColor = vec3(0.3);
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    vec3 viewDir = normalize(viewPos - frag_in.FragPos);// �ӽǷ���
    vec3 reflectDir = reflect(-lightDir, normal); // �߹ⷴ�䷽��


    // ambient��
    vec3 ambient = 0.3 * lightColor;// ������ǿ*�ƹ���ɫ
  	

    // diffuse��
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;// �������ǿ*�ƹ���ɫ
    

    // specular��
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = lightColor * spec;
        
    //��Ӱ
    float shadow = ShadowCalculation(frag_in.FragPosLightSpace);  
    // �����ⲻ��shadowӰ��
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;   
    FragColor = vec4(lighting, 1.0);
} 