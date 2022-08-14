#version 330 core

out vec4 FragColor;
in vec2 TexCoords;

// 几何阶段那就采样过了，这里直接从gAlbedoSpec(ColorAndSpecularCBO)中获取即可
// uniform sampler2D texture_diffuse1;

uniform samplerCube depthMap;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 lightPos;
    //vec3 lightColor;
    
    // float Linear;
    // float Quadratic;
};

Light light;

// uniform vec3 lightPos;
uniform vec3 viewPos;

uniform float far_plane;
uniform bool shadows;// 是否绘制阴影

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

float ShadowCalculation(vec3 fragPos)
{
    // 片元到光源向量
    vec3 fragToLight = fragPos - light.lightPos; 

    // 待比较的片元的深度
    float currentDepth = length(fragToLight);

    // PCF优化：使用20个偏移量代替64次采样
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;

    float viewDistance = length(viewPos - fragPos);
    // 偏移半径
    // float diskRadius = 0.05;
    //根据观察者的距离来增加偏移半径了
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

// Debug，用于查看shadwmap有没有问题
vec4 DebugShowDepth(vec3 fragPos)
{
    // 片元到光源向量
    vec3 fragToLight = fragPos - light.lightPos; 

    // 待比较的片元的深度
    float closestDepth = texture(depthMap, fragToLight).r; 
    float currentDepth = length(fragToLight);

    //vec3 color=texture(texture_diffuse1, frag_in.TexCoords).rgb;

    vec3 color=vec3(closestDepth);
    return  vec4(color, 1.0);
}

void main()
{           
    vec3 FragPos = texture(gPosition, TexCoords).rgb;// 世界坐标
    vec3 Normal = texture(gNormal, TexCoords).rgb;// 
    vec3 Color = texture(gAlbedoSpec, TexCoords).rgb;// 底色/纹理

    // 底色/纹理
    //vec3 color  =vec3(0.4);
    //vec3 color = texture(texture_diffuse1, frag_in.TexCoords).rgb;
    //vec3 normal = normalize(frag_in.Normal);
    vec3 lightColor = vec3(0.4);
    // 环境光
    vec3 ambient = 0.2 * Color;
    // 漫反射
    vec3 lightDir = normalize(light.lightPos - FragPos);
    float diff = max(dot(lightDir, Normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // 高光反射(Blinn_Phong光照模型)
    vec3 viewDir = normalize(viewPos - FragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(Normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    // 阴影
    float shadow = shadows ? ShadowCalculation(FragPos) : 0.0;// 是否开启阴影                  
    shadow = min(shadow, 0.75);

    // 环境光不受阴影影响
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * Color;
    
    FragColor = vec4(lighting, 1.0f);
    
    // Debug用 阴影二值化
    //lighting=(1.0 - shadow)*vec3(1.0);
    //FragColor = DebugShowDepth(FragPos);

    
}