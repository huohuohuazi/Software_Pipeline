#version 330 core

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

// 平行光
struct DirLight {
    // 只有方向，没有位置
    vec3 direction;

    // 光照强度
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// 点光源
struct PointLight {
    vec3 position;
    
    // 光照衰减
    float constant;
    float linear;
    float quadratic;

    // 光照强度
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  

// 聚光灯
struct SpotLight {
    vec3 position;
    vec3 direction;

    // 光源的光照衰减
    // Fatt=1/(Kc+Kl*d+Kq*d*d)，不同的光源范围对应不同的参数
    float constant;// 常数项
    float linear;// 一次项
    float quadratic;// 二次项

    // 聚光灯
    float cutOff;// 内切光角
    float outerCutOff;// 外切光角
  
    // 光照强度
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

//vec4 position;老式opengl使用vec4可以区分平行光与点光源，当direction.w分量为0时为平行光，不考虑位置
// 当w为1时考虑位置，但会面临需要手动指定的问题
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);


uniform Material material;// 材质信息
uniform vec3 viewPos; // 视角方向，尽量传入余弦值，否则cos对于gpu是很大的开销

//uniform不可以动态分配内存，因此需要提前指定数量

#define NR_POINT_LIGHTS 4// 预处理指令，表示要传递四个点光源进来
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform DirLight dirLight;// 一个平行光，作为太阳光
uniform SpotLight spotLight;// 聚光灯


void main()
{    
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);

    // 依次计算不同类型的光源
    // 平行光
    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    // 点光源
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    
    // 聚光灯
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    FragColor = vec4(result, 1.0);
    //FragColor = vec4(1.0,1.0,1.0, 1.0);
}


// 平行光计算
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    
    // 漫反射
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);
    
    // 高光
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

// 点光源计算
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// 聚光灯计算
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    
    // 高光反射
    float diff = max(dot(normal, lightDir)*0.5+0.5, 0.0);    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // 衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // 平滑边界
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    //clamp:if (anle<theta)-->intensity=1;
    //      if (theta<angle<outertheta)-->intensity=插值;
    //      if (anle>outertheta)-->intensity=0

    // 混合
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

