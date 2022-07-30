#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient项
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;// 环境光强*灯光颜色
  	
    // diffuse项
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);// 这里小改一下，用半lambert模型
    float diff = max(dot(norm, lightDir)*0.5+0.5, 0.0);
    vec3 diffuse = diff * lightColor;// 漫反射光强*灯光颜色
    
    // specular项
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);// 视角方向
    vec3 reflectDir = reflect(-lightDir, norm); // 高光反射方向
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; // 高光反射范围*光强*灯光颜色
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 