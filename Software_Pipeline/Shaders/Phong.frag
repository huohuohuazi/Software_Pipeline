#version 430 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    // ambient��
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;// ������ǿ*�ƹ���ɫ
  	
    // diffuse��
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);// ����С��һ�£��ð�lambertģ��
    float diff = max(dot(norm, lightDir)*0.5+0.5, 0.0);
    vec3 diffuse = diff * lightColor;// �������ǿ*�ƹ���ɫ
    
    // specular��
    float specularStrength = 0.5; 
    vec3 viewDir = normalize(viewPos - FragPos);// �ӽǷ���
    vec3 reflectDir = reflect(-lightDir, norm); // �߹ⷴ�䷽��
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor; // �߹ⷴ�䷶Χ*��ǿ*�ƹ���ɫ
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 