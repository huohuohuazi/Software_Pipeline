#version 430 core

layout (std140,binding = 2) uniform PointLightinfo
{
    float K_ambient;   //4  0
    vec3 lightPos;     //16 16
    vec3 lightColor;   //16 32
    bool IsBlinnPhong; //4  48
};// Size=52

out vec4 FragColor;

in vert_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} frag_in;

uniform sampler2D texture_diffuse1;
uniform vec3 cameraPos; 

 
void main()
{
    // ����ɫ
    vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;


    // ambient��
    vec3 ambient = K_ambient * color;// ������ǿ*�ƹ���ɫ
  	

    // diffuse��
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * color;// �������ǿ*�ƹ���ɫ
    


    // specular��
    vec3 viewDir = normalize(cameraPos - frag_in.FragPos);// �ӽǷ���
    vec3 reflectDir = reflect(-lightDir, normal); // �߹ⷴ�䷽��
    float spec=0;
    if(IsBlinnPhong)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    vec3 specular = lightColor * spec;
        
        //vec3 result = ambient;
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
} 