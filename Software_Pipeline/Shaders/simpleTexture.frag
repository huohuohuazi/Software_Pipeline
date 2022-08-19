

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

// ƽ�й�
struct DirLight {
    // ֻ�з���û��λ��
    vec3 direction;

    // ����ǿ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// ���Դ
struct PointLight {
    vec3 position;
    
    // ����˥��
    float constant;
    float linear;
    float quadratic;

    // ����ǿ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// �۹��
struct SpotLight {
    vec3 position;
    vec3 direction;

    // ��Դ�Ĺ���˥��
    // Fatt=1/(Kc+Kl*d+Kq*d*d)����ͬ�Ĺ�Դ��Χ��Ӧ��ͬ�Ĳ���
    float constant;// ������
    float linear;// һ����
    float quadratic;// ������

    // �۹��
    float cutOff;// ���й��
    float outerCutOff;// ���й��
  
    // ����ǿ��
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

//vec4 position;��ʽopenglʹ��vec4��������ƽ�й�����Դ����direction.w����Ϊ0ʱΪƽ�й⣬������λ��
// ��wΪ1ʱ����λ�ã�����������Ҫ�ֶ�ָ��������
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


uniform Material material;// ������Ϣ
uniform vec3 viewPos; // �ӽǷ��򣬾�����������ֵ������cos����gpu�Ǻܴ�Ŀ���

//uniform�����Զ�̬�����ڴ棬�����Ҫ��ǰָ������

#define NR_POINT_LIGHTS 4// Ԥ����ָ���ʾҪ�����ĸ����Դ����
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;// һ��ƽ�й⣬��Ϊ̫����
uniform SpotLight spotLight;// �۹��


void main()
{    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    // ���μ��㲻ͬ���͵Ĺ�Դ
    // ƽ�й�
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // ���Դ
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    // �۹��
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0,1.0,1.0, 1.0);
}


// ƽ�й����
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // ������
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);
    
    // �߹�
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// ���Դ����
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // ��������ɫ
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // ˥��
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// �۹�Ƽ���
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // �߹ⷴ��
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // ˥��
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // ƽ���߽�
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //clamp:if (anle<theta)-->intensity=1;
    //      if (theta<angle<outertheta)-->intensity=��ֵ;
    //      if (anle>outertheta)-->intensity=0

    // ���
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

