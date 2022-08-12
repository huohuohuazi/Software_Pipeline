#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} frag_in;

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool shadows;// 是否绘制阴影

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 光源的投影空间坐标--》z轴归一化--》片元深度，深度越大 离光源越远
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // NDC坐标转化
    projCoords = projCoords * 0.5 + 0.5;
    // 获得待比较片元的深度
    float currentDepth = projCoords.z;

    // 采样深度贴图，获取离光源最近点的位置
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    
    // 获取bias
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    //float bias=0.05;
    
    // PCF
    // 思路是对shadowMap多次采样取平均
    float shadow = 0.0;
    // textureSize(shadowMap, 0) ： 0级mipmap的vec2类型的宽和高
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            // projCoords.xy + vec2(x, y) * texelSize实现texcoord坐标偏移，也就是中心采样点周围共9个像素的uv坐标
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            // 若 片元深度 大于 深度贴图，则说明片元与光源之间有物体阻挡，该片元处于阴影中
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // 相机视野外的默认为非阴影
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
    // 底色/纹理
    vec3 color = texture(diffuseTexture, frag_in.TexCoords).rgb;
    vec3 normal = normalize(frag_in.Normal);
    vec3 lightColor = vec3(0.4);
    // 环境光
    vec3 ambient = 0.2 * color;
    // 漫反射
    vec3 lightDir = normalize(lightPos - frag_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // 高光反射(Blinn_Phong光照模型)
    vec3 viewDir = normalize(viewPos - frag_in.FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 阴影
    float shadow = shadows ? ShadowCalculation(frag_in.FragPosLightSpace) : 0.0;// 是否开启阴影                  
    shadow = min(shadow, 0.75);

    // 环境光不受阴影影响
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
    // Debug用 阴影二值化
    // lighting=(1.0 - shadow)*vec3(1.0);

    FragColor = vec4(lighting, 1.0f);
}