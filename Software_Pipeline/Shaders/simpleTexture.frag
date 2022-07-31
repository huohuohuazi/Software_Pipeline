#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoords;

uniform Material material;// 材质信息
uniform Light light;// 灯光信息
uniform vec3 viewPos; // 视角方向


void main()
{    
    // 环境光项，这里传入的是vec4(0.1)
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
  	
    // 漫反射项
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    // 小改了下，用half_Lambert模型
    float diff = max(dot(norm, lightDir)*0.5+0.5, 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;  
    
    // 高光反射项
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;  

        
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}