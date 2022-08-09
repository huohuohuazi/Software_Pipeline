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

 // 计算阴影
 float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 归一化到NDC坐标，也就是获取片元于光源屏幕空间的坐标
    projCoords = projCoords * 0.5 + 0.5;
    // 对ShadowMap采样 
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // 保存当前片元的深度，于屏幕空间中同坐标的ShadowMap比较。如果深度大于Shadowmap，则为阴影
    // 相较于光源更远，也就是说光源和该片元中间有东西挡着
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;
    // 如果相机
    return shadow;
}


void main()
{
    // 基础色
    vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    vec3 lightColor = vec3(0.3);
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    vec3 viewDir = normalize(viewPos - frag_in.FragPos);// 视角方向
    vec3 reflectDir = reflect(-lightDir, normal); // 高光反射方向


    // ambient项
    vec3 ambient = 0.3 * lightColor;// 环境光强*灯光颜色
  	

    // diffuse项
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;// 漫反射光强*灯光颜色
    

    // specular项
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = lightColor * spec;
        
    //阴影
    float shadow = ShadowCalculation(frag_in.FragPosLightSpace);  
    // 环境光不受shadow影响
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;   
    FragColor = vec4(lighting, 1.0);
} 